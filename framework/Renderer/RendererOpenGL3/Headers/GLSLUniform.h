#ifndef _GLSLUNIFORM_H
#define _GLSLUNIFORM_H

#include "CoreModifiable.h"
#include "Drawable.h"
#include "maBuffer.h"
#include "AttributePacking.h"
#include "Texture.h"

#include "Shader.h"

#define KEY_TYPE unsigned int


class RendererOpenGL;


// ****************************************
// * API3DUniformBase class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformBase
 * \ingroup Renderer
 * \brief	Base class for GLSL uniforms.
 */
 // ****************************************

class API3DUniformBase : public Drawable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(API3DUniformBase, Drawable, Renderer);
	API3DUniformBase(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void	NotifyUpdate(const unsigned int  labelid) override;

	virtual void	Activate(unsigned int a_Location) = 0;
	virtual bool	Deactivate(unsigned int a_Location) { return false; }

	virtual bool	Push(TravState*);
	virtual bool	Pop(TravState*);

	UNIFORM_NAME_TYPE   Get_ID() { return mID; }
	kstl::string		Get_Name() { return mUniName.const_ref(); }

protected:
	void	InitModifiable() override;
	bool	PreDraw(TravState*) override;
	bool	PostDraw(TravState*) override;


	maString			mUniName;
	UNIFORM_NAME_TYPE	mID;
};

// ****************************************
// * API3DUniformInt class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformInt
 * \ingroup Renderer
 * \brief	One int uniform.
 *
 * Set mValue in predraw and -1 in post draw
 */
 // ****************************************
class API3DUniformInt : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformInt, API3DUniformBase, Renderer);
	API3DUniformInt(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;
	void SetValue(int aV) { mValue = aV; }

protected:
	void	NotifyUpdate(const unsigned int  labelid) override;

	maInt			mValue;
};

// ****************************************
// * API3DUniformFloat class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformFloat
 * \ingroup Renderer
 * \brief	One float uniform.
 *
 * Set mValue in predraw and -1 in post draw
 */
 // ****************************************
class API3DUniformFloat : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformFloat, API3DUniformBase, Renderer);
	API3DUniformFloat(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;
	void SetValue(float aV) { mValue = aV; }

protected:
	void	NotifyUpdate(const unsigned int  labelid) override;

	maFloat			mValue;
};

// ****************************************
// * API3DUniformFloat2 class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformFloat2
 * \ingroup Renderer
 * \brief	Two float uniform.
 *
 * Set mValue in predraw and -1 in post draw
 */
 // ****************************************
class API3DUniformFloat2 : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformFloat2, API3DUniformBase, Renderer);
	API3DUniformFloat2(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;

protected:
	void	NotifyUpdate(const unsigned int  labelid) override;

	maVect2DF			mValue;
};

// ****************************************
// * API3DUniformFloat3 class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformFloat3
 * \ingroup Renderer
 * \brief	Three float uniform.
 *
 * Set mValue in predraw and -1 in post draw
 */
 // ****************************************
class API3DUniformFloat3 : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformFloat3, API3DUniformBase, Renderer);
	API3DUniformFloat3(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;

	void WrappedSetValue(float aX, float aY, float aZ) { mValue[0] = aX; mValue[1] = aY;	mValue[2] = aZ; }
	void SetValue(float aX, float aY, float aZ) { mValue[0] = aX; mValue[1] = aY;	mValue[2] = aZ; }
	void SetValue(float *aX) { mValue[0] = aX[0]; mValue[1] = aX[1];	mValue[2] = aX[2]; }

	WRAP_METHODS(WrappedSetValue);

protected:
	void InitModifiable() override;
	void NotifyUpdate(const unsigned int  labelid) override;

	void Normalize();

	maVect3DF			mValue;
	maBool				mNormalize;
};


// ****************************************
// * API3DUniformFloat4 class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformFloat4
 * \ingroup Renderer
 * \brief	Four float uniform.
 *
 * Set mValue in predraw and -1 in post draw
 */
 // ****************************************
class API3DUniformFloat4 : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformFloat4, API3DUniformBase, Renderer);
	API3DUniformFloat4(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;

protected:
	void NotifyUpdate(const unsigned int  labelid) override;

	maVect4DF			mValue;
};

// ****************************************
// * API3DUniformTexture class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformTexture
 * \ingroup Renderer
 * \brief	Texture passed as a uniform.
 *
 */
 // ****************************************
class API3DUniformTexture : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformTexture, API3DUniformBase, Renderer)
	API3DUniformTexture(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~API3DUniformTexture();

	bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

protected:
	void InitModifiable() override;
	void NotifyUpdate(const unsigned int  labelid) override;

	void Activate(unsigned int a_Location) override;
	bool Deactivate(unsigned int a_Location) override;

	maInt		mChannel;
	maString	mTextureName;

	SP<Texture>	mAttachedTexture = nullptr;
};

// ****************************************
// * API3DUniformDataTexture class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformDataTexture
 * \ingroup Renderer
 * \brief	Data texture (dynamic) passed as a uniform.
 *
 */
 // ****************************************
class API3DUniformDataTexture : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformDataTexture, API3DUniformBase, Renderer)
	API3DUniformDataTexture(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	~API3DUniformDataTexture() override;

protected:
	void InitModifiable() override;
	void NotifyUpdate(const unsigned int  labelid) override;

	void Activate(unsigned int a_Location) override;
	bool Deactivate(unsigned int a_Location) override;

	maInt			mChannel;
	maString		mTextureName;

	unsigned int	mTextureGLIndex;
};

// ****************************************
// * API3DUniformGeneratedTexture class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformGeneratedTexture
 * \ingroup Renderer
 * \brief	Generated Noise Data texture passed as a uniform.
 *
 */
 // ****************************************
class API3DUniformGeneratedTexture : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformGeneratedTexture, API3DUniformBase, Renderer)
	API3DUniformGeneratedTexture(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~API3DUniformGeneratedTexture();

protected:
	void NotifyUpdate(const unsigned int  labelid) override;
	void Generate();

	void Activate(unsigned int a_Location) override;
	bool Deactivate(unsigned int a_Location) override;

	maInt			mChannel;
	maVect3DF		mSize;
	maFloat			mScale;
	maFloat			mPersistence;
	maInt			mOctaveCount;

	unsigned int	mTextureGLIndex;
};

// ****************************************
// * API3DUniformMatrixArray class
// * --------------------------------------
/**
 * \file	GLSLUniform.h
 * \class	API3DUniformMatrixArray
 * \ingroup Renderer
 * \brief	Manage an array of matrix as sharder uniform.
 *
 */
 // ****************************************
class API3DUniformMatrixArray : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformMatrixArray, API3DUniformBase, Renderer)
	API3DUniformMatrixArray(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~API3DUniformMatrixArray();

protected:
	void NotifyUpdate(const unsigned int  labelid) override;
	void Activate(unsigned int a_Location) override;

	maInt			mArraySize;
	maBuffer		mMatrixArray;
	Matrix4x4*		mMatrixArrayPointer;
};
#endif //_GLSLUNIFORM_H
