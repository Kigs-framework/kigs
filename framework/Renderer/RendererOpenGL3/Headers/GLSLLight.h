#ifndef _GLSLLIGHT_H
#define _GLSLLIGHT_H


#include "Light.h"
#include "Scene3D.h"
//#define USE_ATTFUNCTION


class CoreModifiable;
class Camera;
class RendererOpenGL;

class API3DLight : public Light
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DLight, Light, Renderer)

	API3DLight(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual bool PreRendering(RendererOpenGL * renderer, Camera * cam, Point3D & camPos);

	bool	Draw(TravState* state) override;

	virtual void	DrawLight(TravState*);
	void PostDrawLight(TravState*);

	void SetUniformLocation(int uniform, const char* location);

	int GetTypeOfLight();
	//inline int		GetPriority() const { return (int)((unsigned int)myPriority); }
	void NotifyUpdate(const unsigned int  labelid) override;

protected:
	virtual ~API3DLight();
	void	InitModifiable() override;

	CMSP		myPositionUniform;
	CMSP		myCamPosUniform;

	CMSP		myDiffuseUniform;
	CMSP		mySpecularUniform;
	CMSP		myAmbiantUniform;

	CMSP		myAttenuationUniform;
	CMSP		mySpotDirUniform;
	CMSP		mySpotCutoffUniform;
	CMSP		mySpotExponentUniform;
	
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


#endif //_GLSLLIGHT_H
