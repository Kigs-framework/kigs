#pragma once

namespace Kigs
{
	namespace Core
	{
		class CoreModifiable;
	}
	namespace Gui
	{
		bool CameraCombo(const char* label, int* current_item, Core::CoreModifiable** camera_result = nullptr);
	};
}
