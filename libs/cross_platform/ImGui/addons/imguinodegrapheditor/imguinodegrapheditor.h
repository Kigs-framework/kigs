/* 	Refactoring from https://github.com/ocornut/imgui/issues/306
	It's basically the same exact code with a few modifications (and tons of additions)
*/

#ifndef IMGUINODEGRAPHEDITOR_H_
#define IMGUINODEGRAPHEDITOR_H_

#ifndef IMGUI_API
#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#endif //IMGUI_API

/*
 *   // Basic usage:
	// Here we create a window (please skip if already in a window)
	ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Example: Custom Node Graph", NULL))
	{
		ImGui::TestNodeGraphEditor();   // see its code for further info

	}
	ImGui::End();
*/

// TODO:
/*
-> DONE - Implement a copy/paste functionality for standard Nodes (i.e. nodes that use FieldInfo)
-> DONE - Load/Save NodeGraphEditor Style.
-> DONE - Serialization/Deserialization of the whole NodeGraphEditor + Nodes
-> DONE - Add node clipping: node links are not culled at all, but it's better than nothing.
-> DONE - Adjust zooming.
		  NOW ZOOMING WORKS PROPERLY ONLY IF ImGui::GetIO().FontAllowUserScaling = false,
		  (otherwise there's a BAD fallback).
-> DONE - Nodes links are culled too (not sure if t's faster... it should).

-> DONE - Added multiple node selection plus the concept of 'active node' (that is actually not used by the current code).
-> DONE - Added link hovering (holding SHIFT down) and direct link deletion (SHIFT down + LMB).
-> DONE - Added node name renaming with double LMB click.

-> Add/Adjust/Fix more FieldTypes. TODO! And test/fix FT_CUSTOM field type too.
*/


namespace ImGui
{

	class Node
	{
	public:
		virtual ~Node() {}
		mutable void* user_ptr;
		mutable int userID;
		inline const char* getName() const { return Name; }
		inline int getType() const { return typeID; }
		inline int getNumInputSlots() const { return InputsCount; }
		inline int getNumOutputSlots() const { return OutputsCount; }
		inline void setOpen(bool flag) { isOpen = flag; }

		ImVec2  Pos;

	protected:
		
		// virtual methods
		virtual bool render(float nodeWidth) // should return "true" if the node has been edited and its values modified (to fire "edited callbacks")
		{
			return false;
		}
		virtual const char* getTooltip() const { return NULL; }
		virtual const char* getInfo() const { return NULL; }
		virtual void onEdited() {}  // called (a few seconds) after the node has been edited
		virtual void onCopied(ImGui::Node* src_copy) {}  // called after the node fileds has been copied from another node
		virtual void onLoaded() {}  // called after the node has been loaded (=deserialized from file)
		virtual bool canBeCopied() const { return true; }
		// called on a class basis to set the default colors different from the ones defined in NodeGraphEditor::GetStyle()
		// [but on an instance basis these colors can still be overridden using the protected fields defined below, or better NodeGraphEditor::overrideNodeTitleBarColors(...)]
		virtual void getDefaultTitleBarColors(ImU32& defaultTitleTextColorOut, ImU32& defaultTitleBgColorOut, float& defaultTitleBgColorGradientOut) const
		{
			defaultTitleTextColorOut = defaultTitleBgColorOut = 0;  // 0 -> use values defined in NodeGraphEditor::GetStyle()
			defaultTitleBgColorGradientOut = -1;                    // -1 -> use value defined in NodeGraphEditor::GetStyle()
		}

		// some constants
#   ifndef IMGUINODE_MAX_NAME_LENGTH
#   define IMGUINODE_MAX_NAME_LENGTH 32
#   endif //IMGUINODE_MAX_NAME_LENGTH
#   ifndef IMGUINODE_MAX_INPUT_SLOTS
#   define IMGUINODE_MAX_INPUT_SLOTS 8
#   endif //IMGUINODE_MAX_INPUT_SLOTS
#   ifndef IMGUINODE_MAX_OUTPUT_SLOTS
#   define IMGUINODE_MAX_OUTPUT_SLOTS 8
#   endif //IMGUINODE_MAX_OUTPUT_SLOTS
#   ifndef IMGUINODE_MAX_SLOT_NAME_LENGTH
#   define IMGUINODE_MAX_SLOT_NAME_LENGTH 12
#   endif //IMGUINODE_MAX_SLOT_NAME_LENGTH
	// ---------------

		ImVec2 Size;
		char    Name[IMGUINODE_MAX_NAME_LENGTH];
		int     InputsCount, OutputsCount;
		char    InputNames[IMGUINODE_MAX_INPUT_SLOTS][IMGUINODE_MAX_SLOT_NAME_LENGTH];
		char    OutputNames[IMGUINODE_MAX_OUTPUT_SLOTS][IMGUINODE_MAX_SLOT_NAME_LENGTH];
		mutable float startEditingTime; // used for Node Editing Callbacks
		mutable bool isOpen;
		mutable bool isSelected;
		int typeID;
		float baseWidthOverride;
		bool mustOverrideName, mustOverrideInputSlots, mustOverrideOutputSlots;
		ImU32 overrideTitleTextColor, overrideTitleBgColor;  // 0 -> don't override
		float overrideTitleBgColorGradient;                 //-1 -> don't override
		bool isInEditingMode;

		Node() : Pos(0, 0), Size(0, 0), isSelected(false), baseWidthOverride(-1), mustOverrideName(false), mustOverrideInputSlots(false), mustOverrideOutputSlots(false), overrideTitleTextColor(0), overrideTitleBgColor(0), overrideTitleBgColorGradient(-1.f), isInEditingMode(false) {}
		void init(const char* name, const ImVec2& pos, const char* inputSlotNamesSeparatedBySemicolons = NULL, const char* outputSlotNamesSeparatedBySemicolons = NULL, int _nodeTypeID = 0/*,float currentWindowFontScale=-1.f*/);

		inline virtual ImVec2 GetInputSlotPos(int slot_no, float currentFontWindowScale = 1.f) const { return ImVec2(Pos.x*currentFontWindowScale, Pos.y*currentFontWindowScale + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
		inline virtual ImVec2 GetOutputSlotPos(int slot_no, float currentFontWindowScale = 1.f) const { return ImVec2(Pos.x*currentFontWindowScale + Size.x, Pos.y*currentFontWindowScale + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
		inline virtual const ImVec2 GetPos(float currentFontWindowScale = 1.f) const { return ImVec2(Pos.x*currentFontWindowScale, Pos.y*currentFontWindowScale); }

		friend struct NodeLink;
		friend struct NodeGraphEditor;

		// Helper static methods to simplify code of the derived classes
		// casts:
		template <typename T> inline static T* Cast(Node* n, int TYPE) { return ((n && n->getType() == TYPE) ? static_cast<T*>(n) : NULL); }
		template <typename T> inline static const T* Cast(const Node* n, int TYPE) { return ((n && n->getType() == TYPE) ? static_cast<const T*>(n) : NULL); }


	};

	struct NodeLink
	{
		Node*  InputNode;   int InputSlot;
		Node*  OutputNode;  int OutputSlot;
		ImU32  link_color;
		void* user_data;

		NodeLink(Node* input_node, int input_slot, Node* output_node, int output_slot)
		{
			InputNode = input_node; InputSlot = input_slot;
			OutputNode = output_node; OutputSlot = output_slot;
			user_data = nullptr;
			link_color = ImColor(0, 0, 0, 0);
		}
		friend struct NodeGraphEditor;
	};

	struct NodeGraphEditor
	{
	public:
		typedef Node* (*NodeFactoryDelegate)(int nodeType, const ImVec2& pos, NodeGraphEditor* editor);
		enum NodeState { NS_ADDED, NS_DELETED, NS_EDITED };
		enum LinkState { LS_ADDED, LS_DELETED };

	protected:
		ImVector<Node*> nodes;          // used as a garbage collector too
		ImVector<NodeLink> links;
		ImVec2 scrolling;
		Node *activeNode;               // It's one of the selected nodes (ATM always the first, but the concept of 'active node' is never used by this code: i.e. we could have not included any 'active node' selection at all)
		Node *sourceCopyNode;           // this is owned by the NodeGraphEditor
		Node *menuNode;                 // It's one of the 2 hovered nodes (hovered _in_list or hovered_in_scene), so that the context-menu can retrieve it.
		bool inited;
		bool allowOnlyOneLinkPerInputSlot;  // multiple links can still be connected to single output slots
		bool avoidCircularLinkLoopsInOut;   // however multiple paths from a node to another are still allowed (only in-out circuits are prevented)
		//bool isAContextMenuOpen;            // to fix a bug
		float oldFontWindowScale;           // to fix zooming (CTRL+mouseWheel)    
		float maxConnectorNameWidth;        //used to enlarge node culling space to include connector names
		int nodeListFilterComboIndex;

		ImRect mouseRectangularSelectionForNodes;
		bool mouseRectangularSelectionForNodesStarted = false;

		// Node types here are supposed to be zero-based and contiguous
		const char** pNodeTypeNames; // NOT OWNED! -> Must point to a static reference. Must contain ALL node names.
		int numNodeTypeNames;
		NodeFactoryDelegate nodeFactoryFunctionPtr;

		struct AvailableNodeInfo
		{
			int type, maxNumInstances, curNumInstances;
			const char* name;   // from static persitent user storage
			AvailableNodeInfo(int _type = 0, int _maxNumInstances = -1, int _curNumInstances = 0, const char* _name = NULL) : type(_type), maxNumInstances(_maxNumInstances), curNumInstances(_curNumInstances), name(_name) {}
		};
		ImVector<AvailableNodeInfo> availableNodesInfo;     // These will appear in the "add node menu"
		ImVector<int> availableNodesInfoInverseMap;         // map: absolute node type -> availableNodesInfo index. Must be size() = totalNumberOfNodeTypes.

		typedef void(*NodeCallback)(Node* node, NodeState state, NodeGraphEditor& editor);
		typedef bool(*LinkCallback)(NodeLink* link, LinkState state, NodeGraphEditor& editor);

		typedef void(*ContextMenuCallback)(Node* node, NodeGraphEditor& editor);

		LinkCallback linkCallback;// called after a link is added and before it's deleted
		NodeCallback nodeCallback;// called after a node is added, after it's edited and before it's deleted
		ContextMenuCallback contextMenuCallback;// called after a node is added, after it's edited and before it's deleted
		float nodeEditedTimeThreshold; // time in seconds that must elapse after the last "editing touch" before the NS_EDITED callback is called

	public:
		struct Style
		{
			ImVec4 color_background;
			ImU32 color_grid;
			float grid_line_width, grid_size;
			ImU32 color_node;
			ImU32 color_node_frame;
			ImU32 color_node_selected;
			ImU32 color_node_active;
			ImU32 color_node_frame_selected;
			ImU32 color_node_frame_active;
			ImU32 color_node_hovered;
			ImU32 color_node_frame_hovered;
			float node_rounding;
			ImVec2 node_window_padding;
			ImU32 color_node_input_slots;
			ImU32 color_node_input_slots_border;
			ImU32 color_node_output_slots;
			ImU32 color_node_output_slots_border;
			float node_slots_radius;
			int node_slots_num_segments;
			ImU32 color_link;
			float link_line_width;
			float link_control_point_distance;
			int link_num_segments;  // in AddBezierCurve(...)
			ImVec4 color_node_title;
			ImU32 color_node_title_background;
			float color_node_title_background_gradient;
			ImVec4 color_node_input_slots_names;
			ImVec4 color_node_output_slots_names;
			ImU32 color_mouse_rectangular_selection;
			ImU32 color_mouse_rectangular_selection_frame;
			Style()
			{
				color_background = ImColor(60, 60, 70, 200);
				color_grid = ImColor(200, 200, 200, 40);
				grid_line_width = 1.f;
				grid_size = 64.f;

				color_node = ImColor(60, 60, 60);
				color_node_frame = ImColor(100, 100, 100);
				color_node_selected = ImColor(75, 75, 85);
				color_node_active = ImColor(85, 85, 65);
				color_node_frame_selected = ImColor(115, 115, 115);
				color_node_frame_active = ImColor(125, 125, 105);
				color_node_hovered = ImColor(85, 85, 85);
				color_node_frame_hovered = ImColor(125, 125, 125);
				node_rounding = 4.f;
				node_window_padding = ImVec2(8.f, 8.f);

				color_node_input_slots = ImColor(150, 150, 150, 150);
				color_node_output_slots = ImColor(150, 150, 150, 150);
				node_slots_radius = 5.f;

				color_link = ImColor(200, 200, 100);
				link_line_width = 3.f;
				link_control_point_distance = 50.f;
				link_num_segments = 0;

				color_node_title = ImGui::GetStyle().Colors[ImGuiCol_Text];
				color_node_title_background = 0;//ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]);
				color_node_title_background_gradient = 0.f;   // in [0,0.5f] used only if available (performance is better when 0)
				color_node_input_slots_names = ImGui::GetStyle().Colors[ImGuiCol_Text]; color_node_input_slots_names.w = 0.75f;
				color_node_output_slots_names = ImGui::GetStyle().Colors[ImGuiCol_Text]; color_node_output_slots_names.w = 0.75f;

				color_mouse_rectangular_selection = ImColor(255, 0, 0, 45);
				color_mouse_rectangular_selection_frame = ImColor(45, 0, 0, 175);

				color_node_input_slots_border = color_node_output_slots_border = ImColor(60, 60, 60, 0);
				node_slots_num_segments = 12;
			}

			static bool Edit(Style& style);
			static void Reset(Style& style) { style = Style(); }


		};
		bool show_grid;
		bool show_connection_names;
		bool show_left_pane;
		bool show_style_editor;         // in the left_pane
		bool show_load_save_buttons;    // in the left_pane
		bool show_top_pane;
		bool show_node_copy_paste_buttons;
		static bool UseSlidersInsteadOfDragControls;
		mutable void* user_ptr1;
		mutable void* user_ptr2;
		static Style& GetStyle() { return style; }
		mutable ImGuiColorEditMode colorEditMode;
		float nodesBaseWidth;

		NodeGraphEditor(bool show_grid_ = true, bool show_connection_names_ = true, bool _allowOnlyOneLinkPerInputSlot = true, bool _avoidCircularLinkLoopsInOut = true, bool init_in_ctr = false)
		{
			scrolling = ImVec2(0.0f, 0.0f);
			show_grid = show_grid_;
			show_connection_names = show_connection_names_;
			activeNode = dragNode.node = sourceCopyNode = NULL;
			allowOnlyOneLinkPerInputSlot = _allowOnlyOneLinkPerInputSlot;
			avoidCircularLinkLoopsInOut = _avoidCircularLinkLoopsInOut;
			nodeCallback = NULL; linkCallback = NULL; contextMenuCallback = NULL; nodeEditedTimeThreshold = 1.5f;
			user_ptr1 = NULL;
			user_ptr2 = NULL;
			show_left_pane = true;
			show_top_pane = true;
			show_style_editor = false;
			show_load_save_buttons = false;
			show_node_copy_paste_buttons = true;
			pNodeTypeNames = NULL;
			numNodeTypeNames = 0;
			nodeFactoryFunctionPtr = NULL;
			inited = init_in_ctr;
			colorEditMode = ImGuiColorEditMode_RGB;
			//isAContextMenuOpen = false;
			oldFontWindowScale = 0.f;
			nodesBaseWidth = 120.f;
			maxConnectorNameWidth = 0;
			nodeListFilterComboIndex = 0;
		}
		virtual ~NodeGraphEditor()
		{
			clear();
		}
		void clear()
		{
			if (linkCallback)
			{
				for (int i = links.size() - 1; i >= 0; i--)
				{
					NodeLink& link = links[i];
					linkCallback(&link, LS_DELETED, *this);
				}
			}
			links.clear();
			for (int i = nodes.size() - 1; i >= 0; i--)
			{
				Node*& node = nodes[i];
				if (node)
				{
					if (nodeCallback) nodeCallback(node, NS_DELETED, *this);
					node->~Node();              // ImVector does not call it
					ImGui::MemFree(node);       // items MUST be allocated by the user using ImGui::MemAlloc(...)
					node = NULL;
				}
			}
			nodes.clear();
			scrolling = ImVec2(0, 0);
			if (sourceCopyNode)
			{
				sourceCopyNode->~Node();              // ImVector does not call it
				ImGui::MemFree(sourceCopyNode);       // items MUST be allocated by the user using ImGui::MemAlloc(...)
				sourceCopyNode = NULL;
			}
			activeNode = dragNode.node = NULL;
			oldFontWindowScale = 0.f;
			for (int i = 0, isz = availableNodesInfo.size(); i < isz; i++) { availableNodesInfo[i].curNumInstances = 0; }
			nodeListFilterComboIndex = 0;
		}

		bool isInited() const { return !inited; }

		bool isEmpty() const { return nodes.size() == 0; }

		// nodeTypeNames must point to a block of static memory: it's not owned, nor copied. pOptionalNodeTypesToUse is copied.
		void registerNodeTypes(const char* nodeTypeNames[], int numNodeTypeNames, NodeFactoryDelegate _nodeFactoryFunctionPtr, const int* pOptionalNodeTypesToUse = NULL, int numNodeTypesToUse = -1, const int* pOptionalMaxNumAllowedInstancesToUse = NULL, int numMaxNumAllowedInstancesToUse = 0, bool sortEntriesAlphabetically = true);
		inline int getNumAvailableNodeTypes() const { return availableNodesInfo.size(); }
		bool registerNodeTypeMaxAllowedInstances(int nodeType, int maxAllowedNodeTypeInstances = -1)
		{
			AvailableNodeInfo* ni = fetchAvailableNodeInfo(nodeType);
			if (ni) ni->maxNumInstances = maxAllowedNodeTypeInstances;
			return (ni);
		}

		Node* addNode(int nodeType, const ImVec2& Pos = ImVec2(0, 0)) { return addNode(nodeType, Pos, NULL); }
		bool deleteNode(Node* node)
		{
			if (node == activeNode)  activeNode = NULL;
			if (node == dragNode.node) dragNode.node = NULL;
			if (node == menuNode)  menuNode = NULL;
			for (int i = 0; i < nodes.size(); i++)
			{
				Node*& n = nodes[i];
				if (n == node)
				{
					AvailableNodeInfo* ni = fetchAvailableNodeInfo(node->getType());
					if (ni) --(ni->curNumInstances);
					removeAnyLinkFromNode(n);
					if (nodeCallback) nodeCallback(n, NS_DELETED, *this);
					n->~Node();              // ImVector does not call it
					ImGui::MemFree(n);       // items MUST be allocated by the user using ImGui::MemAlloc(...)
					if (i + 1 < nodes.size()) n = nodes[nodes.size() - 1];    // swap with the last node
					nodes.resize(nodes.size() - 1);
					if (!activeNode) findANewActiveNode();
					return true;
				}
			}
			if (!activeNode) findANewActiveNode();
			return false;
		}
		int getNumNodeInstances(int nodeType, int* pMaxNumAllowedInstancesForThisNodeType = NULL) const
		{
			const AvailableNodeInfo* ni = fetchAvailableNodeInfo(nodeType);
			if (pMaxNumAllowedInstancesForThisNodeType) *pMaxNumAllowedInstancesForThisNodeType = ni ? ni->maxNumInstances : -1;
			return ni->curNumInstances;
		}
		bool addLink(Node* inputNode, int input_slot, Node* outputNode, int output_slot, bool checkIfAlreadyPresent = false)
		{
			if (!inputNode || !outputNode) return false;
			bool insert = true;
			if (checkIfAlreadyPresent) insert = !isLinkPresent(inputNode, input_slot, outputNode, output_slot);
			if (insert)
			{
				bool allow = true;
				NodeLink link = NodeLink(inputNode, input_slot, outputNode, output_slot);
				if (linkCallback) allow = linkCallback(&link, LS_ADDED, *this);
				if (allow)
					links.push_back(link);
			}
			return insert;
		}
		bool removeLink(Node* inputNode, int input_slot, Node* outputNode, int output_slot)
		{
			int link_idx = -1;
			bool ok = isLinkPresent(inputNode, input_slot, outputNode, output_slot, &link_idx);
			if (ok) ok = removeLinkAt(link_idx);
			return ok;
		}
		void removeAnyLinkFromNode(Node* node, bool removeInputLinks = true, bool removeOutputLinks = true);
		// slots can be set to -1 to get the first link between the two nodes. nodes can be set to null to check for any node
		bool isLinkPresent(Node* inputNode, int input_slot, Node* outputNode, int output_slot, int* pOptionalIndexInLinkArrayOut = NULL) const;

		typedef int LinkIterator;
		NodeLink* iterate_links(Node* node, int slot, LinkIterator* it);

		// To be called INSIDE a window
		void render();

		virtual void render_custom_header() {}


		// Optional helper methods:
		Node* getHoveredNode() { return menuNode; }  // This is actually not strictly the hovered node, but the node called 'menuNode'
		const Node* getHoveredNode() const { return menuNode; }
		int getSelectedNodes(ImVector<Node*>& rv);  // returns rv.size(). The active node should be contained inside rv (the first AFAIK).
		int getSelectedNodes(ImVector<const Node*>& rv) const;
		Node* getActiveNode() { return activeNode; }  // The 'active' node is the first of the selected nodes
		const Node* getActiveNode() const { return activeNode; }
		const char* getActiveNodeInfo() const { return activeNode->getInfo(); }
		void setActiveNode(const Node* node) { if (node) { node->isSelected = true; activeNode = const_cast<Node*>(node); } }
		void selectNode(const Node* node, bool findANewActiveNodeWhenNeeded = true) { selectNodePrivate(node, true, findANewActiveNodeWhenNeeded); }
		void unselectNode(const Node* node, bool findANewActiveNodeWhenNeeded = true) { selectNodePrivate(node, false, findANewActiveNodeWhenNeeded); }
		void selectAllNodes(bool findANewActiveNodeWhenNeeded = true) { selectAllNodesPrivate(true, findANewActiveNodeWhenNeeded); }
		void unselectAllNodes() { selectAllNodesPrivate(false); }
		bool isNodeSelected(const Node* node) const { return (node && node->isSelected); }

		void getOutputNodesForNodeAndSlot(const Node* node, int output_slot, ImVector<Node*>& returnValueOut, ImVector<int>* pOptionalReturnValueInputSlotOut = NULL) const;
		void getInputNodesForNodeAndSlot(const Node* node, int input_slot, ImVector<Node*>& returnValueOut, ImVector<int>* pOptionalReturnValueOutputSlotOut = NULL) const;
		// if allowOnlyOneLinkPerInputSlot == true:
		Node* getInputNodeForNodeAndSlot(const Node* node, int input_slot, int* pOptionalReturnValueOutputSlotOut = NULL) const;
		bool isNodeReachableFrom(const Node *node1, int slot1, bool goBackward, const Node* nodeToFind, int* pOptionalNodeToFindSlotOut = NULL) const;
		bool isNodeReachableFrom(const Node *node1, bool goBackward, const Node* nodeToFind, int* pOptionalNode1SlotOut = NULL, int* pOptionalNodeToFindSlotOut = NULL) const;
		bool hasLinks(Node* node) const;
		int getAllNodesOfType(int typeID, ImVector<Node*>* pNodesOut = NULL, bool clearNodesOutBeforeUsage = true);
		int getAllNodesOfType(int typeID, ImVector<const Node*>* pNodesOut = NULL, bool clearNodesOutBeforeUsage = true) const;

		// It should be better not to add/delete node/links in the callbacks... (but all is untested here)
		void setNodeCallback(NodeCallback cb) { nodeCallback = cb; }
		void setLinkCallback(LinkCallback cb) { linkCallback = cb; }
		void setContextMenuCallback(ContextMenuCallback cb) { contextMenuCallback = cb; }
		void setNodeEditedCallbackTimeThreshold(int seconds) { nodeEditedTimeThreshold = seconds; }



			// I suggest we don not use these 3; however we can:
		bool overrideNodeName(Node* node, const char* newName);
		void overrideNodeTitleBarColors(Node* node, const ImU32* pTextColor, const ImU32* pBgColor, const float* pBgColorGradient);    // default values can be reset using 0 for colors and -1 for gradient
		bool overrideNodeInputSlots(Node* node, const char* slotNamesSeparatedBySemicolons);
		bool overrideNodeOutputSlots(Node* node, const char* slotNamesSeparatedBySemicolons);

		// This are the chars used as buttons in the nodes' titlebars. Users might want to change them.
		static char CloseCopyPasteChars[3][5];  // By default = {"x","^","v"};

	protected:

		struct DragNode
		{
			ImVec2 pos;
			Node* node; int inputSlotIdx, outputSlotIdx;
			DragNode() : node(NULL), inputSlotIdx(-1), outputSlotIdx(-1) {}
			bool isValid() const { return node && (inputSlotIdx >= 0 || outputSlotIdx >= 0); }
			void reset() { *this = DragNode(); }
		};
		DragNode dragNode;

		inline AvailableNodeInfo* fetchAvailableNodeInfo(int nodeType)
		{
			const int tmp = availableNodesInfoInverseMap[nodeType]; return tmp >= 0 ? &availableNodesInfo[tmp] : NULL;
		}
		inline const AvailableNodeInfo* fetchAvailableNodeInfo(int nodeType) const
		{
			const int tmp = availableNodesInfoInverseMap[nodeType]; return tmp >= 0 ? &availableNodesInfo[tmp] : NULL;
		}

		Node* addNode(int nodeType, const ImVec2& Pos, AvailableNodeInfo* pOptionalNi)
		{
			if (!nodeFactoryFunctionPtr) return NULL;
			if (!pOptionalNi) pOptionalNi = fetchAvailableNodeInfo(nodeType);
			if (!pOptionalNi || (pOptionalNi->maxNumInstances >= 0 && pOptionalNi->curNumInstances >= pOptionalNi->maxNumInstances)) return NULL;
			Node* rv = nodeFactoryFunctionPtr(pOptionalNi->type, Pos, this);
			if (rv) ++(pOptionalNi->curNumInstances);
			return addNode(rv);
		}
		// BEST PRACTICE: always call this method like: Node* node = addNode(ExampleNode::Create(...));
		Node* addNode(Node* justCreatedNode)
		{
			if (justCreatedNode)
			{
				nodes.push_back(justCreatedNode);
				if (nodeCallback) nodeCallback(nodes[nodes.size() - 1], NS_ADDED, *this);
			}
			return justCreatedNode;
		}
		void copyNode(Node* n);
		bool removeLinkAt(int link_idx);
		// Warning: node index changes when a node becomes active!
		inline int getNodeIndex(const Node* node)
		{
			for (int i = 0; i < nodes.size(); i++)
			{
				const Node* n = nodes[i];
				if (n == node) return i;
			}
			return -1;
		}
		inline int findANewActiveNode()
		{
			activeNode = NULL;
			for (int i = 0, isz = nodes.size(); i < isz; i++)
			{
				Node* n = nodes[i];
				if (n->isSelected) { activeNode = n; return i; }
			}
			return -1;
		}
		static Style style;

	private:
		// Refactored for cleaner exposure (without the misleading 'flag' argument)
		void selectNodePrivate(const Node* node, bool flag = true, bool findANewActiveNodeWhenNeeded = true);
		void selectAllNodesPrivate(bool flag = true, bool findANewActiveNodeWhenNeeded = true);
		static int AvailableNodeInfoNameSorter(const void *s0, const void *s1)
		{
			const AvailableNodeInfo& ni0 = *((AvailableNodeInfo*)s0);
			const AvailableNodeInfo& ni1 = *((AvailableNodeInfo*)s1);
			return strcmp(ni0.name, ni1.name);
		}

	};

}	// namespace ImGui

#endif //IMGUINODEGRAPHEDITOR_H_

