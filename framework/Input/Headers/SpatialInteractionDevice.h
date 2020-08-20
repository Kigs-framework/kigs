#pragma once

#include "InputDevice.h"

#include "AttributePacking.h"

#include <optional>

struct Hit;


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
	double Time;
	v3f Position;
	
	v3f Forward;
	v3f Up;

	unsigned int ID;
	SourceState state;
	bool allowed = true;
	bool removed = false;
	bool pressed = false;
	bool hasPosition = false;

	int near_interaction_active_count = 0;
	float near_interaction_distance = FLT_MAX;
	Handedness handedness = Handedness::Unspecified;

	struct Joint
	{
		v3f position;
		// Transform Z-unit vector for tip direction (joint to joint), Y-unit for up direction (from joint to back of the hand), X-unit vector for side direction
		quat orientation;
	};
	
	std::optional<Joint> palm;
	std::optional<Joint> index_tip;
	std::optional<Joint> middle_tip;
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
	DECLARE_INLINE_CONSTRUCTOR(SpatialInteractionDevice){}

	virtual bool GetInteractionPosition(u32 ID, v3f& pos) const = 0;
	virtual bool GetInteractionState(u32 ID, SourceState & state) const = 0;
	virtual const Interaction* GetInteraction(u32 ID) const = 0;
	
	kigs::unordered_map<int, Interaction>& GetInteractions() { return mInteractions; }
	const kigs::unordered_map<int, v2f>& GetAllThumbstick() const { return mThumbstickList; }

protected:
	kigs::unordered_map<int, Interaction> mInteractions;
	kigs::unordered_map<int, v2f> mThumbstickList;

	maReference mGazeCamera = BASE_ATTRIBUTE(GazeCamera, "Camera:camera");
};
