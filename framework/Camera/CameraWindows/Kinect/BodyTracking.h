#ifndef _BODYTRACKING_H_
#define _BODYTRACKING_H_

#include "CoreModifiable.h"
#include <Kinect.h>
#include "TecLibs\Tec3D.h"

class UIImage;

//******** Mode d'emploi ***************//
/*

//à mettre dans l'initialisation (faire une variable privée BodyTracking* myBodyTracking;
myBodyTracking = (BodyTracking*)(Core::GetInstanceOf("bodytracking", _S_2_ID("BodyTracking")));
myBodyTracking->InitKinect();

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
*/
class BodyTracking : public CoreModifiable
{
public:
   
	DECLARE_CLASS_INFO(BodyTracking, CoreModifiable, KinectProjectApp)
	//! constructor
	BodyTracking(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	void					InitKinect();
	void					DetectBodies(int windowWidth, int windowHeight);

	BYTE*					getStream();
	BYTE*					getDepthStream();

	UIImage*				bodyCircles [6][25];
	UIImage*				os[6][24];
	Point2D					jointsCoords [6][25];
protected:
	//! destructor
	virtual ~BodyTracking();
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;
	RGBQUAD*                m_pColorRGBX;
	RGBQUAD*                m_pDepthRGBX;
	IMultiSourceFrameReader*m_pMultiSourceFrameReader;
	IMultiSourceFrame*		m_pMultiSourceFrame;

};

#endif //_BODYTRACKING_H_