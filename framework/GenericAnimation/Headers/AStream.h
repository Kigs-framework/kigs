// ****************************************************************************
// * NAME: AStream.h
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE:   AStream is used to give to the corresponding channel, the data
// *            found in the animation resource.
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// * RELEASE: 
// ****************************************************************************


#ifndef __ASTREAM_H__
#define __ASTREAM_H__

#include "AMDefines.h"
#include "AChannel.h"


// +---------
// | AStream declaration
/*!  Class used to manage animation resource and format them for the AChannel
     \ingroup Animation
*/
// +---------

template<typename LocalToGlobalType>
class   AStream : public ABaseStream
{
	DECLARE_ABSTRACT_CLASS_INFO(AStream, ABaseStream, Animation)
public:
	DECLARE_CONSTRUCTOR(AStream);


	// ******************************
	// * Start / Stop animation for this stream
	// *-----------------------------
	/*! Start stream animation
	*/
	// ******************************

	void Start() override
	{
		if (m_IsPlaying == false)
		{
			if (m_pChannel)
				if (m_pChannel->IsRootChannel())
				{
					LocalToGlobalBaseType*   tmp_data = ((AChannel<LocalToGlobalType>*)m_pChannel)->GetLocalToGlobalBeforeChange();
					m_IsPlaying = true;
					((AChannel<LocalToGlobalType>*)m_pChannel)->ResetLocalToGlobalAfterChange(tmp_data);
				}

			m_IsPlaying = true;
			m_RepeatCount = 1;
			m_bEndReached = false;
		}
	};

	// ******************************
	// * Start / Stop animation for this stream
	// *-----------------------------
	/*! Stop stream animation
	*/
	// ******************************
	void Stop() override
	{
		if (m_IsPlaying == true)
		{
			if (m_pChannel)
				if (m_pChannel->IsRootChannel())
				{
					LocalToGlobalBaseType*   tmp_data = ((AChannel<LocalToGlobalType>*)m_pChannel)->GetLocalToGlobalBeforeChange();
					m_IsPlaying = false;
					((AChannel<LocalToGlobalType>*)m_pChannel)->ResetLocalToGlobalAfterChange(tmp_data);
				}

			m_IsPlaying = false;
		}
	};
};



IMPLEMENT_TEMPLATE_CLASS_INFO(LocalToGlobalType, AStream)


template<typename LocalToGlobalType>
AStream<LocalToGlobalType>::AStream(const kstl::string& name, CLASS_NAME_TREE_ARG) : ABaseStream(name, PASS_CLASS_NAME_TREE_ARG)
{
	m_pNextStream = NULL;
	InitStream(NULL, 100, KFLOAT_CONST(1.0f), KFLOAT_CONST(1.0f), true);
	m_IsPlaying = false;
	m_OutsideAnimFlag = false;
	m_HasLoop = 0;
	m_RepeatCount = 1;
	m_pResourceInfo = NULL;
}



#endif //__ASTREAM_H__


