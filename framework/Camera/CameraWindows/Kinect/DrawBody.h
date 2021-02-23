#ifndef _DrawBody_H_
#define _DrawBody_H_

#include "CoreModifiable.h"
#include <Kinect.h>
#include "TecLibs\Tec3D.h"

class UIImage;

//******** Mode d'emploi ***************//
/*

//à mettre dans l'initialisation (faire une variable privée BodyTracking* myBodyTracking;)
myBodyTracking = (BodyTracking*)(Core::GetInstanceOf("bodytracking", _S_2_ID("BodyTracking")));
myBodyTracking->InitKinect();

//et une variable DrawBody* myDrawBody;
myDrawBody = (DrawBody*)(Core::GetInstanceOf("DrawBody", _S_2_ID("DrawBody")));
myDrawBody->InitDrawBody("circle.png", "os.png");
for (int i = 0; i < 6; i++)
{
for (int j = 0; j < 25; j++)
{
Interface->addItem((CoreModifiable*)myDrawBody->bodyCircles[i][j]);
myDrawBody->bodyCircles[i][j]->Init();
}
}
for (int i = 0; i < 6; i++)
{
for (int j = 0; j < 24; j++)
{
Interface->addItem((CoreModifiable*)myDrawBody->os[i][j]);
myDrawBody->os[i][j]->Init();
}
}

//à mettre dans un update
//pour avoir l'image normale :
BYTE* bufferFlux = myBodyTracking->getStream();
if (bufferFlux){
my2DLayer->updateLayerFromBuffer(bufferFlux, 1920, 1980, "camera", TinyImage::RGBA_32_8888);
}
//pour l'image depth :
BYTE* bufferFlux = myBodyTracking->getDepthStream();
if (bufferFlux){
my2DLayer->updateLayerFromBuffer(bufferFlux, 512, 424, "camera", TinyImage::RGBA_32_8888);
}

//pour la détection corps
myBodyTracking->DetectBodies(window_width, window_height);
myDrawBody->DrawBodies(myBodyTracking->jointsCoords);
*/
class DrawBody : public CoreModifiable
{
public:

	DECLARE_CLASS_INFO(DrawBody, CoreModifiable, KinectProjectApp)
	//! constructor
	DrawBody(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	/**
	* \brief	à appeler dans l'init
	*/
	void					InitDrawBody(kstl::string jointFileName, kstl::string boneFileName);

	/**
	* \brief	à appeler dans un update
	* \param	jointsCoords : à récupérer avec DetectBodies de BodyTracking
	*/
	void					DrawBodies(Point2D jointsCoords[][25]);

	UIImage*				bodyCircles[6][25];
	UIImage*				os[6][24];
protected:
	//! destructor
	virtual ~DrawBody();
	
	void					relier(Point2D ptA, Point2D ptB, int index, int indexBody);
};

#endif //_DrawBody_H_