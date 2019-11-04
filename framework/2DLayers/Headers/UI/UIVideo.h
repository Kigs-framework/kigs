#ifndef _UIVIDEO_H_
#define _UIVIDEO_H_

#include "UIItem.h"
#include "SmartPointer.h"

class Texture;

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

	Texture*		GetTexture();
	Texture*		Get_BufferTexture();
	inline bool		Get_Loop() const {return myBLoop;}
	inline bool		Get_HideAtTheEnd() const {return myBHideAtTheEnd;}
	kstl::string	Get_VideoName() const {return (kstl::string)myVideoName;}
	kstl::string	Get_VideoOrder() const {return ((const kstl::string&)m_VideoOrder);}
	inline bool		VideoHasAlpha() const {return mybHasAlpha;}
	inline bool		Get_IsMuted() const {return mybIsMuted;}
	inline float	Get_Volume() const {return myVolume;}

	inline bool		Get_Finished() const {return myBFinished;}

	inline void		Play(){m_VideoOrder.setValue("Play");}
	inline void		Pause(){m_VideoOrder.setValue("Pause");}
	inline void		Stop(){m_VideoOrder.setValue("Stop");}

	 void	ReloadTexture();

protected:
	virtual			 ~UIVideo();
	void InitModifiable()override;
	void NotifyUpdate(const unsigned int /* labelid */)override;
	bool isAlpha(float X, float Y)override;

	maBool			myBLoop;
	maBool			myBHideAtTheEnd;
	maBool			mybHasAlpha;
	maBool			mybIsMuted;

	maFloat			myVolume;

	SmartPointer<Texture>	myTexture;
	SmartPointer<Texture>	myBufferTexture;
	maString		myVideoName;
	bool			myBFinished;
	maEnum<3>		m_VideoOrder;
};

#endif //_UIVIDEO_H_