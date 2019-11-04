#include "Shader.h"

#include "NotificationCenter.h"
#include "TravState.h"


void UniformList::Push(API3DUniformBase * u)
{
	((CoreModifiable*)u)->GetRef();
	List.push_back(u);
	Current = u;
}
void UniformList::Pop()
{
	if (Current == nullptr)
		return;

	((CoreModifiable*)Current)->Destroy();
	List.pop_back();

	if (List.size() > 0)
		Current = List.back();
	else
		Current = nullptr;
}

IMPLEMENT_CLASS_INFO(ShaderBase)

ShaderBase::ShaderBase(const kstl::string& name, CLASS_NAME_TREE_ARG) : Drawable(name, PASS_CLASS_NAME_TREE_ARG)
, myCurrentShader(nullptr)
, myVertexShaderText(*this, false, LABEL_AND_ID(VertexShader), "")
, myFragmentShaderText(*this, false, LABEL_AND_ID(FragmentShader), "")
, myAttachedCamera(*this, false, LABEL_AND_ID(AttachedCamera), "")
, myUseGenericLight(*this, false, LABEL_AND_ID(useGenericLight), false)
, myisGeneric(*this, false, LABEL_AND_ID(isGeneric), false)
, myCurrentShaderKey(-1)
{
	mRenderPassMask = 0xFFFFFFFF;
}

ShaderBase::~ShaderBase()
{
	Dealloc();
}

void ShaderBase::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == myVertexShaderText.getLabelID()) || (labelid == myFragmentShaderText.getLabelID()))
	{
		Dealloc();
		// rebuild only if both shaders are set
		if ((((kstl::string)myVertexShaderText) != "") && (((kstl::string)myFragmentShaderText) != ""))
		{
			BuildShaderStruct* toAdd=Rebuild();
			insertBuildShader(myCurrentShaderKey, toAdd);
		}
	}

	Drawable::NotifyUpdate(labelid);
}

void	ShaderBase::insertBuildShader(unsigned int key, BuildShaderStruct* toAdd)
{
	kstl::map<unsigned int, BuildShaderStruct*>::iterator found= mShaderSourceMap.find(key);
	if (found != mShaderSourceMap.end())
	{
		delete (*found).second;
	}

	mShaderSourceMap[key] = toAdd;

}

void	ShaderBase::setCurrentBuildShader(unsigned int key)
{
	kstl::map<unsigned int, BuildShaderStruct*>::iterator found = mShaderSourceMap.find(key);
	if (found != mShaderSourceMap.end())
	{
		myCurrentShader= (*found).second;
		myCurrentShaderKey = key;
		return;
	}
	myCurrentShader = nullptr;
	myCurrentShaderKey = -1;

}


void	ShaderBase::Dealloc()
{
	for (auto it : mShaderSourceMap)
	{
		delete it.second;
	}
	mShaderSourceMap.clear();
	myCurrentShader = nullptr;
}

void ShaderBase::GetMatrixLoc(int loc[4])
{
	if ((myCurrentShader)->myLocations)
	{
		loc[0] = (myCurrentShader)->myLocations->projMatrix;
		loc[1] = (myCurrentShader)->myLocations->modelMatrix;
		loc[2] = (myCurrentShader)->myLocations->viewMatrix;
		loc[3] = (myCurrentShader)->myLocations->farPlane;
	}
}
