#ifndef _GLSLPOSTPROCESS_H
#define _GLSLPOSTPROCESS_H

#include "Drawable.h"

// apply a off screen rendering at predraw, and use it at post draw to achieve the postprocess effect

class API3DPostProcess : public Drawable
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DPostProcess,Drawable,Renderer)

	API3DPostProcess(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	virtual void	NotifyUpdate(const unsigned int labelid );
	//inline ShaderInfo*	Get_ShaderProgram() const {return & mShaderProgram;}
protected:
	
	void			Active(TravState*);
	void			Deactive(TravState*);
	
	virtual			~API3DPostProcess();

	void			Dealloc();
	void			Rebuild();

	virtual bool	PreDraw(TravState*);
	virtual bool	PostDraw(TravState* travstate);
	void			InitModifiable();

	unsigned int	mVertexShader;
	unsigned int	mFragmentShader;
	ShaderInfo		mShaderProgram;

	maString		myVertexShaderText;
	maString		myFragmentShaderText;

	unsigned int	myFboRenderer, myFboTexture, myDepthTexture;
};
#endif //_GLSLPOSTPROCESS_H
