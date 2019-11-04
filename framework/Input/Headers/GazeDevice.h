#ifndef _GazeDevice_H_
#define _GazeDevice_H_

#include "InputDevice.h"

#include "AttributePacking.h"

struct Hit;


enum class SourceState : u32
{
	Pressed = 0,
	Released,
	Lost,
	Updated,
	Detected
};

struct GazeTouch
{
	double Time;
	v3f Position;
	
	v3f Forward;
	v3f Up;

	unsigned int ID;
	SourceState state;
	bool removed = false;
	bool pressed = false;
	bool hasPosition = false;
};

class GazeDevice : public InputDevice
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(GazeDevice, InputDevice, Input)
	DECLARE_INLINE_CONSTRUCTOR(GazeDevice){}

	virtual bool GetTouchPosition(u32 ID, v3f& pos) const = 0;
	virtual bool GetTouchState(u32 ID, SourceState & state) const = 0;
	virtual const GazeTouch* GetTouchEvent(u32 ID) const = 0;
	
	const std::unordered_map<int, GazeTouch>& GetAllTouches() const { return myTouchList; }
	const std::unordered_map<int, v2f>& GetAllThumbstick() const { return myThumbstickList; }

protected:
	std::unordered_map<int, GazeTouch> myTouchList;
	std::unordered_map<int, v2f> myThumbstickList;

	maReference mGazeCamera = BASE_ATTRIBUTE(GazeCamera, "Camera:camera");
};

#endif //_GazeDevice_H_