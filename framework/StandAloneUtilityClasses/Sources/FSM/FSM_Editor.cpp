#include "FSM/FSM_Editor.h"

#include "FSM/FSM_State.h"
#include "ModuleFileManager.h"

#include "kstlstring.h"
#include "kstlvector.h"



#include <algorithm>
#include <numeric>

#include "addons/imguinodegrapheditor/imguinodegrapheditor.h"


#include "Watchdog.h"

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#else
#define ShellExecute(...)
#endif

using namespace std::string_literals;

#if !(defined(_DEBUG) || defined(KIGS_TOOLS))
#error Need to compile in StaticDebug or StaticReleaseTools to use the FSM_Editor
#endif

static bool operator==(const ImVec2& left, const ImVec2& right)
{
	return (Point2D)left == (Point2D)right;
}

// Internals
namespace internals_node_editor
{
	enum NodeType
	{
		NodeType_State = 0,
		NodeType_FSM,
		NodeType_Exit,
		NodeType_COUNT,
	};

	static const char* nodeTypes[] = { "State", "SubFSM", "ExitNode" };

	ImGui::Node* NodeFSMFactory(int type, const ImVec2& pos, ImGui::NodeGraphEditor* editor);
	void NodeFSMNodeCallback(ImGui::Node* node, ImGui::NodeGraphEditor::NodeState state, ImGui::NodeGraphEditor& editor);
	bool NodeFSMLinkCallback(ImGui::NodeLink* link, ImGui::NodeGraphEditor::LinkState state, ImGui::NodeGraphEditor& editor);
	void NodeFSMContextMenuCallback(ImGui::Node* node, ImGui::NodeGraphEditor& editor);


	struct NameAndCode
	{
		kstl::string name;
		kstl::string code;
	};

	struct FSMEditor : public ImGui::NodeGraphEditor
	{
		FSMEditor()
		{
			allowOnlyOneLinkPerInputSlot = false;
			avoidCircularLinkLoopsInOut = false;
			show_left_pane = false;
		}

		FSM* associated_fsm = nullptr;

		kstl::string toXML();
		kstl::vector<NameAndCode> lua_methods();

		virtual void render_custom_header() override;
	};

	struct FSMAction
	{
		bool is_lua = false;
		kstl::string method_name;
		kstl::string lua_code;

		bool render(const char* name, ImGui::NodeGraphEditor* editor)
		{
			ImGui::PushID(&is_lua);
			ImGui::Checkbox("", &is_lua);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Toggle %s as a lua method", name);
			ImGui::SameLine();
			bool result = false;
			if (is_lua)
			{
				result = ImGui::Button("Edit lua...");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(lua_code.c_str());

				if (result)
				{
					ModuleFileManager* fm = (ModuleFileManager*)KigsCore::Instance()->GetMainModuleInList(FileManagerModuleCoreIndex);
					
					fm->SaveFile("temp.lua", (unsigned char*)lua_code.c_str(), lua_code.size());

					ShellExecute(NULL, "open", "temp.lua", NULL, NULL, SW_SHOWNORMAL);

					static_cast<NodeEditor*>(editor->user_ptr2)->in_lua_edit = true;

					wd::unwatch("temp.lua");
					wd::watch("temp.lua", [this](const ci::fs::path& path)
					{
						unsigned long len;
						CoreRawBuffer* buf = ModuleFileManager::LoadFileAsCharString("temp.lua", len);
						if (buf)
						{
							lua_code = buf->buffer();
							buf->Destroy();
						}
							
					});
				}

				ImGui::SameLine();
				ImGui::Text(name);
			}
			else
			{
				ImGui::InputText(name, method_name);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Method name");
			}
			ImGui::PopID();
			return result;
		}

	};

	

	struct NodeBase : public ImGui::Node
	{
		kstl::vector<kstl::string> outputs;
		kstl::vector<FSMAction> guard_methods;

		FSMEditor* owner = nullptr;




		NodeBase(FSMEditor* lowner) : ImGui::Node()
		{
			owner = lowner;
			outputs.push_back("Add...");
			guard_methods.emplace_back();
		}

		kstl::string toXML();
		virtual kstl::string toXML_inside() { return {}; };
		virtual kstl::vector<NameAndCode> lua_methods()
		{
			kstl::vector<NameAndCode> result;
			for (u32 i = 0; i < outputs.size() - 1; ++i)
			{
				if (guard_methods[i].is_lua)
					result.push_back(NameAndCode{ guard_methods[i].method_name, guard_methods[i].lua_code });
			}
			return result;
		};

		virtual void onCopied(ImGui::Node* srcnode) override
		{
			outputs = static_cast<NodeBase*>(srcnode)->outputs;
			owner = static_cast<NodeBase*>(srcnode)->owner;
		}

		void rename(const kstl::string& name)
		{
			memcpy(Name, name.c_str(), std::min<size_t>(name.length() + 1, IMGUINODE_MAX_NAME_LENGTH));
			Name[IMGUINODE_MAX_NAME_LENGTH - 1] = 0;
		}

		virtual bool render(float nodeWidth) override
		{
			auto accumulator = [](const kstl::string& acc, const kstl::string& el)
			{
				return acc.empty() ? el : acc + ";"s + el;
			};

			kstl::string output_str = std::accumulate(outputs.begin(), outputs.end(), ""s, accumulator);

			init(Name, Pos, " ", output_str.c_str(), typeID);

			bool in_edit = ImGui::Node::render(nodeWidth);

			if (ImGui::Button("Edit transitions"))
			{
				getNodeEditor()->node_to_edit = this;
			}
			//if(owner->associated_fsm && CharToID::GetID(getName()) == owner->associated_fsm->getCurrentStateID())
				//ImGui::TextColored(ImColor(255, 0, 0), "ACTIVE STATE");	

			return in_edit;
		}

		NodeEditor* getNodeEditor()
		{
			return static_cast<NodeEditor*>(owner->user_ptr2);
		}

		NodeFSM* getParentNodeFSM()
		{
			return owner ? static_cast<NodeFSM*>(owner->user_ptr1) : nullptr;
		}

		NodeFSM* getRootNodeFSM()
		{
			return getNodeEditor()->root_fsm;
		}

		FSM* getRootFSM()
		{
			return getNodeEditor()->associated_fsm;
		}
	};


	struct NodeState : public NodeBase
	{
		using NodeBase::NodeBase;
		FSMAction actions[3];

		virtual void onCopied(ImGui::Node* srcnode) override
		{
			NodeBase::onCopied(srcnode);
			actions[0] = static_cast<NodeState*>(srcnode)->actions[0];
			actions[1] = static_cast<NodeState*>(srcnode)->actions[1];
			actions[2] = static_cast<NodeState*>(srcnode)->actions[2];
		}

		virtual bool render(float nodeWidth) override
		{
			bool in_edit = NodeBase::render(nodeWidth);

			const char* actions_names[3];
			actions_names[STATE_ACTION_TYPE::Update] = "Update";
			actions_names[STATE_ACTION_TYPE::Begin] = "Begin";
			actions_names[STATE_ACTION_TYPE::End] = "End";

			for (int i = 0; i < 3; ++i)
			{
				actions[i].render(actions_names[i], owner);
			}


			return in_edit;
		}


		virtual kstl::string toXML_inside() override
		{
			kstl::string result = "\"actions\" : [";
			for (int i = 0; i < 3; ++i)
			{
				result += "\"" + actions[i].method_name + "\"";
				if (i < 2) result += ", ";
			}
			result += "]\n";
			return result;
		}

		virtual kstl::vector<NameAndCode> lua_methods()
		{
			auto result = NodeBase::lua_methods();
			for (u32 i = 0; i < 3; ++i)
			{
				if (actions[i].is_lua)
					result.push_back(NameAndCode{ actions[i].method_name, actions[i].lua_code });
			}
			return result;
		};

		static NodeState* Create(const ImVec2& pos, FSMEditor* editor)
		{
			NodeState* node = new (ImGui::MemAlloc(sizeof(NodeState))) NodeState(editor);
			node->Pos = pos;
			node->init("Unnamed State", pos, 0, 0, NodeType_State);
			return node;
		}
	};


	struct NodeFSM : public NodeBase
	{
		
		NodeFSM(FSMEditor* editor) : NodeBase(editor)
		{
			if(editor)
				fsm_editor.user_ptr2 = editor->user_ptr2;
		}

		//NodeFSM* parent_fsm;
		FSMEditor  fsm_editor;

		NodeBase* initState = nullptr;
		bool advanced = false;
		kstl::string className = "FSM_State";

		virtual bool canBeCopied() const override
		{
			return false;
		}

		void render_advanced_controls()
		{
			ImGui::AlignFirstTextHeightToWidgets();
			ImGui::Text("Class name");
			ImGui::SameLine();
			ImGui::InputText("###input_class_name", className);
		}

		virtual bool render(float nodeWidth) override
		{
			bool in_edit = NodeBase::render(nodeWidth);
			if (ImGui::Button("\n  Scope to this  \n\n"))
			{
				getNodeEditor()->current_editor = &fsm_editor;
			}
			ImGui::Text("Initial State : %s", initState ? initState->getName() : "*unset*");
			return in_edit;
		}

		virtual kstl::string toXML_inside() override
		{
			kstl::string result = R"W("instance" : {"infos" : [")W";
			result += className + "\", \"" + getName() + "\", \"" + (initState ? initState->getName() : "") + "\"],\n";
			result += "\"StateMap\" : {\n";

			result += fsm_editor.toXML();

			result += "}\n}\n";
			return result;
		}

		virtual kstl::vector<NameAndCode> lua_methods()
		{
			auto result = NodeBase::lua_methods();
			auto append = fsm_editor.lua_methods();;
			result.insert(result.end(), append.begin(), append.end());
			return result;
		};


		static NodeFSM* Create(const ImVec2& pos, FSMEditor* editor, bool is_root = false)
		{
			NodeFSM* node = new (ImGui::MemAlloc(sizeof(NodeFSM))) NodeFSM(editor);
			node->Pos = pos;
			node->init("Unnamed SubFSM", pos, 0, 0, NodeType_FSM);
			node->fsm_editor.registerNodeTypes(nodeTypes, NodeType_COUNT, &NodeFSMFactory);
			node->fsm_editor.setNodeCallback(&NodeFSMNodeCallback);
			node->fsm_editor.setLinkCallback(&NodeFSMLinkCallback);
			node->fsm_editor.setContextMenuCallback(&NodeFSMContextMenuCallback);
			
			if (is_root)
				node->fsm_editor.registerNodeTypeMaxAllowedInstances(NodeType_Exit, 0);

			node->fsm_editor.user_ptr1 = node;
			return node;
		}
	};


	struct NodeExit : public NodeBase
	{
		using NodeBase::NodeBase;
		NodeBase* exit_state = nullptr;

		virtual void onCopied(ImGui::Node* srcnode) override
		{
			exit_state = static_cast<NodeExit*>(srcnode)->exit_state;
		}

		virtual bool render(float nodeWidth) override
		{
			bool in_edit = ImGui::Node::render(nodeWidth);
			if (!getParentNodeFSM()) return in_edit;
			auto parent_fsm = getParentNodeFSM()->getParentNodeFSM();
			if (!parent_fsm) return in_edit;

			char itemlist[1024];
			int index = 0;
			ImVector<ImGui::Node*> nodes;

			parent_fsm->fsm_editor.getAllNodesOfType(NodeType_State, &nodes, false);
			parent_fsm->fsm_editor.getAllNodesOfType(NodeType_FSM, &nodes, false);
			for (auto n : nodes)
			{
				if (n == static_cast<ImGui::Node*>(exit_state))
					break;
				++index;
			}

			struct ComboData
			{
				ImVector<ImGui::Node*>* nodes;
				NodeFSM* ref_fsm;
				NodeFSM* parent_fsm;
				int offset;
			};
			ComboData data;
			data.nodes = &nodes;
			data.ref_fsm = getParentNodeFSM();
			data.parent_fsm = parent_fsm;
			data.offset = 0;

			ImGui::Combo("Exit state", &index, [](void* data, int idx, const char **out_text)->bool
			{
				auto& ds = *static_cast<ComboData*>(data);
				ImGui::Node* node = (*ds.nodes)[ds.offset + idx];
				if (node == static_cast<ImGui::Node*>(ds.ref_fsm))
				{
					ds.offset = 1;
					node = (*ds.nodes)[ds.offset + idx];
				}
				*out_text = node->getName();
				return true;
			}, &data, nodes.size() - 1);

			if (index >= 0 && index < nodes.size())
				exit_state = static_cast<NodeBase*>(nodes[index]);
			else
				exit_state = nullptr;

			return in_edit;
		}

		static NodeExit* Create(const ImVec2& pos, FSMEditor* editor)
		{
			NodeExit* node = new (ImGui::MemAlloc(sizeof(NodeExit))) NodeExit(editor);
			node->Pos = pos;
			node->init("Exit SubFSM", pos, " ", 0, NodeType_Exit);
			return node;
		}
	};


	// CallBacks
	ImGui::Node* NodeFSMFactory(int type, const ImVec2& pos, ImGui::NodeGraphEditor* editor)
	{
		switch (type)
		{
		case NodeType_State: return NodeState::Create(pos, static_cast<FSMEditor*>(editor));
		case NodeType_FSM: return NodeFSM::Create(pos, static_cast<FSMEditor*>(editor), false);
		case NodeType_Exit: return NodeExit::Create(pos, static_cast<FSMEditor*>(editor));
		default: assert(0);
		}
		return nullptr;
	}


	void NodeFSMNodeCallback(ImGui::Node* node, ImGui::NodeGraphEditor::NodeState state, ImGui::NodeGraphEditor& editor)
	{
		NodeBase* base = static_cast<NodeBase*>(node);
		
		NodeFSM* parent_fsm = base->getParentNodeFSM();
		int type = node->getType();

		if (state == ImGui::NodeGraphEditor::NodeState::NS_ADDED)
		{
			if (parent_fsm && !parent_fsm->initState && type != NodeType_Exit)
				parent_fsm->initState = base;
		}

		if (state == ImGui::NodeGraphEditor::NodeState::NS_DELETED)
		{
			if (parent_fsm->initState == node)
				parent_fsm->initState = nullptr;
		}
	}


	bool NodeFSMLinkCallback(ImGui::NodeLink* link, ImGui::NodeGraphEditor::LinkState state, ImGui::NodeGraphEditor& editor)
	{
		if (state == ImGui::NodeGraphEditor::LinkState::LS_ADDED)
		{
			NodeBase* from = static_cast<NodeBase*>(link->InputNode);
			NodeBase* to = static_cast<NodeBase*>(link->OutputNode);
			
			if (editor.isLinkPresent(from, link->InputSlot, nullptr, -1))
				return false;

			if(to->getType() == NodeType_Exit)
				link->link_color = ImColor(255, 0, 0, 255);

			if (link->InputSlot == from->outputs.size() - 1)
			{
				from->outputs.push_back("Add...");
				from->guard_methods.emplace_back();
				from->outputs[from->outputs.size() - 2] = "";
				static_cast<NodeEditor*>(static_cast<NodeBase*>(from)->owner->user_ptr2)->node_to_edit = from;
			}
		}

		return true;
	}

	void NodeFSMContextMenuCallback(ImGui::Node* node, ImGui::NodeGraphEditor& editor)
	{
		if (node)
		{
			int type = node->getType();

			if (editor.user_ptr1 && type != NodeType_Exit && ImGui::MenuItem("Set as init state"))
			{
				static_cast<NodeFSM*>(editor.user_ptr1)->initState = static_cast<NodeBase*>(node);
			}
		}
	}


	void FSMEditor::render_custom_header()
	{
		NodeEditor* editor = static_cast<NodeEditor*>(user_ptr2);
		
		if (ImGui::Button("Save..."))
		{
			ImGui::OpenPopup("Save FSM");
		}
		
		ImGui::SameLine();

		NodeFSM* current_fsm = static_cast<NodeFSM*>(user_ptr1);
		NodeFSM* current = current_fsm;
		NodeFSM* stack[32] = {};
		int i = 0;
		while (current && i<32)
		{
			stack[i++] = current;
			current = current->getParentNodeFSM();
		}
		while (i > 0)
		{
			current = stack[--i];
			ImGui::PushID(current);
			ImGui::Text("/"); ImGui::SameLine();
			if (ImGui::Button(current->getName()))
			{
				editor->current_editor = &current->fsm_editor;
			}
			if (i > 0)
				ImGui::SameLine();
			ImGui::PopID();
		}

		ImGui::PushID(current_fsm);
		ImGui::PushItemWidth(200.0f);
		current_fsm->render_advanced_controls();
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("/ Initial State : %s", current_fsm->initState ? current_fsm->initState->getName() : "*unset*");
		ImGui::PopID();

		bool p_open = true;
		if (ImGui::BeginPopupModal("Save FSM", &p_open))
		{
			static kstl::string filename = "fsmtest.xml";
			ImGui::InputText("Filename", filename);

			if (ImGui::Button("Save"))
			{
				NodeFSM* node = static_cast<NodeFSM*>(user_ptr1);

				kstl::string result = "<?xml version=\"1.0\"?>\n<Inst N=\""s + node->getName() + "\" T=\""s + node->className + "\">\n"s;

				auto methods = node->lua_methods();

				for (auto& m : methods)
				{
					result += "<LUA N=\""+ m.name +"\"><![CDATA[\n";
					result += m.code;
					result += "\n]]></LUA>\n";
				}
					
				result += "<Attr T=\"coreitem\" N=\"Description\">\n<![CDATA[\n{ \"StateMap\" : {\n";

				result += node->fsm_editor.toXML();

				result += "\n}\n}\n]]>\n</Attr>\n";

				if (node->initState)
					result += "<Attr N=\"InitState\" V=\""s + node->initState->getName() + "\">\n"s;

				result += "</Inst>\n";

				ModuleFileManager* filemanager = (ModuleFileManager*)KigsCore::Instance()->GetMainModuleInList(FileManagerModuleCoreIndex);
				filemanager->SaveFile(filename.c_str(), (unsigned char*)result.c_str(), result.size());
			}
			ImGui::EndPopup();
		}
	}

	kstl::string NodeBase::toXML()
	{
		kstl::string result = "\""s + getName() + "\" : {\n"s;

		kstl::string inside = toXML_inside();
		result += inside;

		if (outputs.size() > 1)
		{
			if (inside.size())
				result += ",\n";

			result +=
			R"OOO(
"transitions" : 
{
)OOO";

			ImVector<Node*> nodes;
			bool first_transition = true;
			for (u32 i = 0; i < outputs.size() - 1; ++i)
			{
				nodes.clear();
				owner->getOutputNodesForNodeAndSlot(this, i, nodes);
				for (auto n : nodes)
				{
					if (!first_transition) result += ",\n";
					first_transition = false;

					kstl::string state_to = n->getName();
					if (n->getType() == NodeType_Exit)
					{
						NodeBase* state = static_cast<NodeExit*>(n)->exit_state;
						state_to = state ? state->getName() : "";
					}

					result += "\""s + outputs[i] + "\" : [\""s + state_to + "\""s;

					if (!guard_methods[i].method_name.empty())
						result += ", \"" + guard_methods[i].method_name + "\"";

					result += "]";
				}
			}
			result += "}\n";
		}
		result += "}\n";
		return result;


	}

	kstl::string FSMEditor::toXML()
	{
		ImVector<ImGui::Node*> nodes;
		getAllNodesOfType(NodeType_State, &nodes, false);
		getAllNodesOfType(NodeType_FSM, &nodes, false);

		kstl::string result = "";
		bool first_state = true;
		for (auto n : nodes)
		{
			if (!first_state) result += ",\n";
			first_state = false;
			result += static_cast<NodeBase*>(n)->toXML();
		}

		return result;
	}

	kstl::vector<NameAndCode> FSMEditor::lua_methods()
	{
		ImVector<ImGui::Node*> nodes;
		getAllNodesOfType(NodeType_State, &nodes, false);
		getAllNodesOfType(NodeType_FSM, &nodes, false);

		kstl::vector<NameAndCode> result;
		for (auto n : nodes)
		{
			kstl::vector<NameAndCode> methods = static_cast<NodeBase*>(n)->lua_methods();
			result.insert(result.end(), methods.begin(), methods.end());
		}
		return result;
	}

}

using namespace internals_node_editor;

void NodeEditor::init_empty()
{
	if (root_fsm)
		delete root_fsm;
	
	root_fsm = NodeFSM::Create(ImVec2(0, 0), nullptr, true);
	root_fsm->fsm_editor.user_ptr2 = this;
	root_fsm->rename("Root FSM");
	current_editor = &root_fsm->fsm_editor;
}


void NodeEditor::init_rec_with_fsm(FSM* fsm, NodeFSM* node_fsm, const kstl::map<unsigned int, NodeBase*>& parent_nodes)
{
	kstl::string initstatename;
	fsm->getValue(LABEL_TO_ID(InitState), initstatename);
	
	ImVec2 pos(10,10);

	float offsetx = 250;
	float offsety = 150;

	node_fsm->className = fsm->getExactType();

	FSM* root_fsm = static_cast<NodeEditor*>(node_fsm->fsm_editor.user_ptr2)->associated_fsm;

	kstl::map<unsigned int, NodeBase*> nodes;
	for (auto pair : fsm->m_StateMap)
	{
		NodeBase* node = nullptr;
		if (pair.second->isSubFSM())
		{
			auto subfsm = static_cast<NodeFSM*>(node_fsm->fsm_editor.addNode(NodeType_FSM, pos));
			subfsm->fsm_editor.associated_fsm = pair.second->m_State;
			node = subfsm;
		}
		else
		{
			auto state = static_cast<NodeState*>(node_fsm->fsm_editor.addNode(NodeType_State, pos));
			if (pair.second->m_Actions)
			{
				for (int i = 0; i < 3; ++i)
				{
					state->actions[i].method_name = pair.second->m_Actions->m_actionsNames[i];
					if (!state->actions[i].method_name.empty())
					{
						char str[256];
						snprintf(str, 256, "%d§LUA_CODE§%s", CharToID::GetID(state->actions[i].method_name), state->actions[i].method_name.c_str());
						state->actions[i].is_lua = root_fsm->getValue(_S_2_ID(str), state->actions[i].lua_code);
					}
				}
			}
			node = state;
		}
		nodes[pair.first] = node;
		node->rename(pair.second->m_StateName);

		if (pair.second->m_StateName == initstatename)
		{
			node_fsm->initState = node;
		}

		pos.y += offsety;
	}


	kstl::vector<std::pair<NodeBase*, NodeExit*>> exit_links;

	kstl::vector<std::pair<NodeBase*, NodeBase*>> links;

	for (auto pair : fsm->m_StateMap)
	{
		NodeBase* from = nodes[pair.first];
		from->outputs.resize(pair.second->m_Transitions.size() + 1);
		from->guard_methods.resize(pair.second->m_Transitions.size() + 1);
		from->outputs.back() = "Add...";
		int i = 0;
		for (auto transition_pair : pair.second->m_Transitions)
		{
			NodeBase* to = nullptr;
			if (nodes.find(transition_pair.second.m_TransitionStateID) == nodes.end())
			{
				auto exit = static_cast<NodeExit*>(node_fsm->fsm_editor.addNode(NodeType_Exit));
				if (parent_nodes.find(transition_pair.second.m_TransitionStateID) != parent_nodes.end())
				{
					exit->exit_state = parent_nodes.at(transition_pair.second.m_TransitionStateID);
				}
				to = exit;
				exit_links.emplace_back(from, exit);
			}
			else
			{
				to = nodes[transition_pair.second.m_TransitionStateID];		

				if (std::find(links.begin(), links.end(), std::make_pair(to, from)) != links.end())
				{
					float m = std::max(to->Pos.x, from->Pos.x);
					to->Pos.x = m;
					from->Pos.x = m;
				}
				else if (from->Pos.x >= to->Pos.x)
				{
					to->Pos.x = from->Pos.x + offsetx;
				}
				links.emplace_back(from, to);
			}

			node_fsm->fsm_editor.addLink(from, i, to, 0);
			from->outputs[i] = transition_pair.second.m_NotificationName;
			from->guard_methods[i].method_name = transition_pair.second.m_GuardMethodName;

			if (!from->guard_methods[i].method_name.empty())
			{
				char str[256];
				snprintf(str, 256, "%d§LUA_CODE§%s", CharToID::GetID(from->guard_methods[i].method_name), from->guard_methods[i].method_name.c_str());
				from->guard_methods[i].is_lua = root_fsm->getValue(_S_2_ID(str), from->guard_methods[i].lua_code);
			}
			++i;
		}



		if (pair.second->isSubFSM())
		{
			init_rec_with_fsm(pair.second->m_State, static_cast<NodeFSM*>(from), nodes);
		}
	}

	for (auto link : exit_links)
	{
		link.second->Pos = link.first->Pos + ImVec2(offsetx, 0);
	}

}

void NodeEditor::init_with_FSM_instance(FSM* fsm)
{
	if (root_fsm)
		delete root_fsm;

	associated_fsm = fsm;

	root_fsm = NodeFSM::Create(ImVec2(0, 0), nullptr, true);
	root_fsm->fsm_editor.user_ptr2 = this;
	root_fsm->fsm_editor.associated_fsm = fsm;

	root_fsm->rename(fsm->getName());
	current_editor = &root_fsm->fsm_editor;
	

	init_rec_with_fsm(fsm, root_fsm, {});
}

void NodeEditor::render()
{
	if (current_editor)
	{
		current_editor->render();
	}

	auto center = ImGui::GetWindowPos() + ImGui::GetWindowSize() / 2;
	bool p_open = true;

	if (node_to_edit)
	{
		node_in_edit = node_to_edit;
		ImGui::OpenPopup("Edit transitions");
		node_to_edit = nullptr;
	}

	if (in_lua_edit)
	{
		in_lua_edit = false;
		ImGui::OpenPopup("Lua in edit...");
	}

	if (ImGui::BeginPopup("Edit transitions"))
	{
		for (int i = 0; i < node_in_edit->outputs.size() - 1; ++i)
		{
			ImGui::PushID(&node_in_edit->outputs[i]);

			ImGui::InputText("", node_in_edit->outputs[i]);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Notification name");
			ImGui::SameLine();
			ImGui::Text("Notification name");
			node_in_edit->guard_methods[i].render("Guard method", node_in_edit->owner);
			ImGui::PopID();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Lua in edit..."))
	{
		if (ImGui::Button("I'm done editing !"))
		{
			wd::unwatch("temp.lua");
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

}
