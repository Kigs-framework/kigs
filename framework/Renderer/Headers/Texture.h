#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Drawable.h"
#include "TinyImage.h"
#include "TecLibs/2D/BBox2DI.h"


// data struct to delay init in draw (for single thread opengl)
struct TextureDelayedInitData
{
	SmartPointer<TinyImage> delayedimage;
	bool needRealloc;
};


// ****************************************
// * Texture class
// * --------------------------------------
/**
* \file	Texture.h
* \class	Texture
* \ingroup Renderer
* \brief manage a texture.
*
*/
// ****************************************
class Texture : public Drawable
{
public:
	static constexpr unsigned int isDirtyContext = 1<<usedFlags;
	static constexpr unsigned int usedFlags = Drawable::usedFlags+1;

	friend class RenderingScreen;


	DECLARE_ABSTRACT_CLASS_INFO(Texture,Drawable,Renderer)

	/**
	* \brief	constructor
	* \fn 		Texture(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	Texture(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	* \brief	set the repeat value
	* \fn 		void SetRepeatUV (bool RU, bool RV)
	* \param	RU : TRUE if repeat on u axis
	* \param	RV : TRUE if repeat on v axis
	*/
	void SetRepeatUV (bool RU, bool RV) {mRepeat_U = RU; mRepeat_V=RV;}

	/**
	* \brief	create a texture from an image
	* \fn 		virtual bool	CreateFromImage()=0;
	*/
	virtual bool	CreateFromImage(const SmartPointer<TinyImage>& image,bool directInit=false)=0;

	/**
	* \brief	update zone in texture (only RGBA8888)
	* \fn		virtual bool	UpdateBufferZone(...)=0;
	*/
	virtual bool	UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const Point2DI& bitmapSize)
	{
		assert(0);
		return false;
	}

	/**
	* \brief	create a texture from a text
	* \fn		virtual bool	CreateFromText(...)=0;
	*/
	bool	CreateFromText(const char* text, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1, float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888,  int a_drawingLimit = -1)
	{
		usString tmpText(text);
		return CreateFromText(tmpText.us_str(), fontSize, fontName, a_Alignment, R, G, B, A, format,  a_drawingLimit);
	}

	/**
	* \brief	create a texture from a text in utf16
	* \fn		virtual bool	CreateFromBuffer(...)=0;
	*/
	virtual bool	CreateFromText(const unsigned short* text, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1, float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888, int a_drawingLimit = -1)
	{
		assert(0);
		return false;
	}

	/**
	* \brief	create a texture from a multiline text
	* \fn		virtual bool	CreateFromText(...)=0;
	*/

	bool	CreateFromText(const char* text, unsigned int _maxLineNumber = 0, unsigned int maxSize = 128, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1,  float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888,  int a_drawingLimit = -1)
	{
		usString tmpText(text);
		return  CreateFromText(tmpText.us_str(), _maxLineNumber, maxSize, fontSize, fontName, a_Alignment, R, G, B, A, format,  a_drawingLimit);
	}
	
	/**
	* \brief	create a texture from a multiline text in utf16
	* \fn		virtual bool	CreateFromBuffer(...)=0;
	*/
	virtual bool	CreateFromText(const unsigned short* text, unsigned int _maxLineNumber = 0, unsigned int maxSize = 128, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1, float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888, int a_drawingLimit = -1)
	{
		assert(0);
		return false;
	}

	virtual  CMSP	getSharedInstance() override;

	/**
	* \brief	load the texture
	* \fn 		virtual bool	Load()=0;
	*/
	virtual bool	Load()=0;

	virtual void	GetPixels(unsigned int* _array){};
	virtual void	SetPixels(unsigned int* _array, int Width, int Height){};

	//! type of texture
	enum TextureType
	{
		TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_CUBE_MAP
	};
	friend class TextureFileManager;

	unsigned int	GetSelfDrawingNeeds() override
	{
		return ((unsigned int)Need_Predraw)|((unsigned int)Need_Postdraw);
	}

	inline void GetSize(unsigned int& width, unsigned int& height) { width = mWidth; height = mHeight; }
	inline void GetSize(float& width, float& height) { width = (float)mWidth; height = (float)mHeight; }
	inline v2f GetSize() { return { (float)mWidth, (float)mHeight }; }
	inline void GetPow2Size(unsigned int& width, unsigned int& height) {width=mPow2Width; height=mPow2Height;}
	inline void GetRatio(kfloat& rX, kfloat& rY) {rX=mRatioX; rY=mRatioY;}
	inline int GetTransparency() {return mTransparencyType;}

	bool isText()
	{
		return mIsText;
	}

	virtual void	InitForFBO();

protected:
	/**
	* \brief	initialize modifiable
	* \fn		virtual	void	InitModifiable();
	*/
	void	InitModifiable() override;
	void	UninitModifiable() override
	{
		UnsetFlag(isDirtyContext);
		Drawable::UninitModifiable();
	}

	inline void ComputeRatio()
	{
		mRatioX = (kfloat)((kfloat)mWidth / (kfloat)mPow2Width);
		mRatioY = (kfloat)((kfloat)mHeight / (kfloat)mPow2Height);
	}

	kfloat mRatioX;
	kfloat mRatioY;

	
	//! height of the texture
	maUInt		mWidth;
	//! width of the texture
	maUInt		mHeight;
	//! 0 = no transparency, 1 = billboard (binary transparency), 2 = need blend, -1 = to test on file loading
	maInt		mTransparencyType;
	//! TRUE if the texture has mipmap
	maBool		mHasMipmap;
	maBool		mForceNearest;
	//! file name of the used texture
	maString	mFileName;
	//! type of the used texture
	maInt		mTextureType;
	//! TRUE if I repeat the texture on the u axis
	maBool		mRepeat_U;
	//! TRUE if I repeat the texture on the v axis
	maBool		mRepeat_V;
	//! TRUE if we want the texture be directly accessed from memory (platform dependant)
	maBool		mIsDynamic;
	//! TRUE if we want the texture to be resized to a power of two at loading (done on the specific part !)
	maBool		mForcePow2;

	unsigned int mPow2Width;
	unsigned int mPow2Height;
	unsigned int mPixelSize;


	bool			mIsText;

	/**
	* \brief	destructor
	* \fn 		~Texture();
	*/
	virtual ~Texture();
};

#endif //_TEXTURE_H_
