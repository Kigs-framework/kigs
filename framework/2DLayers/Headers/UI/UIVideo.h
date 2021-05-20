#ifndef _UIVIDEO_H_
#define _UIVIDEO_H_

#include "UIItem.h"
#include "SmartPointer.h"

class Texture;

// ****************************************
// * UIVideo class
// * --------------------------------------
/**
* \file	UIVideo.h
* \class	UIVideo
* \ingroup 2DLayers
* \brief	Display a video.
*
* Obsolete ? Same as UIStream ?
*
*/
// ****************************************

class UIVideo : public UIItem
{
public:
	DECLARE_CLASS_INFO(UIVideo,UIItem,2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIVideo(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual	~UIVideo();

	Texture*		GetTexture();
	Texture*		Get_BufferTexture();
	inline bool		Get_Loop() const {return mLoop;}
	inline bool		Get_HideAtTheEnd() const {return mHideAtTheEnd;}
	kstl::string	Get_VideoName() const {return (kstl::string)mVideoName;}
	kstl::string	Get_VideoOrder() const {return ((const kstl::string&)mOrder);}
	inline bool		VideoHasAlpha() const {return mHasAlpha;}
	inline bool		Get_IsMuted() const {return mIsMuted;}
	inline float	Get_Volume() const {return mVolume;}

	inline bool		Get_Finished() const {return mFinished;}

	inline void		Play(){mOrder.setValue("Play");}
	inline void		Pause(){mOrder.setValue("Pause");}
	inline void		Stop(){mOrder.setValue("Stop");}

	 void	ReloadTexture();

protected:
	
	void InitModifiable()override;
	void NotifyUpdate(const unsigned int /* labelid */)override;
	bool isAlpha(float X, float Y)override;

	maBool			mLoop;
	maBool			mHideAtTheEnd;
	maBool			mHasAlpha;
	maBool			mIsMuted;

	maFloat			mVolume;

	SmartPointer<Texture>	mTexturePointer;
	SmartPointer<Texture>	mBufferTexturePointer;
	maString		mVideoName;
	bool			mFinished;
	maEnum<3>		mOrder;
};

#endif //_UIVIDEO_H_