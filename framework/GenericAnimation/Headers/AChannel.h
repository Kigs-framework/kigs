#ifndef __ACHANNEL_H__
#define __ACHANNEL_H__

#include "ABaseChannel.h"
#include "ASystem.h"

// +---------
//  AChannel declaration
/*! Class used to manage animation on a given group
\ingroup Animation
*/
// +--------

template<typename LocalToGlobalType>
class   AChannel : public ABaseChannel
{
	
	public:
	
	DECLARE_ABSTRACT_CLASS_INFO(AChannel, ABaseChannel, Animation);
	DECLARE_CONSTRUCTOR(AChannel);
	
	
	// ******************************
	// * AnimateRoot
	// *-----------------------------
	/*!  called by the system to setup animation at time t
	for the root channel
	*/
	// ******************************
	
	void    AnimateRoot(ATimeValue t, SP<ABaseSystem> system);
	
	// ******************************
	// * Animate
	// *-----------------------------
	/*!  called by the system to setup animation at time t
	
	*/
	// ******************************
	
	void    Animate(ATimeValue t, SP<AChannel> otherchannel);
	
	// ******************************
	// * GetChannelLocalToGlobalData
	// *-----------------------------
	/*! return the channel local to global data
	*/
	// ******************************
	
	LocalToGlobalBaseType* GetChannelLocalToGlobalData() override
	{
		return &mLocalToGlobalStreamData;
	};
	
	
	bool	isAnimated() { return mFirstStream != 0; }
	
	//protected:
	
	virtual ~AChannel();
	
	// +---------
	// | protected members
	// +---------
	LocalToGlobalType							mWorkingStreamData;
	LocalToGlobalType							mStandStreamData;
	LocalToGlobalType							mLocalToGlobalStreamData;
	
	
	
	// ******************************
	// * GetLocalToGlobalBeforeChange
	// *-----------------------------
	/*! Used to change root channel data when mWeight are changed, or when
	a animation is add or sub ...
	*/
	// ******************************
	
	LocalToGlobalBaseType*   GetLocalToGlobalBeforeChange() override;
	
	// ******************************
	// * ResetLocalToGlobalAfterChange
	// *-----------------------------
	/*! After the mWeight change or sub/add animation done, reset
	the local to global data, so the animation does not "jump"
	*/
	// ******************************
	
	void    ResetLocalToGlobalAfterChange(LocalToGlobalBaseType* tmp_data) override;
	
	
	
	// ******************************
	// * GetStandData
	// *-----------------------------
	/*! Return a pointer on this AChannel stand data
	*/
	// ******************************
	
	LocalToGlobalBaseType*       GetStandData() override
	{
		return &mStandStreamData;
	};
	
	// ******************************
	// * SetStandData
	// *-----------------------------
	/*! set the stand data for the channel
	*/
	// ******************************
	
	void       SetStandData(LocalToGlobalBaseType* stddata) override
	{
		mStandStreamData = *(LocalToGlobalType*)stddata;
	};
	
	// ******************************
	// * UpdateTransformParameters
	// *-----------------------------
	/*! after the stream mix is done, the parameters for the transformation must be update
	*/
	// ******************************
	
	virtual void    UpdateTransformParameters() = 0;
	
	// ******************************
	// * SetStandStreamData
	// *-----------------------------
	/*! Set the stand stream data, called by the ASystem InitChanneld method
	*/
	// ******************************
	
	virtual void    SetStandStreamData() = 0;
	
	// ******************************
	// * UpdateLocalToGlobalData
	// *-----------------------------
	/*!  Called after the stream mixing is done, set the local to global data
	given the father channel local to global data
	*/
	// ******************************
	
	void            UpdateLocalToGlobalData(SP<AChannel> otherchannel);
	
	
};


IMPLEMENT_TEMPLATE_CLASS_INFO(LocalToGlobalType, AChannel)


template<typename LocalToGlobalType>
AChannel<LocalToGlobalType>::AChannel(const kstl::string& name, CLASS_NAME_TREE_ARG) : ABaseChannel(name, PASS_CLASS_NAME_TREE_ARG)
{
	//! init members
	
}

template<typename LocalToGlobalType>
AChannel<LocalToGlobalType>::~AChannel()
{
	
};

// ******************************
// * AnimateRoot
// *-----------------------------
//!* -  Do the stream mixing,call UpdateTransformParameters, call the ModifierChannel Animate() 
//!*    and then call the sons Animate()
// ******************************
template<typename LocalToGlobalType>
void    AChannel<LocalToGlobalType>::AnimateRoot(ATimeValue t, SP<ABaseSystem> _sys)
{
	
	//+---------
	//! Stream mixing
	//+---------
	
	SP<ASystem<LocalToGlobalType>>	sys = _sys;
	
	SP<ABaseStream>     read = mFirstStream;
	
	SP<ABaseStream>     valid_stream = mSystem->GetValidStream();
	
	if (read == nullptr)
	{
		if (valid_stream)
		{
			valid_stream->CopyData(&mWorkingStreamData, &mStandStreamData);
		}
		else
		{
			return;
		}
	}
	
	Float       mWeight = KFLOAT_CONST(0.0f);
	Float       mCoefT;
	
	//! first test if we use the animation local to global data
	//! ( else we just have to copy the data given by user )
	
	if (sys->mUseAnimationLocalToGlobal == true)
	{
		//! check if we have to update the local to global data when the animation loop
		//! ( else we just restart the animation from the starting PRS data )
		if (sys->mUpdateLocalToGlobalWhenLoop == true)
		{
			//+-----
			//| get the global data without loop
			//+-----
			bool   offset_init = false;
			
			kstl::vector<ATimeValue>	StoredTimes;
			StoredTimes.reserve(10);
			// find the first playing stream, and init the working data with it
			// in the same time check if the local time is outside the anim time ( offset_init == true )
			while (read != nullptr)
			{
				if (read->IsPlaying())
				{
					StoredTimes.push_back(read->GetLocalTime());
					read->SetTimeWithoutLoop(t);
					if (read->IsOutsideAnim())
					{
						offset_init = true;
					}
					read->InitData(&mWorkingStreamData);
					mWeight = read->GetWeight();
					read = read->GetNextStream();
					break;
				}
				
				read = read->GetNextStream();
			}
			
			// find other playing streams ( if any )
			// and also check the time ( offset_init == true )
			
			while (read != nullptr)
			{
				if (read->IsPlaying())
				{
					mWeight += read->GetWeight();
					StoredTimes.push_back(read->GetLocalTime());
					read->SetTimeWithoutLoop(t);
					if (read->IsOutsideAnim() == true)
					{
						offset_init = true;
					}
					
					if (mWeight)
					{
						mCoefT = read->GetWeight()*(KFLOAT_CONST(1.0f) / mWeight);
						read->LERPData(&mWorkingStreamData, mCoefT);
					}
				}
				read = read->GetNextStream();
			}
			
			// so offset_init == true ? we have to update the starting local to global position
			// before proceeding
			if (offset_init == true)
			{
				LocalToGlobalBaseType*   tmp_data = mFirstStream->NewStreamOutputDataInstance();
				mFirstStream->CopyData(tmp_data, &mWorkingStreamData);
				
				// +-----
				// | then get the offset 
				// +-----
				mWeight = KFLOAT_CONST(0.0);
				read = mFirstStream;
				offset_init = false;
				
				while (read != nullptr)
				{
					if (read->IsPlaying())
					{
						if (read->IsOutsideAnim() == true)
						{
							read->SetTimeWithStartLoop(t);
						}
						
						read->InitData(&mWorkingStreamData);
						mWeight = read->GetWeight();
						read = read->GetNextStream();
						break;
					}
					
					read = read->GetNextStream();
				}
				
				while (read != nullptr)
				{
					if (read->IsPlaying())
					{
						mWeight += read->GetWeight();
						
						if (read->IsOutsideAnim() == true)
						{
							read->SetTimeWithStartLoop(t);
						}
						if (mWeight)
						{
							mCoefT = read->GetWeight()*(KFLOAT_CONST(1.0f) / mWeight);
							read->LERPData(&mWorkingStreamData, mCoefT);
						}
						
					}
					read = read->GetNextStream();
				}
				
				// tmp_data             : contains the PRS just before looping through the 
				//                        animation relative to the StartingLocalToGlobalData
				// mWorkingStreamData : contains the PRS just after looping through the animation 
				//                        relative to the StartingLocalToGlobalData
				
				// m_pStartingLocalToGlobalData is the PRS that defines where the animation
				// started at the begin of the last loop or at the time t=0.
				
				// We first transform tmp_data and mWorkingStreamData in the world coordinate
				// system by using the PRS transformation m_pStartingLocalToGlobalData
				// Next we modify m_pStartingLocalToGlobalData so that the "PRS just after" in the
				// new StartingLocalToGlobalData frame correspond to the "PRS just before" in the
				// old StartingLocalToGlobalData frame so that the movement is continuous.
				
				mFirstStream->MulData(tmp_data, &sys->m_pStartingLocalToGlobalData);
				mFirstStream->MulData(&mWorkingStreamData, &sys->m_pStartingLocalToGlobalData);
				mFirstStream->SetAndModifyData(&mWorkingStreamData, tmp_data, &sys->m_pStartingLocalToGlobalData);
				
				// then just do as if animation was playing normally
				IntU32	countplayingstream = 0;
				
				read = mFirstStream;
				while (read != nullptr)
				{
					if (read->IsPlaying())
					{
						read->SetLocalTime(StoredTimes[countplayingstream++]);
						read->SetTime(t);
						read->InitData(&mWorkingStreamData);
						mWeight = read->GetWeight();
						read = read->GetNextStream();
						break;
					}
					
					read = read->GetNextStream();
				}
				
				
				while (read != nullptr)
				{
					if (read->IsPlaying())
					{
						mWeight += read->GetWeight();
						read->SetLocalTime(StoredTimes[countplayingstream++]);
						read->SetTime(t);
						if (mWeight)
						{
							mCoefT = read->GetWeight()*(KFLOAT_CONST(1.0f) / mWeight);
							read->LERPData(&mWorkingStreamData, mCoefT);
						}
					}
					read = read->GetNextStream();
				}
				
				mFirstStream->DeleteStreamOutputDataInstance(tmp_data);
			}
			else
			{
				// if animation has not loop and no stream are playing, just copy
				// stand data to the working data
				if (mWeight == KFLOAT_CONST(0.0f))
				{
					valid_stream->CopyData(&mWorkingStreamData, &mStandStreamData);
				}
			}
		}
		else // the animation is not in update when loop mode , so we play it normally
		{
			while (read != nullptr)
			{
				if (read->IsPlaying())
				{
					read->SetTime(t);
					read->InitData(&mWorkingStreamData);
					mWeight = read->GetWeight();
					read = read->GetNextStream();
					break;
				}
				
				read = read->GetNextStream();
			}
			
			
			while (read != nullptr)
			{
				if (read->IsPlaying())
				{
					mWeight += read->GetWeight();
					read->SetTime(t);
					if (mWeight)
					{
						mCoefT = read->GetWeight()*(KFLOAT_CONST(1.0f) / mWeight);
						read->LERPData(&mWorkingStreamData, mCoefT);
					}
				}
				read = read->GetNextStream();
			}
			
			if (mWeight == KFLOAT_CONST(0.0f))
			{
				valid_stream->CopyData(&mWorkingStreamData, &mStandStreamData);
			}
		}
		// Calc the matrix of the root node based on the starting point and the current animation data
		valid_stream->MulData(&mWorkingStreamData, &sys->m_pStartingLocalToGlobalData);
		// Copy it to m_pInstantLocalToGlobalData to move the Node3D later
		valid_stream->CopyData(&sys->m_pInstantLocalToGlobalData, &mWorkingStreamData);
	}
	
	
	// else
	// The LocalToGlobal of the root node is already applied by the Node3D it's attached on, we don't want to apply it twice so we 
	// set it to the identity
	{	
		valid_stream->IdentityData(&mLocalToGlobalStreamData);
		UpdateTransformParameters();
	}
	
	// +---------
	// | now call son animate
	// +---------
	
	if (mSystem->mRecurseAnimate)
	{
		SP<AChannel>	param = nullptr;

		if (mSystem->mOnlyLocalSkeletonUpdate == false)
		{
			param = SharedFromThis();
		}
		kstl::vector<ModifiableItemStruct>::const_iterator it;
			
		for (const auto& i : getItems())
		{
			if (i.mItem->isSubType(AChannel::mClassID))
			{
				((AChannel*)i.mItem.get())->Animate(t, param);
			}
		}
	}
};


// ******************************
// * Animate
// *-----------------------------
// * -  Do the stream mixing,call UpdateTransformParameters, call the ModifierChannel Animate() 
// *    and the call the sons Animate()
// ******************************
template<typename LocalToGlobalType>
void    AChannel<LocalToGlobalType>::Animate(ATimeValue t, SP<AChannel> otherchannel)
{
	// +---------
	// | Stream mixing
	// +---------
	
	SP<ABaseStream>     read = mFirstStream;
	
	if (read == nullptr)
	{
		SP<ABaseStream>     valid_stream = mSystem->GetValidStream();
		if (valid_stream)
		{
			valid_stream->CopyData(&mWorkingStreamData, &mStandStreamData);
		}
	}
	else
	{
		
		Float       weight = 0;
		Float       coef_t;
		
		while (read != nullptr)
		{
			if (read->IsPlaying())
			{
				read->SetTime(t);
				read->InitData(&mWorkingStreamData);
				weight = read->GetWeight();
				read = read->GetNextStream();
				break;
			}
			
			read = read->GetNextStream();
		}
		
		
		while (read != nullptr)
		{
			if (read->IsPlaying())
			{
				weight += read->GetWeight();
				read->SetTime(t);
				if (weight)
				{
					coef_t = read->GetWeight()*(KFLOAT_CONST(1.0f) / weight);
					read->LERPData(&mWorkingStreamData, coef_t);
				}
			}
			read = read->GetNextStream();
		}
		if (weight == KFLOAT_CONST(0.0f))
		{
			SP<ABaseStream>     valid_stream = mSystem->GetValidStream();
			if (valid_stream)
			{
				valid_stream->CopyData(&mWorkingStreamData, &mStandStreamData);
			}
		}
	}
	
	if (otherchannel)
	{
		UpdateLocalToGlobalData(otherchannel);
	}
	else
	{
		SP<ABaseStream>     valid_stream = mSystem->GetValidStream();
		
		if (valid_stream)
		{
			valid_stream->CopyData(&mLocalToGlobalStreamData, &mWorkingStreamData);
		}
	}
	
	UpdateTransformParameters();
	
	
	// +---------
	// | now call son animate
	// +---------
	
	if (mSystem->mRecurseAnimate)
	{
		if (otherchannel && (mSystem->mOnlyLocalSkeletonUpdate == false))
		{
			kstl::vector<ModifiableItemStruct>::const_iterator it;
			
			for (it = getItems().begin(); it != getItems().end(); ++it)
			{
				if ((*it).mItem->isSubType(AChannel::mClassID))
				{
					((AChannel*)(*it).mItem.get())->Animate(t, SharedFromThis());
				}
			}
		}
		else
		{
			kstl::vector<ModifiableItemStruct>::const_iterator it;
			
			for (it = getItems().begin(); it != getItems().end(); ++it)
			{
				if ((*it).mItem->isSubType(AChannel::mClassID))
				{
					((AChannel*)(*it).mItem.get())->Animate(t, 0);
				}
			}
		}
	}
	
};


/*!******************************
// * GetLocalToGlobalBeforeChange
// *-----------------------------
// * -
// * -
// ******************************/
template<typename LocalToGlobalType>
LocalToGlobalBaseType*   AChannel<LocalToGlobalType>::GetLocalToGlobalBeforeChange()
{
	LocalToGlobalBaseType*   tmp_data = nullptr;
	
	
	SP<ASystem<LocalToGlobalType>> sys = GetSystem();
	
	if (sys->GetUseAnimationLocalToGlobal() == true)
	{
		if (mFirstStream != nullptr)
		{
			if (mFirstStream->GetNextStream() != nullptr)
			{
				tmp_data = mFirstStream->NewStreamOutputDataInstance();
				mFirstStream->CopyData(tmp_data, &sys->m_pInstantLocalToGlobalData);
			}
		}
	}
	
	return tmp_data;
};

/*!******************************
// * ResetLocalToGlobalAfterChange
// *-----------------------------
// * -
// * -
// ******************************/
template<typename LocalToGlobalType>
void    AChannel<LocalToGlobalType>::ResetLocalToGlobalAfterChange(LocalToGlobalBaseType* tmp_data)
{
	if (tmp_data == nullptr)
	{
		return;
	}
	SP<ASystem<LocalToGlobalType>> sys = GetSystem();
	if (sys->GetUseAnimationLocalToGlobal() == true)
	{
		Float  weight = KFLOAT_CONST(0.0f);
		Float  coef_t;
		SP<ABaseStream> read = mFirstStream;
		while (read != nullptr)
		{
			if (read->IsPlaying())
			{
				
				read->UpdateData(GetStandData());
				
				read->InitData(&mWorkingStreamData);
				weight = read->GetWeight();
				read = read->GetNextStream();
				break;
			}
			
			read = read->GetNextStream();
		}
		
		
		while (read != nullptr)
		{
			if (read->IsPlaying())
			{
				read->UpdateData(GetStandData());
				
				weight += read->GetWeight();
				coef_t = read->GetWeight()*(KFLOAT_CONST(1.0f) / weight);
				read->LERPData(&mWorkingStreamData, coef_t);
			}
			read = read->GetNextStream();
		}
		
		mFirstStream->MulData(&mWorkingStreamData, &sys->m_pStartingLocalToGlobalData);
		mFirstStream->CopyData(&sys->m_pInstantLocalToGlobalData, &mWorkingStreamData);
		sys->SetLocalToGlobalData(tmp_data);
		mFirstStream->DeleteStreamOutputDataInstance(tmp_data);
	}
};

template<typename LocalToGlobalType>
void    AChannel<LocalToGlobalType>::UpdateLocalToGlobalData(SP<AChannel> otherchannel)
{
	SP<ABaseStream>     valid_stream = mSystem->GetValidStream();
	
	if (valid_stream)
	{
		valid_stream->CopyData(&mLocalToGlobalStreamData, &mWorkingStreamData);
		valid_stream->MulData(&mLocalToGlobalStreamData, &otherchannel->mLocalToGlobalStreamData);
	}
};


#endif //__ACHANNEL_H__