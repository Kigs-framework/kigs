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
	
	void    AnimateRoot(ATimeValue t, ABaseSystem* system);
	
	// ******************************
	// * Animate
	// *-----------------------------
	/*!  called by the system to setup animation at time t
	
	*/
	// ******************************
	
	void    Animate(ATimeValue t, AChannel* otherchannel);
	
	// ******************************
	// * GetChannelLocalToGlobalData
	// *-----------------------------
	/*! return the channel local to global data
	*/
	// ******************************
	
	LocalToGlobalBaseType* GetChannelLocalToGlobalData() override
	{
		return &m_pLocalToGlobalStreamData;
	};
	
	
	bool	isAnimated() { return m_pFirstStream != 0; }
	
	//protected:
	
	virtual ~AChannel();
	
	// +---------
	// | protected members
	// +---------
	LocalToGlobalType							m_pWorkingStreamData;
	LocalToGlobalType							m_pStandStreamData;
	LocalToGlobalType							m_pLocalToGlobalStreamData;
	
	
	
	// ******************************
	// * GetLocalToGlobalBeforeChange
	// *-----------------------------
	/*! Used to change root channel data when weight are changed, or when
	a animation is add or sub ...
	*/
	// ******************************
	
	LocalToGlobalBaseType*   GetLocalToGlobalBeforeChange() override;
	
	// ******************************
	// * ResetLocalToGlobalAfterChange
	// *-----------------------------
	/*! After the weight change or sub/add animation done, reset
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
		return &m_pStandStreamData;
	};
	
	// ******************************
	// * SetStandData
	// *-----------------------------
	/*! set the stand data for the channel
	*/
	// ******************************
	
	void       SetStandData(LocalToGlobalBaseType* stddata) override
	{
		m_pStandStreamData = *(LocalToGlobalType*)stddata;
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
	
	void            UpdateLocalToGlobalData(AChannel* otherchannel);
	
	
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
void    AChannel<LocalToGlobalType>::AnimateRoot(ATimeValue t, ABaseSystem* _sys)
{
	
	//+---------
	//! Stream mixing
	//+---------
	
	ASystem<LocalToGlobalType>*	sys = (ASystem<LocalToGlobalType>*)_sys;
	
	ABaseStream*     read = m_pFirstStream;
	
	ABaseStream*     valid_stream = m_pSystem->GetValidStream();
	
	if (read == NULL)
	{
		if (valid_stream != NULL)
		{
			valid_stream->CopyData(&m_pWorkingStreamData, &m_pStandStreamData);
		}
		else
		{
			return;
		}
	}
	
	Float       weight = KFLOAT_CONST(0.0f);
	Float       coef_t;
	
	//! first test if we use the animation local to global data
	//! ( else we just have to copy the data given by user )
	
	if (sys->m_UseAnimationLocalToGlobal == true)
	{
		//! check if we have to update the local to global data when the animation loop
		//! ( else we just restart the animation from the starting PRS data )
		if (sys->m_UpdateLocalToGlobalWhenLoop == true)
		{
			//+-----
			//| get the global data without loop
			//+-----
			bool   offset_init = false;
			
			kstl::vector<ATimeValue>	StoredTimes;
			StoredTimes.reserve(10);
			// find the first playing stream, and init the working data with it
			// in the same time check if the local time is outside the anim time ( offset_init == true )
			while (read != NULL)
			{
				if (read->IsPlaying())
				{
					StoredTimes.push_back(read->GetLocalTime());
					read->SetTimeWithoutLoop(t);
					if (read->IsOutsideAnim())
					{
						offset_init = true;
					}
					read->InitData(&m_pWorkingStreamData);
					weight = read->GetWeight();
					read = read->GetNextStream();
					break;
				}
				
				read = read->GetNextStream();
			}
			
			// find other playing streams ( if any )
			// and also check the time ( offset_init == true )
			
			while (read != NULL)
			{
				if (read->IsPlaying())
				{
					weight += read->GetWeight();
					StoredTimes.push_back(read->GetLocalTime());
					read->SetTimeWithoutLoop(t);
					if (read->IsOutsideAnim() == true)
					{
						offset_init = true;
					}
					
					if (weight)
					{
						coef_t = read->GetWeight()*(KFLOAT_CONST(1.0f) / weight);
						read->LERPData(&m_pWorkingStreamData, coef_t);
					}
				}
				read = read->GetNextStream();
			}
			
			// so offset_init == true ? we have to update the starting local to global position
			// before proceeding
			if (offset_init == true)
			{
				LocalToGlobalBaseType*   tmp_data = m_pFirstStream->NewStreamOutputDataInstance();
				m_pFirstStream->CopyData(tmp_data, &m_pWorkingStreamData);
				
				// +-----
				// | then get the offset 
				// +-----
				weight = KFLOAT_CONST(0.0);
				read = m_pFirstStream;
				offset_init = false;
				
				while (read != NULL)
				{
					if (read->IsPlaying())
					{
						if (read->IsOutsideAnim() == true)
						{
							read->SetTimeWithStartLoop(t);
						}
						
						read->InitData(&m_pWorkingStreamData);
						weight = read->GetWeight();
						read = read->GetNextStream();
						break;
					}
					
					read = read->GetNextStream();
				}
				
				while (read != NULL)
				{
					if (read->IsPlaying())
					{
						weight += read->GetWeight();
						
						if (read->IsOutsideAnim() == true)
						{
							read->SetTimeWithStartLoop(t);
						}
						if (weight)
						{
							coef_t = read->GetWeight()*(KFLOAT_CONST(1.0f) / weight);
							read->LERPData(&m_pWorkingStreamData, coef_t);
						}
						
					}
					read = read->GetNextStream();
				}
				
				// tmp_data             : contains the PRS just before looping through the 
				//                        animation relative to the StartingLocalToGlobalData
				// m_pWorkingStreamData : contains the PRS just after looping through the animation 
				//                        relative to the StartingLocalToGlobalData
				
				// m_pStartingLocalToGlobalData is the PRS that defines where the animation
				// started at the begin of the last loop or at the time t=0.
				
				// We first transform tmp_data and m_pWorkingStreamData in the world coordinate
				// system by using the PRS transformation m_pStartingLocalToGlobalData
				// Next we modify m_pStartingLocalToGlobalData so that the "PRS just after" in the
				// new StartingLocalToGlobalData frame correspond to the "PRS just before" in the
				// old StartingLocalToGlobalData frame so that the movement is continuous.
				
				m_pFirstStream->MulData(tmp_data, &sys->m_pStartingLocalToGlobalData);
				m_pFirstStream->MulData(&m_pWorkingStreamData, &sys->m_pStartingLocalToGlobalData);
				m_pFirstStream->SetAndModifyData(&m_pWorkingStreamData, tmp_data, &sys->m_pStartingLocalToGlobalData);
				
				// then just do as if animation was playing normally
				IntU32	countplayingstream = 0;
				
				read = m_pFirstStream;
				while (read != NULL)
				{
					if (read->IsPlaying())
					{
						read->SetLocalTime(StoredTimes[countplayingstream++]);
						read->SetTime(t);
						read->InitData(&m_pWorkingStreamData);
						weight = read->GetWeight();
						read = read->GetNextStream();
						break;
					}
					
					read = read->GetNextStream();
				}
				
				
				while (read != NULL)
				{
					if (read->IsPlaying())
					{
						weight += read->GetWeight();
						read->SetLocalTime(StoredTimes[countplayingstream++]);
						read->SetTime(t);
						if (weight)
						{
							coef_t = read->GetWeight()*(KFLOAT_CONST(1.0f) / weight);
							read->LERPData(&m_pWorkingStreamData, coef_t);
						}
					}
					read = read->GetNextStream();
				}
				
				m_pFirstStream->DeleteStreamOutputDataInstance(tmp_data);
			}
			else
			{
				// if animation has not loop and no stream are playing, just copy
				// stand data to the working data
				if (weight == KFLOAT_CONST(0.0f))
				{
					valid_stream->CopyData(&m_pWorkingStreamData, &m_pStandStreamData);
				}
			}
		}
		else // the animation is not in update when loop mode , so we play it normally
		{
			while (read != NULL)
			{
				if (read->IsPlaying())
				{
					read->SetTime(t);
					read->InitData(&m_pWorkingStreamData);
					weight = read->GetWeight();
					read = read->GetNextStream();
					break;
				}
				
				read = read->GetNextStream();
			}
			
			
			while (read != NULL)
			{
				if (read->IsPlaying())
				{
					weight += read->GetWeight();
					read->SetTime(t);
					if (weight)
					{
						coef_t = read->GetWeight()*(KFLOAT_CONST(1.0f) / weight);
						read->LERPData(&m_pWorkingStreamData, coef_t);
					}
				}
				read = read->GetNextStream();
			}
			
			if (weight == KFLOAT_CONST(0.0f))
			{
				valid_stream->CopyData(&m_pWorkingStreamData, &m_pStandStreamData);
			}
		}
		// Calc the matrix of the root node based on the starting point and the current animation data
		valid_stream->MulData(&m_pWorkingStreamData, &sys->m_pStartingLocalToGlobalData);
		// Copy it to m_pInstantLocalToGlobalData to move the Node3D later
		valid_stream->CopyData(&sys->m_pInstantLocalToGlobalData, &m_pWorkingStreamData);
	}
	
	
	// else
	// The LocalToGlobal of the root node is already applied by the Node3D it's attached on, we don't want to apply it twice so we 
	// set it to the identity
	{	
		valid_stream->IdentityData(&m_pLocalToGlobalStreamData);
		UpdateTransformParameters();
	}
	
	// +---------
	// | now call son animate
	// +---------
	
	if (m_pSystem->m_RecurseAnimate)
	{
		if (m_pSystem->m_OnlyLocalSkeletonUpdate == false)
		{
			kstl::vector<ModifiableItemStruct>::const_iterator it;
			
			for (it = getItems().begin(); it != getItems().end(); ++it)
			{
				if ((*it).myItem->isSubType(AChannel::myClassID))
				{
					((AChannel*)(*it).myItem)->Animate(t, this);
				}
			}
		}
		else
		{
			
			kstl::vector<ModifiableItemStruct>::const_iterator it;
			
			for (it = getItems().begin(); it != getItems().end(); ++it)
			{
				if ((*it).myItem->isSubType(AChannel::myClassID))
				{
					((AChannel*)(*it).myItem)->Animate(t, 0);
				}
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
void    AChannel<LocalToGlobalType>::Animate(ATimeValue t, AChannel* otherchannel)
{
	// +---------
	// | Stream mixing
	// +---------
	
	ABaseStream*     read = m_pFirstStream;
	
	if (read == NULL)
	{
		ABaseStream*     valid_stream = m_pSystem->GetValidStream();
		if (valid_stream != NULL)
		{
			valid_stream->CopyData(&m_pWorkingStreamData, &m_pStandStreamData);
		}
	}
	else
	{
		
		Float       weight = 0;
		Float       coef_t;
		
		while (read != NULL)
		{
			if (read->IsPlaying())
			{
				read->SetTime(t);
				read->InitData(&m_pWorkingStreamData);
				weight = read->GetWeight();
				read = read->GetNextStream();
				break;
			}
			
			read = read->GetNextStream();
		}
		
		
		while (read != NULL)
		{
			if (read->IsPlaying())
			{
				weight += read->GetWeight();
				read->SetTime(t);
				if (weight)
				{
					coef_t = read->GetWeight()*(KFLOAT_CONST(1.0f) / weight);
					read->LERPData(&m_pWorkingStreamData, coef_t);
				}
			}
			read = read->GetNextStream();
		}
		if (weight == KFLOAT_CONST(0.0f))
		{
			ABaseStream*     valid_stream = m_pSystem->GetValidStream();
			if (valid_stream != NULL)
			{
				valid_stream->CopyData(&m_pWorkingStreamData, &m_pStandStreamData);
			}
		}
	}
	
	if (otherchannel != NULL)
	{
		UpdateLocalToGlobalData(otherchannel);
	}
	else
	{
		ABaseStream*     valid_stream = m_pSystem->GetValidStream();
		
		if (valid_stream != NULL)
		{
			valid_stream->CopyData(&m_pLocalToGlobalStreamData, &m_pWorkingStreamData);
		}
	}
	
	UpdateTransformParameters();
	
	
	// +---------
	// | now call son animate
	// +---------
	
	if (m_pSystem->m_RecurseAnimate)
	{
		if ((otherchannel != NULL) && (m_pSystem->m_OnlyLocalSkeletonUpdate == false))
		{
			kstl::vector<ModifiableItemStruct>::const_iterator it;
			
			for (it = getItems().begin(); it != getItems().end(); ++it)
			{
				if ((*it).myItem->isSubType(AChannel::myClassID))
				{
					((AChannel*)(*it).myItem)->Animate(t, this);
				}
			}
		}
		else
		{
			kstl::vector<ModifiableItemStruct>::const_iterator it;
			
			for (it = getItems().begin(); it != getItems().end(); ++it)
			{
				if ((*it).myItem->isSubType(AChannel::myClassID))
				{
					((AChannel*)(*it).myItem)->Animate(t, 0);
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
	LocalToGlobalBaseType*   tmp_data = NULL;
	
	
	ASystem<LocalToGlobalType>* sys = (ASystem<LocalToGlobalType>*)GetSystem();
	
	if (sys->GetUseAnimationLocalToGlobal() == true)
	{
		if (m_pFirstStream != NULL)
		{
			if (m_pFirstStream->GetNextStream() != NULL)
			{
				tmp_data = m_pFirstStream->NewStreamOutputDataInstance();
				m_pFirstStream->CopyData(tmp_data, &sys->m_pInstantLocalToGlobalData);
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
	if (tmp_data == NULL)
	{
		return;
	}
	ASystem<LocalToGlobalType>* sys = (ASystem<LocalToGlobalType>*)GetSystem();
	if (sys->GetUseAnimationLocalToGlobal() == true)
	{
		Float  weight = KFLOAT_CONST(0.0f);
		Float  coef_t;
		ABaseStream* read = m_pFirstStream;
		while (read != NULL)
		{
			if (read->IsPlaying())
			{
				
				read->UpdateData(GetStandData());
				
				read->InitData(&m_pWorkingStreamData);
				weight = read->GetWeight();
				read = read->GetNextStream();
				break;
			}
			
			read = read->GetNextStream();
		}
		
		
		while (read != NULL)
		{
			if (read->IsPlaying())
			{
				read->UpdateData(GetStandData());
				
				weight += read->GetWeight();
				coef_t = read->GetWeight()*(KFLOAT_CONST(1.0f) / weight);
				read->LERPData(&m_pWorkingStreamData, coef_t);
			}
			read = read->GetNextStream();
		}
		
		m_pFirstStream->MulData(&m_pWorkingStreamData, &sys->m_pStartingLocalToGlobalData);
		m_pFirstStream->CopyData(&sys->m_pInstantLocalToGlobalData, &m_pWorkingStreamData);
		sys->SetLocalToGlobalData(tmp_data);
		m_pFirstStream->DeleteStreamOutputDataInstance(tmp_data);
	}
};

template<typename LocalToGlobalType>
void    AChannel<LocalToGlobalType>::UpdateLocalToGlobalData(AChannel* otherchannel)
{
	ABaseStream*     valid_stream = m_pSystem->GetValidStream();
	
	if (valid_stream != NULL)
	{
		valid_stream->CopyData(&m_pLocalToGlobalStreamData, &m_pWorkingStreamData);
		valid_stream->MulData(&m_pLocalToGlobalStreamData, &otherchannel->m_pLocalToGlobalStreamData);
	}
};


#endif //__ACHANNEL_H__