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
// * Base2DLayer class
// * --------------------------------------
/**
* \file	Base2DLayer.h
* \class	Base2DLayer
* \ingroup 2DLayers
* \brief	base class for 2DLayer (background roto zoom screen, tiles ...)
* \author	ukn
* \version ukn
* \date	ukn
*
* Base2DLayer inherits Scene3D so that it can be added in the scenegraph
*
* <dl class="dependency"><dt><b>Dependency:</b></dt><dd>Module2DLayer</dd></dl>
*
* <dl class="exported"><dt><b>Exported parameters :</b></dt><dd>
* <table>
* <tr><td>Enum</td>	<td><strong>Type</strong> :</td>				<td>type of the layer ("Character","Bitmap")</td></tr>
* <tr><td>Int</td>		<td><strong>SizeX</strong> :</td>				<td>size on X axis</td></tr>
* <tr><td>Int</td>		<td><strong>SizeY</strong> :</td>				<td>size on Y axis</td></tr>
* <tr><td>Enum</td>	<td><strong>ColorMode</strong> :</td>			<td>type of the layer color mode ("16","256","256x16","DC")</td></tr>
* <tr><td>reference</td>	<td><strong>RenderingScreen</strong> :</td>	<td>linked renderingScreen (for init)</td></tr>
* <tr><td>string</td>	<td><strong>VirtualScreenName</strong> :</td>	<td>name of the linked VirtualTileScreen (for init)</td></tr>
* <tr><td>Bool</td>	<td><strong>RepeatMode</strong> :</td>			<td>repeat mode</td></tr>
* <tr><td>string</td>	<td><strong>FileName</strong> :</td>			<td>File name used by loading method</td></tr>
* <tr><td>Int</td>		<td><strong>ScrollX</strong> :</td>				<td>Scroll position on X axis of the layer on the virtual screen</td></tr>
* <tr><td>Int</td>		<td><strong>ScrollY</strong> :</td>				<td>Scroll position on Y axis of the layer on the virtual screen</td></tr>
* </table>
* </dd></dl>
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

	RenderingScreen*	GetRenderingScreen() { return (RenderingScreen*)(CoreModifiable*)myRenderingScreen; }

	void	getSize(int& sx, int& sy)
	{
		sx = mySizeX.const_ref();
		sy = mySizeY.const_ref();
	}

	//void SetMouseInfo(LayerMouseInfo* mi) { theMouseInfo = mi; }


protected:
	void InitModifiable() override;

	//! set rendering screen active
	bool	StartDrawing(TravState* state);
	//! close rendering screen
	void	EndDrawing(TravState* state);

	//! pointer to the renderingSreen
	maReference myRenderingScreen;
	maReference myManager;

	//! size on X axis
	maInt    mySizeX;
	//! size on Y axis
	maInt    mySizeY;

	maBool		myClearZBuffer = BASE_ATTRIBUTE(ClearZBuffer, false);
	maBool		myClearColorBuffer = BASE_ATTRIBUTE(ClearColorBuffer, false);
	maVect4DF	myClearColor = BASE_ATTRIBUTE(ClearColor, 0.0f, 0.0f, 0.0f, 1.0f);

	// if not interactive, don't do update (but still draw)
	maBool	myIsInteractive;

	//LayerMouseInfo * theMouseInfo = nullptr;
};
#endif //ABSTRACT2DLAYER_H_
