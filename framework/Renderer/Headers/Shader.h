#ifndef _SHADER_H
#define _SHADER_H

#include "Drawable.h"
#include "maReference.h"

#include <map>

#define UNIFORM_NAME_TYPE unsigned int

// ****************************************
// * ShaderInfo class
// * --------------------------------------
/**
* \file	Shader.h
* \class	ShaderInfo
* \ingroup Renderer
* \brief Utility class to register shaders
*
*/
// ****************************************
class ShaderInfo
{
public:
	ShaderInfo() : mID(0xFFFFFFFF), mType(0) {}
	virtual ~ShaderInfo() {};
	unsigned int mID;
	unsigned int mType;	// 0=>Vertex, 1=> fragment 
};


struct Locations
{
	unsigned int projMatrix = 0xFFFFFFFF;
	unsigned int modelMatrix = 0xFFFFFFFF;
	unsigned int viewMatrix = 0xFFFFFFFF;
	unsigned int uvMatrix = 0xFFFFFFFF;

	unsigned int attribVertex = 0xFFFFFFFF;
	unsigned int attribNormal = 0xFFFFFFFF;
	unsigned int attribColor = 0xFFFFFFFF;
	unsigned int attribTexcoord = 0xFFFFFFFF;
	unsigned int attribTangent = 0xFFFFFFFF;
	unsigned int attribBoneWeight = 0xFFFFFFFF;
	unsigned int attribBoneIndex = 0xFFFFFFFF;
	unsigned int attribInstanceMatrix[3] = { 0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF };

#ifdef WUP
	unsigned int attribRenderTargetArrayIndex = 0xFFFFFFFF;
#endif

	const unsigned int* attribs = &attribVertex;

	unsigned int textureLocation[4] = { 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF };

	unsigned int tangentSpaceLOD = 0xFFFFFFFF;
	unsigned int farPlane = 0xFFFFFFFF;

	unsigned int fogScale = 0xFFFFFFFF;
	unsigned int fogColor = 0xFFFFFFFF;

#ifdef WUP // holographics
	unsigned int holoViewMatrixLocation[3];
#endif

};

class API3DUniformBase;


// ****************************************
// * UniformList struct
// * --------------------------------------
/**
* \file	Shader.h
* \class	UniformList
* \ingroup Renderer
* \brief Manage shader uniforms
*
*/
// ****************************************
struct UniformList
{
	UniformList(std::string aName)
	{
		mCurrent = nullptr;
		mUniformName = aName;
		mLocation = 0xffffffff;
#ifdef USE_D3D
		mLocationFragment = 0xffffffff;
#endif
		mList.clear();
	}

	API3DUniformBase * Back()
	{
		return mCurrent;
	}

	void ResetLocation(unsigned int program)
	{}

	void Push(API3DUniformBase * u);
	void Pop();

	unsigned int mLocation;
#ifdef USE_D3D
	unsigned int mLocationFragment;
#endif

	API3DUniformBase* mCurrent;
	std::vector<CMSP> mList;

	std::string mUniformName;
};


// ****************************************
// * BuildShaderStruct class
// * --------------------------------------
/**
* \file	Shader.h
* \class	BuildShaderStruct
* \ingroup Renderer
* \brief Utility class to build shaders
*
*/
// ****************************************
class BuildShaderStruct
{
public:
	BuildShaderStruct() : mVertexShader(nullptr), mFragmentShader(nullptr), mShaderProgram(nullptr)
	{}

	virtual ~BuildShaderStruct()
	{
		if (mVertexShader)
		{
			delete mVertexShader;
			mVertexShader = nullptr;
		}
		if (mFragmentShader)
		{
			delete mFragmentShader;
			mFragmentShader = nullptr;
		}
		if (mShaderProgram)
		{
			delete mShaderProgram;
			mShaderProgram = nullptr;
		}

		if (mLocations)
			delete mLocations;
		mLocations = nullptr;

		if (mUniforms)
		{
			auto itr = mUniforms->begin();
			for (; itr != mUniforms->end(); ++itr)
			{
				while (itr->second->Back())
					itr->second->Pop();
				delete itr->second;
			}
			mUniforms->clear();

			delete mUniforms;
		}
		mUniforms = nullptr;
	}

	ShaderInfo *	mVertexShader;
	ShaderInfo *	mFragmentShader;
	ShaderInfo *	mShaderProgram;

	Locations*		mLocations = nullptr;
	std::map<UNIFORM_NAME_TYPE, UniformList*>*	mUniforms = nullptr;

};

// ****************************************
// * ShaderBase class
// * --------------------------------------
/**
* \file	Shader.h
* \class	ShaderBase
* \ingroup Renderer
* \brief Base virtual class for shaders
*
*/
// ****************************************

class ShaderBase : public Drawable
{
public:
	friend class ModuleSpecificRenderer;

	DECLARE_ABSTRACT_CLASS_INFO(ShaderBase,Drawable,Renderer)

	ShaderBase(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void	NotifyUpdate(const unsigned int labelid) override;

	virtual void	ChooseShader(TravState* state, unsigned int attribFlag) = 0;
	virtual void	Active(TravState* state, bool resetUniform = false)=0;
	virtual void	Deactive(TravState* state)=0;

	bool isGeneric() { return misGeneric; }

	template<typename castType= ShaderInfo>
	inline castType *	GetCurrentShaderProgram() const { if (mCurrentShader) return (castType*)mCurrentShader->mShaderProgram; return nullptr; }

	template<typename castType = ShaderInfo>
	inline castType* GetCurrentVertexShaderInfo() { if (mCurrentShader) return (castType*)mCurrentShader->mVertexShader; return nullptr; }

	template<typename castType = ShaderInfo>
	inline castType* GetCurrentFragmentShaderInfo() { if (mCurrentShader) return (castType*)mCurrentShader->mFragmentShader; return nullptr; }

	virtual void PushUniform(CoreModifiable*)=0;
	virtual void PopUniform(CoreModifiable*)=0;

	virtual	~ShaderBase();

	virtual void	Dealloc();

	virtual BuildShaderStruct*	Rebuild() = 0;
	virtual ShaderInfo *		CreateProgram() { return nullptr; };


	API3DUniformBase * GetUniform(const std::string& aUniformName)
	{
		unsigned int aUniformNameID = CharToID::GetID(aUniformName);
		return GetUniform(aUniformNameID);
	}
	unsigned int GetUniformLocation(const std::string& aUniformName)
	{
		unsigned int aUniformNameID = CharToID::GetID(aUniformName);
		return GetUniformLocation(aUniformNameID);
	}

	API3DUniformBase * GetUniform(unsigned int aUniformNameID)
	{
		if (mCurrentShader)
		{
			UniformList * l_ul = (*((mCurrentShader)->mUniforms))[aUniformNameID];
			if (l_ul)
				return l_ul->Back();
		}
		return NULL;
	}
	unsigned int GetUniformLocation(unsigned int  aUniformNameID)
	{
		if (mCurrentShader)
		{
			UniformList * l_ul = (*((mCurrentShader)->mUniforms))[aUniformNameID];
			if (l_ul)
				return l_ul->mLocation;
		}
		return (unsigned int)-1;
	}

	void GetMatrixLoc(int loc[5]);
	const Locations * GetLocation() const {
		if (mCurrentShader)
		{
			return (mCurrentShader)->mLocations;
		}
		return nullptr;
	}

protected:

	void	insertBuildShader(unsigned int key, BuildShaderStruct* toAdd);
	void	setCurrentBuildShader(unsigned int key);


	BuildShaderStruct*	mCurrentShader;
	unsigned int		mCurrentShaderKey;

	// TODO : rename to mIsGeneric ( but CoreModifiableAttribute ID should be renamed too ).
	maBool			misGeneric;
	maString		mVertexShaderText;
	maString		mFragmentShaderText;
	maReference		mAttachedCamera;
	// TODO : rename rename to mUseGenericLight (but CoreModifiableAttribute ID should be renamed too ).
	maBool			museGenericLight;

	std::map<unsigned int, BuildShaderStruct*> mShaderSourceMap;
};
#endif //_SHADER_H
