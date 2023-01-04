#pragma once

#include "Core.h"
#include "CustomUI/SequenceMap/UIMap.h"
#include "CustomUI/SequenceMap/UIPointInteret.h"
#include "CustomUI/SequenceMap/PointInteretVignette.h"

#include "UserPositionGPS/UserPositionGPS.h"

//#include "RecordFilePlayer.h"

namespace Kigs
{
	namespace Gps
	{
		using namespace Kigs::Core;
		inline void InitGPSCourseClasses(KigsCore* core)
		{
			DECLARE_FULL_CLASS_INFO(core, UIMap, UIMap, 2DLayers);
			DECLARE_FULL_CLASS_INFO(core, UserPositionGPS, UserPositionGPS, 2DLayers);
			DECLARE_FULL_CLASS_INFO(core, UIPointInteret, UIPointInteret, 2DLayers);
			DECLARE_FULL_CLASS_INFO(core, PointInteretVignette, PointInteretVignette, 2DLayers);

			//DECLARE_FULL_CLASS_INFO(core, RecordFilePlayer, RecordFilePlayer, ArenesBezier);
		}
	}
}