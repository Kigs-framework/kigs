#ifndef _SHADER_H
#define _SHADER_H

#include "Drawable.h"
#include "maReference.h"

#include <map>

#define UNIFORM_NAME_TYPE unsigned int

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

struct UniformList
{
	UniformList(std::string aName)
	{
		Current = nullptr;
		myUniformName = aName;
		Location = 0xffffffff;
#ifdef USE_D3D
		LocationFragment = 0xffffffff;
#endif
		List.clear();
	}

	API3DUniformBase * Back()
	{
		return Current;
	}

	void ResetLocation(unsigned int program)
	{}

	void Push(API3DUniformBase * u);
	void Pop();

	unsigned int Location;
#ifdef USE_D3D
	unsigned int LocationFragment;
#endif

	API3DUniformBase* Current;
	std::vector<API3DUniformBase*> List;

	std::string myUniformName;
};


class BuildShaderStruct
{
public:
	BuildShaderStruct() : myVertexShader(nullptr), myFragmentShader(nullptr), myShaderProgram(nullptr)
	{}

	virtual ~BuildShaderStruct()
	{
		if (myVertexShader)
		{
			delete myVertexShader;
			myVertexShader = nullptr;
		}
		if (myFragmentShader)
		{
			delete myFragmentShader;
			myFragmentShader = nullptr;
		}
		if (myShaderProgram)
		{
			delete myShaderProgram;
			myShaderProgram = nullptr;
		}

		if (myLocations)
			delete myLocations;
		myLocations = nullptr;

		if (myUniforms)
		{
			auto itr = myUniforms->begin();
			for (; itr != myUniforms->end(); ++itr)
			{
				while (itr->second->Back())
					itr->second->Pop();
				delete itr->second;
			}
			myUniforms->clear();

			delete myUniforms;
		}
		myUniforms = nullptr;
	}

	ShaderInfo *	myVertexShader;
	ShaderInfo *	myFragmentShader;
	ShaderInfo *	myShaderProgram;

	Locations*		myLocations = nullptr;
	std::map<UNIFORM_NAME_TYPE, UniformList*>*	myUniforms = nullptr;

};

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

	bool isGeneric() { return myisGeneric; }

	template<typename castType= ShaderInfo>
	inline castType *	GetCurrentShaderProgram() const { if (myCurrentShader) return (castType*)myCurrentShader->myShaderProgram; return nullptr; }

	template<typename castType = ShaderInfo>
	inline castType* GetCurrentVertexShaderInfo() { if (myCurrentShader) return (castType*)myCurrentShader->myVertexShader; return nullptr; }

	template<typename castType = ShaderInfo>
	inline castType* GetCurrentFragmentShaderInfo() { if (myCurrentShader) return (castType*)myCurrentShader->myFragmentShader; return nullptr; }

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
		if (myCurrentShader)
		{
			UniformList * l_ul = (*((myCurrentShader)->myUniforms))[aUniformNameID];
			if (l_ul)
				return l_ul->Back();
		}
		return NULL;
	}
	unsigned int GetUniformLocation(unsigned int  aUniformNameID)
	{
		if (myCurrentShader)
		{
			UniformList * l_ul = (*((myCurrentShader)->myUniforms))[aUniformNameID];
			if (l_ul)
				return l_ul->Location;
		}
		return (unsigned int)-1;
	}

	void GetMatrixLoc(int loc[3]);
	const Locations * GetLocation() const {
		if (myCurrentShader)
		{
			return (myCurrentShader)->myLocations;
		}
		return nullptr;
	}

protected:

	void	insertBuildShader(unsigned int key, BuildShaderStruct* toAdd);
	void	setCurrentBuildShader(unsigned int key);


	BuildShaderStruct*	myCurrentShader;
	unsigned int		myCurrentShaderKey;

	maBool			myisGeneric;
	maString		myVertexShaderText;
	maString		myFragmentShaderText;
	maReference		myAttachedCamera;
	maBool			myUseGenericLight;

	std::map<unsigned int, BuildShaderStruct*> mShaderSourceMap;
};
#endif //_SHADER_H
