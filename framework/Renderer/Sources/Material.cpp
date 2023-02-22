#include "PrecompiledHeaders.h"

#include "Material.h"

using namespace Kigs::Draw;
IMPLEMENT_CLASS_INFO(Material)


Material::Material(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{

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
	bool PropsAreEqual = mFacing==pOther->mFacing &&
		mBlendFuncSource==pOther->mBlendFuncSource &&
		mBlendFuncDest==pOther->mBlendFuncDest &&
		mBlendEnabled==pOther->mBlendEnabled &&
		mMaterialColorEnabled==pOther->mMaterialColorEnabled &&
		mAmbientColor == pOther->mAmbientColor &&
		mDiffuseColor == pOther->mDiffuseColor &&
		mSpecularColor == pOther->mSpecularColor &&
		mEmissionColor == pOther->mEmissionColor &&
		mShininess==pOther->mShininess &&
		mTransparency==pOther->mTransparency;
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
