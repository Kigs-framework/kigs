#ifndef _MODULEIMGUI_H_
#define _MODULEIMGUI_H_

#include "ModuleBase.h"
#include "imgui.h"

#include "AttributePacking.h"


class ImGuiLayer;

class ModuleImGui : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(ModuleImGui, ModuleBase, ImGui)
	DECLARE_INLINE_CONSTRUCTOR(ModuleImGui){}

	//! init module
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	//! close module
	void Close() override;
            
	//! update module
	void Update(const Timer& timer, void* addParam) override;

	// Quickly create a layer for debug purposes (uses the IMEditor's one if available)
	ImGuiLayer* CreateDebugLayer(kfloat bg_opacity = -1.0f);


	WRAP_METHODS(SetupDebugLayer);
protected:
	
	void SetupDebugLayer()
	{
		CreateDebugLayer();
	}
}; 


#endif



