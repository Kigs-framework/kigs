#pragma once
#ifdef TODO

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "RecordFile.h"

namespace Kigs
{
	namespace Input
	{
		class ModuleInput;
	}
	namespace Gps
	{
		using namespace Kigs::Core;

		class FrameBufferStream;
		class MPEG4BufferStream;

		class RecordFilePlayer : public CoreModifiable
		{
		public:
			DECLARE_CLASS_INFO(RecordFilePlayer, CoreModifiable, StandAloneClass);
			DECLARE_CONSTRUCTOR(RecordFilePlayer);

			virtual void Update(const Time::Timer& timer, void*) override;

			void Start(const Time::Timer& timer);

			SP<FrameBufferStream>& GetStream();
			~RecordFilePlayer();

		protected:
			double _start_time;
			maBool _updateGyro;
			maBool _updateAccel;
			maBool _updateGPS;
			maBool _start_on_first_update;
			maString _filename;

			Input::ModuleInput* _input;
			SP<MPEG4BufferStream> _stream;
			RecordFileReader _reader;

			RecordFieldBase* _field_time;
			RecordFieldBase* _field_gyro;
			RecordFieldBase* _field_accel;
			RecordFieldBase* _field_lat;
			RecordFieldBase* _field_long;
			bool _playing = false;
			int _current_frame = -1;

			virtual void InitModifiable() override;
		};
	}
}
#endif// TODO