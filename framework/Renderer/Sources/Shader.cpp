#include "Shader.h"

#include "NotificationCenter.h"
#include "TravState.h"

using namespace Kigs::Draw;

void UniformList::Push(API3DUniformBase * u)
{
	mList.push_back(((CoreModifiable*)u)->SharedFromThis());
	mCurrent = u;
}
void UniformList::Pop()
{
	if (mCurrent == nullptr)
		return;

	mList.pop_back();

	if (mList.size() > 0)
		mCurrent = (API3DUniformBase*)mList.back().get();
	else
		mCurrent = nullptr;
}

IMPLEMENT_CLASS_INFO(ShaderBase)

ShaderBase::ShaderBase(const std::string& name, CLASS_NAME_TREE_ARG) : Drawable(name, PASS_CLASS_NAME_TREE_ARG)
, mCurrentShader(nullptr)
, mVertexShader(*this, false, "VertexShader", "")
, mFragmentShader(*this, false, "FragmentShader", "")
, mGeometryShader(*this, false, "GeometryShader", "")
, mAttachedCamera(*this, false, "AttachedCamera", "")
, museGenericLight(*this, false, "useGenericLight", false)
, misGeneric(*this, false, "isGeneric", false)
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
	if ((labelid == mVertexShader.getLabelID()) || (labelid == mFragmentShader.getLabelID()))
	{
		Dealloc();
		// rebuild only if both shaders are set
		if ((((std::string)mVertexShader) != "") && (((std::string)mFragmentShader) != ""))
		{
			BuildShaderStruct* toAdd=Rebuild();
			insertBuildShader(mCurrentShaderKey, toAdd);
			setCurrentBuildShader(mCurrentShaderKey);
		}
	}

	Drawable::NotifyUpdate(labelid);
}

void	ShaderBase::insertBuildShader(unsigned int key, BuildShaderStruct* toAdd)
{
	std::map<unsigned int, BuildShaderStruct*>::iterator found= mShaderSourceMap.find(key);
	if (found != mShaderSourceMap.end())
	{
		delete (*found).second;
	}

	mShaderSourceMap[key] = toAdd;

}

void	ShaderBase::setCurrentBuildShader(unsigned int key)
{
	std::map<unsigned int, BuildShaderStruct*>::iterator found = mShaderSourceMap.find(key);
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

void ShaderBase::GetMatrixLoc(int loc[5])
{
	if ((mCurrentShader)->mLocations)
	{
		loc[0] = (mCurrentShader)->mLocations->projMatrix;
		loc[1] = (mCurrentShader)->mLocations->modelMatrix;
		loc[2] = (mCurrentShader)->mLocations->viewMatrix;
		loc[3] = (mCurrentShader)->mLocations->farPlane;
		loc[4] = (mCurrentShader)->mLocations->uvMatrix;
	}
}
