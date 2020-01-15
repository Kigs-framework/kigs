#include "PrecompiledHeaders.h"

#include "Material.h"


IMPLEMENT_CLASS_INFO(Material)

static inline bool IsVecEqual(const maVect4DF &v1, const maVect4DF&v2)
{
	return v1[0]==v2[0] && v1[1]==v2[1] && v1[2]==v2[2] && v1[3]==v2[3];
}

Material::Material(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
myFacing(*this,false,"Facing",1),
myBlendFuncSource(*this,false,"BlendFuncSource",4),
myBlendFuncDest(*this,false,"BlendFuncDest",5),
myBlendEnabled(*this,false,"BlendEnabled",false),
myMaterialColorEnabled(*this,false,"MaterialColorEnabled",false),
myAmbientColor(*this,false,"AmbientColor"),
myDiffuseColor(*this,false,"DiffuseColor"),
mySpecularColor(*this,false,"SpecularColor"),
myEmissionColor(*this,false,"EmissionColor"),
myShininess(*this,false,"Shininess",120.0f),
myTransparency(*this,false,"Transparency",1.0f)
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
	bool PropsAreEqual = (int)myFacing==(int)pOther->myFacing &&
		(int)myBlendFuncSource==(int)pOther->myBlendFuncSource &&
		(int)myBlendFuncDest==(int)pOther->myBlendFuncDest &&
		(bool)myBlendEnabled==(bool)pOther->myBlendEnabled &&
		(bool)myMaterialColorEnabled==(bool)pOther->myMaterialColorEnabled &&
		IsVecEqual(myAmbientColor, pOther->myAmbientColor) &&
		IsVecEqual(myDiffuseColor, pOther->myDiffuseColor) &&
		IsVecEqual(mySpecularColor, pOther->mySpecularColor) &&
		IsVecEqual(myEmissionColor, pOther->myEmissionColor) &&
		(kfloat)myShininess==(kfloat)pOther->myShininess &&
		(kfloat)myTransparency==(kfloat)pOther->myTransparency;
	if (!PropsAreEqual)
		return false;
	//compare children
	if (getItems().size()!=other.getItems().size())
		return false;
	for (size_t i = 0; i < getItems().size(); i++)
	{
		if (!(getItems()[i]).myItem->Equal(*((other.getItems()[i]).myItem.get())))
			return false;
	}
	return true;
}

void	Material::DoPreDraw(TravState* travstate)
{
	// change material
	if(travstate->myCurrentMaterial != this /*|| m_IsTransparent*/)
	{
		// force previous post draw
		if(travstate->myCurrentMaterial)
		{
			Material* toPostDraw=travstate->myCurrentMaterial;
			travstate->myCurrentMaterial=0;
			toPostDraw->DoPostDraw(travstate);
		}

		Drawable::DoPreDraw(travstate);
		travstate->myCurrentMaterial = this;
		//kigsprintf("%p(%s) %d >> Predraw\n", this, getName().c_str(),count++);
		// current material is now this
	}
}

void	Material::DoPostDraw(TravState* travstate)
{
	if(travstate->myCurrentMaterial != this/* || m_IsTransparent*/)
	{
		//kigsprintf("%p(%s) %d >> Postdraw\n", this, getName().c_str(), --count);
		Drawable::DoPostDraw(travstate);
		// no more current material
		travstate->myCurrentMaterial = 0;
	}
}
