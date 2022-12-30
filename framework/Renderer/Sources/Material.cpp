#include "PrecompiledHeaders.h"

#include "Material.h"

using namespace Kigs::Draw;
IMPLEMENT_CLASS_INFO(Material)

static inline bool IsVecEqual(const maVect4DF &v1, const maVect4DF&v2)
{
	return v1[0]==v2[0] && v1[1]==v2[1] && v1[2]==v2[2] && v1[3]==v2[3];
}

Material::Material(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mFacing(*this,false,"Facing",1),
mBlendFuncSource(*this,false,"BlendFuncSource",4),
mBlendFuncDest(*this,false,"BlendFuncDest",5),
mBlendEnabled(*this,false,"BlendEnabled",false),
mMaterialColorEnabled(*this,false,"MaterialColorEnabled",false),
mAmbientColor(*this,false,"AmbientColor"),
mDiffuseColor(*this,false,"DiffuseColor"),
mSpecularColor(*this,false,"SpecularColor"),
mEmissionColor(*this,false,"EmissionColor"),
mShininess(*this,false,"Shininess",120.0f),
mTransparency(*this,false,"Transparency",1.0f)
{
  SetAmbientColor(0.2f, 0.2f, 0.2f);
  SetDiffuseColor(0.3f, 0.3f, 0.3f);
  SetSpecularColor(0.1f, 0.1f, 0.1f);
  SetEmissionColor(0.0f, 0.0f, 0.0f);
}    

    
Material::~Material()
{
}


bool	Material::Equal(const CoreModifiable& other)
{
	if (other.getExactType()!=getExactType())
		return false;

	// must have same attribute count
	if(getAttributes().size() != other.getAttributes().size())
	{
		return false;
	}

	Material *pOther = (Material*)&other;
	bool PropsAreEqual = (int)mFacing==(int)pOther->mFacing &&
		(int)mBlendFuncSource==(int)pOther->mBlendFuncSource &&
		(int)mBlendFuncDest==(int)pOther->mBlendFuncDest &&
		(bool)mBlendEnabled==(bool)pOther->mBlendEnabled &&
		(bool)mMaterialColorEnabled==(bool)pOther->mMaterialColorEnabled &&
		IsVecEqual(mAmbientColor, pOther->mAmbientColor) &&
		IsVecEqual(mDiffuseColor, pOther->mDiffuseColor) &&
		IsVecEqual(mSpecularColor, pOther->mSpecularColor) &&
		IsVecEqual(mEmissionColor, pOther->mEmissionColor) &&
		(float)mShininess==(float)pOther->mShininess &&
		(float)mTransparency==(float)pOther->mTransparency;
	if (!PropsAreEqual)
		return false;
	//compare children
	if (getItems().size()!=other.getItems().size())
		return false;
	for (size_t i = 0; i < getItems().size(); i++)
	{
		if (!(getItems()[i]).mItem->Equal(*((other.getItems()[i]).mItem.get())))
			return false;
	}
	return true;
}

void	Material::DoPreDraw(TravState* travstate)
{
	// change material
	if(travstate->mCurrentMaterial != this)
	{
		// force previous post draw
		if(travstate->mCurrentMaterial)
		{
			Material* toPostDraw=travstate->mCurrentMaterial;
			travstate->mCurrentMaterial=0;
			toPostDraw->DoPostDraw(travstate);
		}

		Drawable::DoPreDraw(travstate);
		travstate->mCurrentMaterial = this;
		//kigsprintf("%p(%s) %d >> Predraw\n", this, getName().c_str(),count++);
		// current material is now this
	}
}

void	Material::DoPostDraw(TravState* travstate)
{
	if(travstate->mCurrentMaterial != this)
	{
		//kigsprintf("%p(%s) %d >> Postdraw\n", this, getName().c_str(), --count);
		Drawable::DoPostDraw(travstate);
		// no more current material
		travstate->mCurrentMaterial = 0;
	}
}
