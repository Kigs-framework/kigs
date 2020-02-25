#ifndef _HLSLLIGHT_H
#define _HLSLLIGHT_H

#include "Drawable.h"
#include "HLSLShader.h"
#include "Scene3D.h"
//#define USE_ATTFUNCTION

class Node3D;
class CoreModifiable;
class Camera;
class RendererDX11;

class API3DLight : public API3DShader
{
public:
	friend class RendererDX11;

	DECLARE_CLASS_INFO(API3DLight, API3DShader, Renderer)

	API3DLight(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual bool PreRendering(RendererDX11 * renderer, Camera * cam, Point3D & camPos);

	void PrepareLightInfo(LightStruct& light_data, Camera* cam);

	bool	Draw(TravState* state) override;


	Node3D* GetFather();
	bool	PreDraw(TravState*) override { return false; };
	bool	PostDraw(TravState*)  override { return false; };
	virtual void	DrawLight(TravState*);
	void PostDrawLight(TravState*);

	void SetUniformLocation(int uniform, const char* location);

	int GetTypeOfLight();
	inline int		GetPriority() const { return (int)((unsigned int)myPriority); }
	void NotifyUpdate(const unsigned int  labelid) override;

	inline void setIsOn(bool a_value) { myIsOn = a_value; }
	inline bool getIsOn() { return myIsOn; }

	inline void setIsDeffered(bool a_value) { myDefferedLight= a_value; }
	inline bool getIsDeffered() { return myDefferedLight; }

protected:
	virtual ~API3DLight();
	void	InitModifiable() override;
	
	void SetOffset(float X, float Y, float Z);
	void SetSpotDir(float X, float Y, float Z);


	//#define USE_ATTFUNCTION
#ifdef USE_ATTFUNCTION
	DECLARE_METHOD(SetAttenuation);
	DECLARE_METHOD(GetAttenuation);
	COREMODIFIABLE_METHODS(SetAttenuation, GetAttenuation);
#endif

	//! TRUE if the light is on
	maBool		myIsOn;

	//Param for uniforms
	maVect3DF  	myPosOffset;

	maVect3DF	myDiffuseColor;
	maVect3DF	myAmbiantColor;
	maVect3DF	mySpecularColor;

	maFloat		myConstAttenuation;
	maFloat		myLinAttenuation;
	maFloat		myQuadAttenuation;

	maVect3DF	mySpotDirection;
	maFloat		mySpotCutOff;
	maFloat		mySpotAttenuation;
	
	//! TRUE if the light is directional
	maBool		myIsDirectional;
	maBool		myDefferedLight;

	maUInt		myPriority;

	//0 for point, 1 for directional, 2 for spot, 3 for customShader
	maEnum<4>	myLightType;

	maReference	myPositionNode3D;

	Scene3D*			myScene;
	Node3D *			myParentNode;
	
	unsigned int		myListUpdateFrame;
};

struct LightPriorityCompare
{
	//! overload operator () for comparison
	bool operator()(const API3DLight * a1, const API3DLight * a2) const
	{
		if (a1->GetPriority() == a2->GetPriority())
			return (a1) < (a2);
		return a1->GetPriority() < a2->GetPriority();
	}
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
