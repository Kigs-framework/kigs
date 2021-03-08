#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "RecordFile.h"

class ModuleInput;
class FrameBufferStream;
class MPEG4BufferStream;

class RecordFilePlayer : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(RecordFilePlayer, CoreModifiable, StandAloneClass);
	DECLARE_CONSTRUCTOR(RecordFilePlayer);

	virtual void Update(const Timer& timer, void*) override;

	void Start(const Timer& timer);

	SP<FrameBufferStream>& GetStream();
	~RecordFilePlayer();

protected:
	kdouble _start_time;
	maBool _updateGyro;
	maBool _updateAccel;
	maBool _updateGPS;
	maBool _start_on_first_update;
	maString _filename;

	ModuleInput* _input;
	SP<MPEG4BufferStream> _stream;
	RecordFileReader _reader;

	RecordFieldBase* _field_time;
	RecordFieldBase* _field_gyro;
	RecordFieldBase* _field_accel;
	RecordFieldBase* _field_lat;
	RecordFieldBase* _field_long;
	bool _playing = false;
	int _current_frame=-1;

	virtual void InitModifiable() override;
};