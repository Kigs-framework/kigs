#include "GPSCourse/Headers/RecordFilePlayer.h"
#include "ModuleInput.h"
#include "GyroscopeDevice.h"
#include "AccelerometerDevice.h"
#include "GeolocationDevice.h"
#include "MPEG4BufferStream.h"
#include "Timer.h"

#ifdef TODO

using namespace Kigs::Gps;

IMPLEMENT_CLASS_INFO(RecordFilePlayer);


IMPLEMENT_CONSTRUCTOR(RecordFilePlayer)
, _updateGyro(*this, false, "UpdateGyro", true)
, _updateAccel(*this, false, "UpdateAccelerometer", true)
, _updateGPS(*this, false, "UpdateGPS", true)
, _start_on_first_update(*this, false, "StartOnFirstUpdate", true)
, _filename(*this, false, "FileName", "")
, _stream(nullptr)
{
	
}

//#pragma optimize("", off)
void RecordFilePlayer::Update(const Timer& timer, void*)
{
	if (!_playing)
	{
		if(_start_on_first_update)
		{
			_start_time = timer.GetTime();
			_playing = true;
			_start_on_first_update = false;
			if(_stream)
			{
				_stream->Stop();
				_stream->Start();
			}
		}
		else return;
	}

	double t = timer.GetTime() - _start_time;
	
	double * curr_time = nullptr;
	int i;
	for (i = 0; i < _reader.GetFrameCount(); ++i)
	{
		_reader.SetFrame(i);
		_reader.GetFromFrame(_field_time, &curr_time);

		if (*curr_time > t)
			break;
	}

	if (i >= _reader.GetFrameCount()) return;
	if (i == _current_frame) return;

	_current_frame = i;
	_reader.SetFrame(_current_frame);

	if (_updateGyro)
	{
		float * gyro_values;
		_reader.GetFromFrame(_field_gyro, &gyro_values);
		_input->GetGyroscope()->setArrayValue("RotationQuaternion", gyro_values, 4);
		//printf("%0.2f %0.2f %0.2f %0.2f\n", gyro_values[0], gyro_values[1], gyro_values[2], gyro_values[3]);
	}

	if (_updateAccel)
	{
		float * accel_values;
		_reader.GetFromFrame(_field_gyro, &accel_values);
		_input->GetAccelerometer()->setValue("AccX", accel_values[0]);
		_input->GetAccelerometer()->setValue("AccY", accel_values[1]);
		_input->GetAccelerometer()->setValue("AccZ", accel_values[2]);
	}

	if (_updateGPS)
	{
		double *lati;
		double *longi;
		_reader.GetFromFrame(_field_lat, &lati);
		_reader.GetFromFrame(_field_long, &longi);
		_input->GetGeolocation()->setValue("Latitude", lati[0]);
		_input->GetGeolocation()->setValue("Longitude", longi[0]);

		//printf("%03.2f  : %0.8f %0.8f\n", (float)_current_frame/ (float)_reader.GetFrameCount()*100.0f,  lati[0], longi[0]);
	}

	if (_stream)
		_stream->Update(timer, 0);
}

void RecordFilePlayer::Start(const Timer& timer)
{
	_start_time = timer.GetTime()-1.0f;
	_playing = true;
	if (_stream)
	{
		CMSP toAdd((CoreModifiable*)&timer, GetRefTag{});
		_stream->addItem(toAdd);
		_stream->Stop();
		_stream->Start();
	}
}

void RecordFilePlayer::InitModifiable()
{
	CoreModifiable::InitModifiable();

	_input = (ModuleInput*)KigsCore::Instance()->GetMainModuleInList(InputModuleCoreIndex);
	_reader.InitWithFile(_filename.const_ref().c_str());
	_reader.GetField(&_field_time, "Time");
	_reader.GetField(&_field_gyro, "Rotation");
	_reader.GetField(&_field_accel, "Acceleration");
	_reader.GetField(&_field_lat, "Latitude");
	_reader.GetField(&_field_long, "Longitude");
	_reader.GetField(&_field_long, "Longitude");
	std::string videofile;
	if (_reader.GetParameter("FileName", videofile) == 0)
	{
		videofile = SplitStringByCharacter(videofile, '/').back();
		_stream = KigsCore::GetInstanceOf("webcamera", "MPEG4BufferStream");
		_stream->setValue("FileName", videofile);
		_stream->Init();
	}

}

SP<FrameBufferStream>& RecordFilePlayer::GetStream()
{
	return (SP<FrameBufferStream> &)_stream;
}

RecordFilePlayer::~RecordFilePlayer()
{
	_stream = nullptr;
}

#endif