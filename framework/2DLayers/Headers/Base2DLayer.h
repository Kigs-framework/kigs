#ifndef BASE2DLAYER_H_
#define BASE2DLAYER_H_

#include "Abstract2DLayer.h"

#include "PrecompiledHeaders.h"
#include "GlobalEnum.h"
#include "Core.h"
#include "TinyImage.h"
#include "maReference.h"
#include <deque>

class RenderingScreen;
class Texture;
class BaseTileBuffer;
class BaseTilesBank;
class VirtualTileScreen;
class BaseSprite;
class SpriteManager;
class UIItem;
class ModuleInput;
class MouseVelocityComputer;
class MultiTouchPinch;
class TravState;


class Base2DLayer : public Abstract2DLayer
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Base2DLayer,Abstract2DLayer,2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	Base2DLayer(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	//! no camera for 2D Layer, throw an error
	void  addCamera(CoreModifiable* /*camera*/) override { KIGS_ERROR("Trying to add a camera on a 2D Layer",1); }
	//! no camera for 2D Layer, throw an error
	bool  removeCamera(CoreModifiable* /*camera*/) override { KIGS_ERROR("Trying to remove a camera from a 2D Layer",1); return false; }

	/**
	 * \brief	update the texture from a buffer (used with camera)
	 * \param	buffer : pixel color buffer
	 */
	virtual void updateLayerFromBuffer(unsigned char* buffer, unsigned int width, unsigned int height, kstl::string name,TinyImage::ImageFormat format=TinyImage::ABGR_16_1555_DIRECT_COLOR)=0;

	/**
	 * \brief	call directly CoreModifiable addItem (not Scene3D or Node3D)
	 * \param	item : item to add
	 * \param	linkName : name of the added item
	 * \return	TRUE if the item is added, FALSE otherwise
	 */
	bool	addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;

	/**
	 * \fn 		virtual bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME);
	 * \brief	call directly CoreModifiable addItem (not Scene3D or Node3D)
	 * \param	item : item to remove
	 * \param	linkName : name of the removed item
	 * \return	TRUE if the item is removed, FALSE otherwise
	 */
	bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME) override;

	/**
	 * \brief	draw the scene3D
	 * \fn 		virtual void            TravDraw(TravState* state)=0;
	 * \param	state : the current TravState
	 * \return	TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
	 */
	void TravDraw(TravState* state) override =0;

	/**
	 * \brief	call cull process for this node and all sons at visible positions
	 * \fn 		virtual void TravCull(TravState* state);
	 * \param	state : travstate
	 */
	void TravCull(TravState* state) override =0;

	bool ChangeTexture(kstl::string TextureName);

	void GetSize(kfloat &X, kfloat &Y);

	/**
	 * \brief	get the tile bank
	 * \fn 		BaseTilesBank*	GetTileBank()
	 * \return	the tile bank
	 */
	BaseTilesBank*	GetTileBank() { return myTilesBank; }

	/**
	 * \brief	get the virtual screen
	 * \fn 		VirtualTileScreen*	GetVirtualScreen()
	 * \return	the frame buffer
	 */
	VirtualTileScreen*	GetVirtualScreen();
	
	void UpdateLayer();
//	using Abstract2DLayer::Update;

	/**
	 * \brief	get the tile buffer
	 * \fn 		BaseTileBuffer* GetTileBuffer()
	 * \return	the tile buffer
	 */
	BaseTileBuffer* GetTileBuffer() { return myTileBuffer; }

	// *******************************************************
	// Tiles layer method
	// *******************************************************
	/**
	 * \brief	load the tile data
	 * \fn 		virtual	void	LoadData()=0;
	 */
	void	LoadData();

	/**
	 * \brief	set the tile info
	 * \fn 		void	SetTileInfo(unsigned short info,unsigned int posX,unsigned int posY);
	 * \param	info : index of the info
	 * \param	posX : pos on X axis of the tile
	 * \param	posY : pos on Y axis of the tile
	 */
	void	SetTileInfo(unsigned short info,unsigned int posX,unsigned int posY);

	/**
	 * \brief	set the scrolling values of the layer (used by virtual screen)
	 * \fn 		void setScroll(int scrollX, int scrollY)
	 * \param	scrollX : scrolling on X axis
	 * \param	scrollY : scrolling on Y axis
	 */
	void setScroll(int scrollX, int scrollY);

	void setScreenPos(int scrX, int scrY);

	void GetPositionOnScreen(kfloat X, kfloat Y, kfloat &myPosX, kfloat &myPosY);
	void GetPositionOnLayer(kfloat X, kfloat Y, kfloat &myPosX, kfloat &myPosY);

	// *******************************************************
	// Direct draw method for bitmap layer only
	// *******************************************************
	virtual void ClearDirectDrawList();

	virtual void AddDirectDraw(DirectDrawStruct* toAdd)	
	{
		m_DirectDrawList.push_back(toAdd);
	}

	virtual void RemoveDirectDraw(DirectDrawStruct* toRemove);

	virtual void DrawRectangle(kfloat X, kfloat Y, int sizeX, int sizeY, kfloat r, kfloat g, kfloat b)=0;
	
	virtual bool UsePalette(){return false;}

protected:
	/**
	 * \brief	destructor
	 * \fn 		~Base2DLayer();
	 */
	virtual ~Base2DLayer();

	/**
	 * \fn 		void InitModifiable() override
	 * \brief	init the modifiable and set the isInit flag to true if OK
	 */
	void	InitModifiable() override;

	//! type of the layer ("Character","Bitmap" or "Sprite")
	maEnum<3>			myType;
	maBool				myUsePalette;
	//! type of the layer color mode ("16","256","256x16","DC")
	maEnum<4>			myColorMode;
	//! name of the linked virtualScreen
	maString			myVirtualScreenName;
	//! repeat mode
	maBool				myRepeatX;
	maBool				myRepeatY;

	//! linked sprite manager if any
	SpriteManager*		mySpriteManager;
	
	//! pointer to the texture (from a bitmap)
	Texture*			myBitmapTexture;

	//! file name (texture file)
	maString			myFileName;

	//! pointer to the tile bank
	BaseTilesBank*		myTilesBank;

	//! pointer to the virtual screen
	VirtualTileScreen*	myVirtualScreen;

	//! tile buffer
	BaseTileBuffer*		myTileBuffer;

	//! Scroll on X axis of the screen
	int					myScrollX;
	//! Scroll on Y axis of the screen
	int					myScrollY;

	// position on X axis of the screen on the layer
	int					myLayerOffsetX;
	// position on Y axis of the screen on the layer
	int					myLayerOffsetY;

	// position on X axis of the screen on the layer
	int					myScreenOffsetX;
	// position on Y axis of the screen on the layer
	int					myScreenOffsetY;

	maInt				myScreenPosX;
	maInt				myScreenPosY;
	int					myOldScreenPosX;
	int					myOldScreenPosY;

	bool				bShouldSetOffset;
	bool				bShouldUploadData;

	bool mySecondTouch;
	
	kstl::vector<DirectDrawStruct*> m_DirectDrawList;
};
#endif //BASE2DLAYER_H_
