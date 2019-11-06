#include "PrecompiledHeaders.h"
#include "ImGuiConfig.h"
#include <ModuleImGui.h>

#include <Core.h>
#include <Timer.h>

#include <ImGuiLayer.h>


IMPLEMENT_CLASS_INFO(ModuleImGui)


ImGuiLayer* ModuleImGui::CreateDebugLayer(kfloat bg_opacity)
{
	ImGuiLayer* DEBUG_ImGuiDebugLayer;
	kstl::set<CoreModifiable*> insts;
	CoreModifiable::GetInstancesByName("ImGuiLayer", "imgui_debug_layer", insts);
	if (insts.size())
		DEBUG_ImGuiDebugLayer = (ImGuiLayer*)*insts.begin();
	else
	{
		DEBUG_ImGuiDebugLayer = (ImGuiLayer*)KigsCore::GetInstanceOf("imgui_debug_layer", "ImGuiLayer");
		DEBUG_ImGuiDebugLayer->setValue("RenderingScreen", "RenderingScreen:theRenderingScreen");
		DEBUG_ImGuiDebugLayer->setValue("Priority", INT_MIN);
		DEBUG_ImGuiDebugLayer->setValue("ClearColorBuffer", false);
		DEBUG_ImGuiDebugLayer->setValue("UseOldWUPKeyboard", true);
		DEBUG_ImGuiDebugLayer->Init();
		KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex)->addItem(DEBUG_ImGuiDebugLayer);
		DEBUG_ImGuiDebugLayer->Destroy();
	}
	DEBUG_ImGuiDebugLayer->SetActiveImGuiLayer();
	if(bg_opacity>=0.0f)
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = bg_opacity;

//	ImGui::GetIO().FontGlobalScale = 3.5f;
	return DEBUG_ImGuiDebugLayer;
}

void LoadImguiBindings(lua_State*);

void ModuleImGui::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ImGui",params);
	DECLARE_FULL_CLASS_INFO(core, ImGuiLayer, ImGuiLayer, ImGui);
}


void ModuleImGui::Close()
{
    BaseClose();
}    

void ModuleImGui::Update(const Timer& timer, void* addParam)
{
	// Base update calls specific renderer
	BaseUpdate(timer,addParam);
}    
