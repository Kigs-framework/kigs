#pragma once

#include "InputDevice.h"
#include "AttributePacking.h"
#include "TecLibs/Math/IntersectionAlgorithms.h"

#include <optional>
#include <unordered_set>

namespace Kigs
{
	namespace Input
	{
		using namespace Kigs::Core;

		enum class SourceState : u32
		{
			Pressed = 0,
			Released,
			Lost,
			Updated,
			Detected
		};

		enum class Handedness
		{
			Unspecified,
			Left,
			Right
		};

		struct Interaction
		{
			double StartTime = 0.0;;
			double LastTime = 0.0;;
			double TimeStill = 0.0;
			double DT = 0.0;

			p3f Position;

			v3f Forward;
			v3f Up;

			unsigned int ID;
			SourceState state;
			bool allowed = true;
			bool removed = false;
			bool pressed = false;
			bool hasPosition = false;

			float near_interaction_distance = FLT_MAX;
			Handedness handedness = Handedness::Unspecified;

			struct Joint
			{
				p3f position;
				// Transform Z-unit vector for tip direction (joint to joint), Y-unit for up direction (from joint to back of the hand), X-unit vector for side direction
				quat orientation;
			};

			std::optional<Joint> palm;
			std::optional<Joint> index_tip;
			std::optional<Joint> middle_tip;
			Maths::Hit current_near_interaction_hit;

			p3f SmoothPosition{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			v3f SmoothDirection{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
		};

		// ****************************************
		// * SpatialInteractionDevice class
		// * --------------------------------------
		/**
		 * \class	SpatialInteractionDevice
		 * \file	SpatialInteractionDevice.h
		 * \ingroup Input
		 * \brief	Base class to manage spacial interaction ( Hololens ).
		 */
		 // ****************************************

		class SpatialInteractionDevice : public InputDevice
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(SpatialInteractionDevice, InputDevice, Input)
				DECLARE_INLINE_CONSTRUCTOR(SpatialInteractionDevice) {}

			std::optional<v3f> GetInteractionPosition(u32 ID) const;
			std::optional<SourceState> GetInteractionState(u32 ID) const;
			std::shared_ptr<Interaction> GetInteraction(u32 ID) const;

			unordered_map<int, std::shared_ptr<Interaction>>& GetInteractions() { return mInteractions; }
			const unordered_map<int, v2f>& GetAllThumbstick() const { return mThumbstickList; }

		protected:
			unordered_map<int, std::shared_ptr<Interaction>> mInteractions;
			unordered_map<int, v2f> mThumbstickList;

			maReference mGazeCamera = BASE_ATTRIBUTE(GazeCamera, "Camera:camera");
		};
	}
}