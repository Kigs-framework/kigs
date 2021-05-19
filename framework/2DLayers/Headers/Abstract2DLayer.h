#ifndef ABSTRACT2DLAYER_H_
#define ABSTRACT2DLAYER_H_

#include "Scene3D.h"
#include "maReference.h"

struct LayerMouseInfo;
class RenderingScreen;

class DirectDrawStruct
{
public:
	virtual ~DirectDrawStruct() {}

	virtual void Draw(TravState* state) = 0;

protected:
	DirectDrawStruct() {}
};

// ****************************************
// * Abstract2DLayer class
// * --------------------------------------
/**
* \file	Abstract2DLayer.h
* \class	Abstract2DLayer
* \ingroup 2DLayers
* \brief	Base class for 2D Layers.
* 
*/
// ****************************************
class Abstract2DLayer : public Scene3D
{
public:
public:
	DECLARE_ABSTRACT_CLASS_INFO(Abstract2DLayer, Scene3D, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	Abstract2DLayer(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	RenderingScreen*	GetRenderingScreen() { return (RenderingScreen*)(CoreModifiable*)mRenderingScreen; }

	void	getSize(int& sx, int& sy)
	{
		sx = mSize[0];
		sy = mSize[1];
	}

protected:
	void InitModifiable() override;

	//! set rendering screen active
	bool	StartDrawing(TravState* state);
	//! close rendering screen
	void	EndDrawing(TravState* state);

	//! pointer to the renderingSreen
	maReference mRenderingScreen;
	maReference mManager;

	//! size
	maVect2DF    mSize;
	
	maBool		mClearZBuffer = BASE_ATTRIBUTE(ClearZBuffer, false);
	maBool		mClearColorBuffer = BASE_ATTRIBUTE(ClearColorBuffer, false);
	// if not interactive, don't do update (but still draw)
	maBool		mIsInteractive;
	maVect4DF	mClearColor = BASE_ATTRIBUTE(ClearColor, 0.0f, 0.0f, 0.0f, 1.0f);



};
#endif //ABSTRACT2DLAYER_H_
