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
* \ingroup Drawable
* \ingroup RendererDrawable
* \brief	Texture object
* \author	ukn
* \version ukn
* \date	ukn
*
* Exported parameters :<br>
* <ul>
* <li>
*		kfloat <strong>Width</strong> : width of the texture
* </li>
* <li>
*		kfloat <strong>Height</strong> : height of the texture
* </li>
* <li>
*		int <strong>TransparencyType</strong> : 0 = no transparency, 1 = billboard (binary transparency), 2 = need blend, -1 = to test on file loading
* </li>
* <li>
*		bool <strong>HasMipmap</strong> :  TRUE if the texture has mipmap
* </li>
* <li>
*		string <strong>FileName</strong> : file name of the used texture
* </li>
* <li>
*		int <strong>TextureType</strong> : type of the used texture
* </li>
* <li>
*		bool <strong>Repeat_U</strong> : TRUE if I repeat the texture on the u axis
* </li>
* <li>
*		bool <strong>Repeat_V</strong> : TRUE if I repeat the texture on the v axis
* </li>
* </ul>
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
	void SetRepeatUV (bool RU, bool RV) {myRepeatU = RU; myRepeatV=RV;}

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
	bool	CreateFromText(const char* text, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1, float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888, bool _bold = false, bool _stroke = false, float st_R = 0, float st_G = 0, float st_B = 0, float st_A = 1, int a_drawingLimit = -1)
	{
		usString tmpText(text);
		return CreateFromText(tmpText.us_str(), fontSize, fontName, a_Alignment, R, G, B, A, format, _bold, _stroke, st_R, st_G, st_B, st_A, a_drawingLimit);
	}

	/**
	* \brief	create a texture from a text in utf16
	* \fn		virtual bool	CreateFromBuffer(...)=0;
	*/
	virtual bool	CreateFromText(const unsigned short* text, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1, float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888, bool _bold = false, bool _stroke = false, float st_R = 0, float st_G = 0, float st_B = 0, float st_A = 1, int a_drawingLimit = -1)
	{
		assert(0);
		return false;
	}

	/**
	* \brief	create a texture from a multiline text
	* \fn		virtual bool	CreateFromText(...)=0;
	*/

	bool	CreateFromText(const char* text, unsigned int _maxLineNumber = 0, unsigned int maxSize = 128, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1,  float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888, bool _bold = false, bool _stroke = false, float st_R = 0, float st_G = 0, float st_B = 0, float st_A = 1, int a_drawingLimit = -1)
	{
		usString tmpText(text);
		return  CreateFromText(tmpText.us_str(), _maxLineNumber, maxSize, fontSize, fontName, a_Alignment, R, G, B, A, format, _bold, _stroke, st_R, st_G, st_B, st_A, a_drawingLimit);
	}
	
	/**
	* \brief	create a texture from a multiline text in utf16
	* \fn		virtual bool	CreateFromBuffer(...)=0;
	*/
	virtual bool	CreateFromText(const unsigned short* text, unsigned int _maxLineNumber = 0, unsigned int maxSize = 128, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1, float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888, bool _bold = false, bool _stroke = false, float st_R = 0, float st_G = 0, float st_B = 0, float st_A = 1, int a_drawingLimit = -1)
	{
		assert(0);
		return false;
	}

	virtual CoreModifiable*	getSharedInstance() override;

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

	inline void GetSize(unsigned int& width, unsigned int& height) { width = myWidth; height = myHeight; }
	inline void GetSize(float& width, float& height) { width = (float)myWidth; height = (float)myHeight; }
	inline v2f GetSize() { return { (float)myWidth, (float)myHeight }; }
	inline void GetPow2Size(unsigned int& width, unsigned int& height) {width=myPow2Width; height=myPow2Height;}
	inline void GetRatio(kfloat& rX, kfloat& rY) {rX=myRatioX; rY=myRatioY;}
	inline int GetTransparency() {return myTransparencyType;}

	bool isText()
	{
		return myIsText;
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
		myRatioX = (kfloat)((kfloat)myWidth / (kfloat)myPow2Width);
		myRatioY = (kfloat)((kfloat)myHeight / (kfloat)myPow2Height);
	}

	kfloat myRatioX;
	kfloat myRatioY;

	
	//! height of the texture
	maUInt		myWidth;
	//! width of the texture
	maUInt		myHeight;
	//! 0 = no transparency, 1 = billboard (binary transparency), 2 = need blend, -1 = to test on file loading
	maInt		myTransparencyType;
	//! TRUE if the texture has mipmap
	maBool		myHasMipmap;
	maBool		myForceNearest;
	//! file name of the used texture
	maString	myFileName;
	//! type of the used texture
	maInt		myTextureType;
	//! TRUE if I repeat the texture on the u axis
	maBool		myRepeatU;
	//! TRUE if I repeat the texture on the v axis
	maBool		myRepeatV;
	//! TRUE if we want the texture be directly accessed from memory (platform dependant)
	maBool		myIsDynamic;
	//! TRUE if we want the texture to be resized to a power of two at loading (done on the specific part !)
	maBool		myForcePow2;

	unsigned int myPow2Width;
	unsigned int myPow2Height;
	unsigned int myPixelSize;


	bool			myIsText;

	/**
	* \brief	destructor
	* \fn 		~Texture();
	*/
	virtual ~Texture();
};

#endif //_TEXTURE_H_
