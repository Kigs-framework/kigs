#ifndef _HLSLLIGHT_H
#define _HLSLLIGHT_H

#include "Drawable.h"
#include "Light.h"
#include "Scene3D.h"
//#define USE_ATTFUNCTION

class Node3D;
class CoreModifiable;
class Camera;
class RendererDX11;
struct LightStruct;

// ****************************************
// * API3DLight class
// * --------------------------------------
/**
 * \file	HLSLLight.h
 * \class	API3DLight
 * \ingroup Renderer
 * \brief	DX11 Light.
 */
 // ****************************************

class API3DLight : public Light
{
public:
	friend class RendererDX11;

	DECLARE_CLASS_INFO(API3DLight, Light, Renderer)
	API3DLight(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~API3DLight();

	virtual bool PreRendering(RendererDX11 * renderer, Camera * cam, Point3D & camPos);

	void PrepareLightInfo(LightStruct& light_data, Camera* cam);

	bool	Draw(TravState* state) override;

	virtual void	DrawLight(TravState*);
	void PostDrawLight(TravState*);

	void SetUniformLocation(int uniform, const char* location);

	int GetTypeOfLight();
	void NotifyUpdate(const unsigned int  labelid) override;

protected:
	void	InitModifiable() override;
	
};

#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2
#define CUSTOM_LIGHT 3


#define DIFFUSE_COLOR 0
#define SPECULAR_COLOR 1
#define POSITION_LIGHT 2
#define ATTENUATION 3
#define DIRECTION 4
#define SPOT_CUT_OFF 5
#define SPOT_EXPONENT 6
#define SPOT_DIRECTION 7
#define AMBIANT_COLOR 8

#endif //_HLSLLIGHT_H
