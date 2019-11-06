#ifndef _GLSLUNIFORM_H
#define _GLSLUNIFORM_H

#include "CoreModifiable.h"
#include "Drawable.h"
#include "maBuffer.h"
#include "AttributePacking.h"

#include "Shader.h"

#define KEY_TYPE unsigned int

class Texture;
class RendererOpenGL;

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

	UNIFORM_NAME_TYPE   Get_ID() { return myID; }
	kstl::string		Get_Name() { return myUniName.const_ref(); }

protected:
	void	InitModifiable() override;
	bool	PreDraw(TravState*) override;
	bool	PostDraw(TravState*) override;


	maString			myUniName;
	UNIFORM_NAME_TYPE	myID;
};

// set myValue in predraw and -1 in post draw
class API3DUniformInt : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformInt, API3DUniformBase, Renderer);
	API3DUniformInt(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;
	void SetValue(int aV) { myValue = aV; }

protected:
	void	NotifyUpdate(const unsigned int  labelid) override;

	maInt			myValue;
};

// set myValue in predraw and -1 in post draw
class API3DUniformFloat : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformFloat, API3DUniformBase, Renderer);
	API3DUniformFloat(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;
	void SetValue(float aV) { myValue = aV; }

protected:
	void	NotifyUpdate(const unsigned int  labelid) override;

	maFloat			myValue;
};

// set myValue in predraw and -1 in post draw
class API3DUniformFloat2 : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformFloat2, API3DUniformBase, Renderer);
	API3DUniformFloat2(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;

protected:
	void	NotifyUpdate(const unsigned int  labelid) override;

	maVect2DF			myValue;
};

// set myValue in predraw and -1 in post draw
class API3DUniformFloat3 : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformFloat3, API3DUniformBase, Renderer);
	API3DUniformFloat3(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;

	void WrappedSetValue(float aX, float aY, float aZ) { myValue[0] = aX; myValue[1] = aY;	myValue[2] = aZ; }
	void SetValue(float aX, float aY, float aZ) { myValue[0] = aX; myValue[1] = aY;	myValue[2] = aZ; }
	void SetValue(float *aX) { myValue[0] = aX[0]; myValue[1] = aX[1];	myValue[2] = aX[2]; }

	WRAP_METHOD(WrappedSetValue);

protected:
	void InitModifiable() override;
	void NotifyUpdate(const unsigned int  labelid) override;

	void Normalize();

	maVect3DF			myValue;
	maBool				myNormalize;
};

// set myValue in predraw and -1 in post draw
class API3DUniformFloat4 : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformFloat4, API3DUniformBase, Renderer);
	API3DUniformFloat4(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Activate(unsigned int a_Location) override;

protected:
	void NotifyUpdate(const unsigned int  labelid) override;

	maVect4DF			myValue;
};

class API3DUniformTexture : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformTexture, API3DUniformBase, Renderer)
	API3DUniformTexture(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	bool	addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME) override;


protected:
	virtual ~API3DUniformTexture();
	void InitModifiable() override;
	void NotifyUpdate(const unsigned int  labelid) override;

	void Activate(unsigned int a_Location) override;
	bool Deactivate(unsigned int a_Location) override;

	maInt		myTextureChannel;
	maString	myTextureName;

	Texture*	myAttachedTexture;
};

class API3DUniformDataTexture : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformDataTexture, API3DUniformBase, Renderer)
	API3DUniformDataTexture(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	~API3DUniformDataTexture() override;
	void InitModifiable() override;
	void NotifyUpdate(const unsigned int  labelid) override;

	void Activate(unsigned int a_Location) override;
	bool Deactivate(unsigned int a_Location) override;

	maInt			myTextureChannel;
	maString		myTextureName;

	unsigned int	myTextureGLIndex;
};

class API3DUniformGeneratedTexture : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformGeneratedTexture, API3DUniformBase, Renderer)
	API3DUniformGeneratedTexture(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	virtual ~API3DUniformGeneratedTexture();
	void NotifyUpdate(const unsigned int  labelid) override;
	void Generate();

	void Activate(unsigned int a_Location) override;
	bool Deactivate(unsigned int a_Location) override;

	maInt			myTextureChannel;
	maVect3DF		mySize;
	maFloat			myScale;
	maFloat			myPersistence;
	maInt			myOctaveCount;

	unsigned int	myTextureGLIndex;
};


class API3DUniformMatrixArray : public API3DUniformBase
{
public:
	DECLARE_CLASS_INFO(API3DUniformMatrixArray, API3DUniformBase, Renderer)
	API3DUniformMatrixArray(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	virtual ~API3DUniformMatrixArray();
	void NotifyUpdate(const unsigned int  labelid) override;
	void Activate(unsigned int a_Location) override;

	maInt			myArraySize;
	maBuffer		myMatrixArrayAccess;
	Matrix4x4*		myMatrixArray;
};
#endif //_GLSLUNIFORM_H
