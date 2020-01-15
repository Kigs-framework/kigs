#ifndef _GLSLDeferred2_H
#define _GLSLDeferred2_H

#include "GLSLDeferredFilter.h"
#include "Drawable.h"
#include "Node3D.h"

class Camera;
class RendererOpenGL;
class API3DShader;
struct FilterSorter;

class API3DDeferred : public Node3D
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DDeferred, Node3D, Renderer)

	API3DDeferred(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	int getColorChannel() { return myFboTexture[0]; }
	int getNormalChannel() { return myFboTexture[1]; }
	int getPositionChannel() { return myFboTexture[2]; }
	int getSpecularChannel() { return myFboTexture[3]; }
	int getAmbiantChannel() { return myFboTexture[4]; }
	int getFinalChannel() { return myFinalRenderTexture; }

	void RenderQuad();

protected:
	virtual ~API3DDeferred();
	void	InitModifiable() override;
	void	TravDraw(TravState* state) override;

	bool	addItem(CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
	bool	removeItem(CMSP& item DECLARE_DEFAULT_LINK_NAME)override;

	virtual void	Dealloc();


	void CreateBuffers(RendererOpenGL* renderer, int sizeX, int sizeY);
	void CreateFramebuffer(RendererOpenGL* renderer);

	void PreDrawTreatment(TravState* state);
	void PreGeometryTreatment(TravState* state);
	void PostGeometryTreatment(TravState* state);
	void PreLightTreatment(TravState* state);
	void PostLightTreatment(TravState* state);
	void PostDrawTreatment(TravState* state);


	int						mySize[2];
	short					myCoords[8];
	unsigned int			myDepthTexture;
	unsigned int			myFboTexture[5];
	unsigned int			myFboRenderer;

	unsigned int			myFboLightTexture;
	unsigned int			myFboLight;

	unsigned int			myFinalRenderTexture;

	API3DShader *		myRenderShader;

	kstl::set<API3DDeferredFilter*, API3DDeferredFilter::PriorityCompare> myFilters;
};


struct FilterSorter
{
public:
	bool operator() (API3DDeferredFilter& lhs, API3DDeferredFilter& rhs) const;
};

#endif //_GLSLDeferred2_H
