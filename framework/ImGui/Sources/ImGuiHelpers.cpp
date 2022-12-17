#include "ImGuiHelpers.h"

#include "ModuleImGui.h"


#include "Camera.h"

namespace ImGui
{
	namespace
	{
		bool getter(void *data, int index, const char** item_text) 
		{
			auto it = (*(std::set<CoreModifiable*>*)data).begin();
			std::advance(it, index);
			*item_text = (*it)->getName().c_str();
			return true;
		}
	};
	
	bool CameraCombo(const char* label, int* current_item, CoreModifiable** camera_result)
	{
		std::vector<CMSP> insts=CoreModifiable::GetInstances("Camera");
		
		if(ImGui::Combo(label, current_item, getter, (void*)&insts, (int)insts.size(), 10))
		{
			int i = 0;
			for(auto& cm : insts)
			{
				auto cam = (Camera*)cm.get();
				if(*current_item == -1)
				{
					bool b = false;
					cam->getValue("CameraIsEnabled", b);
					if(b)
					{
						*current_item = i;
						if(camera_result)
							*camera_result = cam;
						return false;
					}
				}
				else
				{
					cam->setValue("CameraIsEnabled", i==*current_item);
				}
				if(i==*current_item && camera_result)
					*camera_result = cam;
				++i;
			}
			
			return true;
		}
		return false;
	}
};