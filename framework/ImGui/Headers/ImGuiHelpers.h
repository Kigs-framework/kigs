#pragma once



#include "Camera.h"

namespace ImGui
{
	bool CameraCombo(const char* label, int* current_item, CoreModifiable** camera_result = nullptr);
};

