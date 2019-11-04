
#ifndef __ABASESTREAM_H__
#define __ABASESTREAM_H__

#include "AMDefines.h"
#include "CoreModifiable.h"

class   AnimationResourceInfo;
class LocalToGlobalBaseType;
class ABaseChannel;

// ASystem is template to ABaseSystem can be use to have pointer without template parameters
class   ABaseStream : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(ABaseStream, CoreModifiable, Animation);
	DECLARE_CONSTRUCTOR(ABaseStream);

	friend class    ABaseChannel;
	friend class    ALinks;
	friend class    AObject;
	template<typename LocalToGlobalType> friend class   AChannel;

	// ******************************
	// * Init
	// *-----------------------------
	/*! init some members with given values
	*/
	// ******************************

	void    InitStream(ABaseChannel* channel, IntU32 priority, Float  weight, Float speed, bool loop)
	{
		m_Priority = priority;
		m_Weight = weight;
		m_Speed = speed;
		m_StartTime = KFLOAT_ZERO;
		m_LocalTime = KFLOAT_ZERO;
		m_pChannel = channel;
		m_Loop = loop;
	};

	AnimationResourceInfo* GetResourceInfo()
	{
		return m_pResourceInfo;
	};

	// ******************************
	// * NewStreamOutputDataInstance
	// *-----------------------------
	/*! return an instance of the stream output data
	*/
	// ******************************

	virtual LocalToGlobalBaseType*   NewStreamOutputDataInstance() = 0;

	// ******************************
	// * DeleteStreamOutputDataInstance
	// *-----------------------------
	/*! destroy an instance of output data
	*/
	// ******************************

	virtual void    DeleteStreamOutputDataInstance(LocalToGlobalBaseType* data) = 0;

	// ******************************
	// * InitData
	// *-----------------------------
	/*! do : data = this->data
	*/
	// ******************************

	virtual   void      InitData(LocalToGlobalBaseType* data) = 0;

	// ******************************
	// * LERPData
	// *-----------------------------
	/*! do : data = (this->data*t)+(data*(1-t))
	*/
	// ******************************

	virtual   void      LERPData(LocalToGlobalBaseType* data, Float t) = 0;

	// ******************************
	// * MulData
	// *-----------------------------
	/*! do : data1 = data1*data2
	*/
	// ******************************

	virtual   void      MulData(LocalToGlobalBaseType* data1, LocalToGlobalBaseType* data2) = 0;

	// ******************************
	// * MulInvData
	// *-----------------------------
	/*! do : data1 = data1*(1/data2)
	*/
	// ******************************

	virtual   void      MulInvData(LocalToGlobalBaseType* data1, LocalToGlobalBaseType* data2) = 0;

	// ******************************
	// * CopyData
	// *-----------------------------
	/*! do : data1 = data2
	*/
	// ******************************

	virtual   void      CopyData(LocalToGlobalBaseType* dst, LocalToGlobalBaseType* src) = 0;

	// ******************************
	// * IdentityData
	// *-----------------------------
	/*! do : data = identity
	*/
	// ******************************

	virtual   void      IdentityData(LocalToGlobalBaseType* data) = 0;


	// ******************************
	// * SameData
	// *-----------------------------
	/*! do : data == other_data
	*/
	// ******************************

	virtual   bool      SameData(LocalToGlobalBaseType* data, LocalToGlobalBaseType* other_data) = 0;


	// ******************************
	// * GetSystemType
	// *-----------------------------
	/*! return the type of system to use with this stream
	*/
	// ******************************

	virtual kstl::string    GetSystemType() = 0;


	// ******************************
	// * InitFromResource
	// *-----------------------------
	/*! init the stream with the given resource
	*/
	// ******************************

	virtual void InitFromResource(AnimationResourceInfo* info, IntU32 streamindex) = 0;

	virtual void        Start() = 0;
	virtual void        Stop() = 0;

	// ******************************
	// * GetNextStream
	// *-----------------------------
	/*! return next stream in the stream list
	*/
	// ******************************

	ABaseStream*        GetNextStream() const
	{
		return m_pNextStream;
	};

	// ******************************
	// * SetAndModifyData
	// *-----------------------------
	/*!  - let data1 defining a source coordinate space
	- let data2 be the destination coordinate space
	- the method set data1 to data2 and affect modifieddata according to that
	*/
	// ******************************

	virtual   void      SetAndModifyData(LocalToGlobalBaseType* data1, LocalToGlobalBaseType* data2, LocalToGlobalBaseType* modifieddata) = 0;

protected:

	virtual ~ABaseStream()
	{
	}

	// ******************************
	// * GetStreamLength
	// *-----------------------------
	/*! return the length of the stream
	*/
	// ******************************

	virtual ATimeValue    GetStreamLength() = 0;

	// ******************************
	// * UpdateData
	// *-----------------------------
	/*! Update Data according to m_LocalTime
	*/
	// ******************************

	virtual void    UpdateData(LocalToGlobalBaseType* standdata) = 0;

	// ******************************
	// * Local Time management
	// *-----------------------------
	/*! Get local time
	*/
	// ******************************

	ATimeValue     GetLocalTime()  const
	{
		return m_LocalTime;
	};

	// ******************************
	// * Local Time management
	// *-----------------------------
	/*! Set local time
	*/
	// ******************************

	void            SetLocalTime(ATimeValue local_time)
	{
		m_LocalTime = local_time;
	};

	// ******************************
	// * Start Time management
	// *-----------------------------
	/* Get start time
	*/
	// ******************************

	ATimeValue     GetStartTime()  const
	{
		return m_StartTime;
	};

	// ******************************
	// * Start Time management
	// *-----------------------------
	/* Set start time
	*/
	// ******************************

	void            SetStartTime(ATimeValue start_time)
	{
		m_StartTime = start_time;
	};

	// ******************************
	// * GetChannel
	// *-----------------------------
	/*! return the channel where the stream is
	*/
	// ******************************

	ABaseChannel*   GetChannel()
	{
		return m_pChannel;
	};



	// ******************************
	// * SetRepeatCount
	// *-----------------------------
	/*! Set the repeat count
	*/
	// ******************************

	void    SetRepeatCount(IntU32 count)
	{
		m_RepeatCount = (IntS32)count;
		if (count>0)
		{
			m_bEndReached = false;
		}
		else
		{
			m_bEndReached = true;
		}
	};

	// ******************************
	// * GetRepeatCount
	// *-----------------------------
	/*! Get the repeat count
	*/
	// ******************************

	IntS32    GetRepeatCount()
	{
		return(m_RepeatCount);
	};

	// ******************************
	// * IsPlaying
	// *-----------------------------
	/*! return true if stream is playing
	*/
	// ******************************

	bool        IsPlaying()
	{
		return m_IsPlaying;
	};

	// ******************************
	// * IsOutsideAnim
	// *-----------------------------
	/*! return true if stream time is outside animation time
	*/
	// ******************************

	bool        IsOutsideAnim()
	{
		return m_OutsideAnimFlag;
	};

	// ******************************
	// * HasLoop
	// *-----------------------------
	/*! return true if stream has loop when the last SetTime occured
	*/
	// ******************************

	bool        HasLoop()
	{
		return m_HasLoop;
	};

	// ******************************
	// * HasReachedEnd
	// *-----------------------------
	/*! return true if stream has reached end when the last SetTime occured
	*/
	// ******************************

	bool        HasReachedEnd()
	{
		return m_bEndReached;
	};

	// ******************************
	// * SetLoop
	// *-----------------------------
	/*! Set loop mode
	*/
	// ******************************

	void        SetLoop(bool loop)
	{
		m_Loop = loop;
	};

	// ******************************
	// * SetTime
	// *-----------------------------
	/*! Update local time and data with the given global time
	manage loop mode
	*/
	// ******************************

	void    SetTime(ATimeValue t);

	// ******************************
	// * SetTimeWithoutLoop
	// *-----------------------------
	/*! Update local time and data with the given global time
	special mode for AChannel
	*/
	// ******************************

	void    SetTimeWithoutLoop(ATimeValue t);

	// ******************************
	// * SetTimeWithStartLoop
	// *-----------------------------
	/*! Update local time and data with the given global time
	special mode for AChannel
	*/
	// ******************************

	void    SetTimeWithStartLoop(ATimeValue t);

	// ******************************
	// * SetNextStream
	// *-----------------------------
	/*! - Stream are set in a channel in a linked list, this function set
	the next list member to the given stream
	- the given stream must not have a next stream ( because only this stream is inserted in the list )
	*/
	// ******************************

	void        SetNextStream(ABaseStream* stream)
	{
		if (m_pNextStream != NULL)
		{
			stream->m_pNextStream = m_pNextStream;
		}

		m_pNextStream = stream;

	};

	// ******************************
	// * DelNextStream
	// *-----------------------------
	/*! - Remove the next stream from the linked list
	- the stream is just removed, not deleted, so the return value should be deleted
	*/
	// ******************************

	ABaseStream*    DelNextStream()
	{
		if (m_pNextStream != NULL)
		{
			ABaseStream*    tmp_stream = m_pNextStream;
			m_pNextStream = m_pNextStream->m_pNextStream;
			return  tmp_stream;
		}

		return NULL;
	};

	// ******************************
	// * SetChannel
	// *-----------------------------
	/*! set the channel associated to this stream
	*/
	// ******************************

	void    SetChannel(ABaseChannel* channel)
	{
		m_pChannel = channel;
	};

	// ******************************
	// * GetPriority
	// *-----------------------------
	/*! Return the priority of this stream
	*/
	// ******************************

	IntU32            GetPriority()   const
	{
		return m_Priority;
	};

	// ******************************
	// * SetPriority
	// *-----------------------------
	/*! Set the priority of this stream, you must not change priority
	without resorting the stream list
	*/
	// ******************************

	void            SetPriority(IntU32    priority)
	{
		m_Priority = priority;
	};

	// ******************************
	// * GetWeight
	// *-----------------------------
	/*! return the weight of the stream
	*/
	// ******************************

	Float           GetWeight() const
	{
		return m_Weight;
	};

	// ******************************
	// * SetWeight
	// *-----------------------------
	/*! Set the weight of the stream
	*/
	// ******************************

	void            SetWeight(Float weight);

	// ******************************
	// * GetSpeed
	// *-----------------------------
	/*! return the speed of the stream
	*/
	// ******************************

	Float           GetSpeed()  const
	{
		return m_Speed;
	};

	// ******************************
	// * SetSpeed
	// *-----------------------------
	/*! set the speed of the stream
	*/
	// ******************************

	void            SetSpeed(Float speed)
	{
		// here we need to patch starttime so that the animation is still correct

		if (speed != KFLOAT_CONST(0.0))
		{
			if (m_Speed != KFLOAT_CONST(0.0))
			{
				m_StartTime += (ATimeValue)((Float)m_LocalTime / m_Speed - (Float)m_LocalTime / speed);
			}
			else
			{
				m_StartTime = (ATimeValue)(m_StartTime - (ATimeValue)((Float)m_LocalTime / speed));
			}
		}

		m_Speed = speed;
	};

	// ******************************
	// * ForceNextStream
	// *-----------------------------
	/*! Set next stream in the stream list
	*/
	// ******************************

	void            ForceNextStream(const ABaseStream*  next)
	{
		m_pNextStream = (ABaseStream *)next;
	};



	AnimationResourceInfo*  m_pResourceInfo;


	bool            m_IsPlaying;
	bool            m_Loop;
	IntU32          m_Priority;
	Float           m_Weight;
	Float           m_Speed;
	ATimeValue      m_LocalTime;
	ATimeValue      m_StartTime;
	bool            m_OutsideAnimFlag;
	bool            m_bEndReached;
	bool	        m_HasLoop;
	IntS32          m_RepeatCount;

	// The next stream in the linked list
	ABaseStream*        m_pNextStream;

	ABaseChannel*       m_pChannel;
};



#endif //__ABASESTREAM_H__