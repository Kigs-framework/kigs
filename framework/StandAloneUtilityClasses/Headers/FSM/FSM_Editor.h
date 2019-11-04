#pragma once

#include "kstlmap.h"

class FSM;

namespace internals_node_editor
{
	struct NodeFSM;
	struct NodeBase;
}
namespace ImGui
{
	struct NodeGraphEditor;
}

struct NodeEditor
{
	FSM* associated_fsm;
	
	void render();
	void init_empty();
	void init_with_FSM_instance(FSM* fsm);

	void init_rec_with_fsm(FSM* fsm, internals_node_editor::NodeFSM* node_fsm, const kstl::map<unsigned int, internals_node_editor::NodeBase*>& parent_nodes);

	ImGui::NodeGraphEditor* current_editor = nullptr;
	internals_node_editor::NodeFSM* root_fsm = nullptr;
	internals_node_editor::NodeBase* node_to_edit = nullptr;
	internals_node_editor::NodeBase* node_in_edit = nullptr;

	bool in_lua_edit = false;
};