#include "Shader.h"

#include "NotificationCenter.h"
#include "TravState.h"


void UniformList::Push(API3DUniformBase * u)
{
	((CoreModifiable*)u)->GetRef();
	mList.push_back(u);
	mCurrent = u;
}
void UniformList::Pop()
{
	if (mCurrent == nullptr)
		return;

	((CoreModifiable*)mCurrent)->Destroy();
	mList.pop_back();

	if (mList.size() > 0)
		mCurrent = mList.back();
	else
		mCurrent = nullptr;
}

IMPLEMENT_CLASS_INFO(ShaderBase)

ShaderBase::ShaderBase(const kstl::string& name, CLASS_NAME_TREE_ARG) : Drawable(name, PASS_CLASS_NAME_TREE_ARG)
, mCurrentShader(nullptr)
, mVertexShaderText(*this, false, LABEL_AND_ID(VertexShader), "")
, mFragmentShaderText(*this, false, LABEL_AND_ID(FragmentShader), "")
, mAttachedCamera(*this, false, LABEL_AND_ID(AttachedCamera), "")
, museGenericLight(*this, false, LABEL_AND_ID(useGenericLight), false)
, misGeneric(*this, false, LABEL_AND_ID(isGeneric), false)
, mCurrentShaderKey(-1)
{
	mRenderPassMask = 0xFFFFFFFF;
}

ShaderBase::~ShaderBase()
{
	Dealloc();
}

void ShaderBase::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mVertexShaderText.getLabelID()) || (labelid == mFragmentShaderText.getLabelID()))
	{
		Dealloc();
		// rebuild only if both shaders are set
		if ((((kstl::string)mVertexShaderText) != "") && (((kstl::string)mFragmentShaderText) != ""))
		{
			BuildShaderStruct* toAdd=Rebuild();
			insertBuildShader(mCurrentShaderKey, toAdd);
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
		mCurrentShader= (*found).second;
		mCurrentShaderKey = key;
		return;
	}
	mCurrentShader = nullptr;
	mCurrentShaderKey = -1;

}


void	ShaderBase::Dealloc()
{
	for (auto it : mShaderSourceMap)
	{
		delete it.second;
	}
	mShaderSourceMap.clear();
	mCurrentShader = nullptr;
}

void ShaderBase::GetMatrixLoc(int loc[4])
{
	if ((mCurrentShader)->mLocations)
	{
		loc[0] = (mCurrentShader)->mLocations->projMatrix;
		loc[1] = (mCurrentShader)->mLocations->modelMatrix;
		loc[2] = (mCurrentShader)->mLocations->viewMatrix;
		loc[3] = (mCurrentShader)->mLocations->farPlane;
	}
}
