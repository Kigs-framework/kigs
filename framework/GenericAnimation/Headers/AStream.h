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
		if (mIsPlaying == false)
		{
			if (mChannel)
				if (mChannel->IsRootChannel())
				{
					LocalToGlobalBaseType*   tmp_data = ((AChannel<LocalToGlobalType>*)mChannel)->GetLocalToGlobalBeforeChange();
					mIsPlaying = true;
					((AChannel<LocalToGlobalType>*)mChannel)->ResetLocalToGlobalAfterChange(tmp_data);
				}

			mIsPlaying = true;
			mRepeatCount = 1;
			mEndReached = false;
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
		if (mIsPlaying == true)
		{
			if (mChannel)
				if (mChannel->IsRootChannel())
				{
					LocalToGlobalBaseType*   tmp_data = ((AChannel<LocalToGlobalType>*)mChannel)->GetLocalToGlobalBeforeChange();
					mIsPlaying = false;
					((AChannel<LocalToGlobalType>*)mChannel)->ResetLocalToGlobalAfterChange(tmp_data);
				}

			mIsPlaying = false;
		}
	};
};



IMPLEMENT_TEMPLATE_CLASS_INFO(LocalToGlobalType, AStream)


template<typename LocalToGlobalType>
AStream<LocalToGlobalType>::AStream(const kstl::string& name, CLASS_NAME_TREE_ARG) : ABaseStream(name, PASS_CLASS_NAME_TREE_ARG)
{
	mNextStream = NULL;
	InitStream(NULL, 100, KFLOAT_CONST(1.0f), KFLOAT_CONST(1.0f), true);
	mIsPlaying = false;
	mOutsideAnimFlag = false;
	mHasLoop = 0;
	mRepeatCount = 1;
	mResourceInfo = NULL;
}



#endif //__ASTREAM_H__


