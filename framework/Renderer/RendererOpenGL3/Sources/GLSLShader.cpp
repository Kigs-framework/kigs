#include "RendererDefines.h"

#include "GLSLShader.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "ModuleSceneGraph.h"
#include "Core.h"
#include "RendererOpenGL.h"
#include "CoreRawBuffer.h"
#include "GLSLUniform.h"
#include "TravState.h"
#include "NotificationCenter.h"
#include "OpenGLMaterial.h"
#include "Platform/Renderer/OpenGLInclude.h"



GLSLShaderInfo::~GLSLShaderInfo()
{
	if (mID != 0xFFFFFFFF)
	{
		switch (mType)
		{
		case 0:
			glDeleteProgram(mID);
			break;
		case 1:
		case 2:
			glDeleteShader(mID);
			break;
		default:
			break;
		}
		mID = 0xFFFFFFFF;
	}
}



IMPLEMENT_CLASS_INFO(API3DShader)

API3DShader::API3DShader(const kstl::string& name, CLASS_NAME_TREE_ARG) : ShaderBase(name, PASS_CLASS_NAME_TREE_ARG)
{
	KigsCore::GetNotificationCenter()->addObserver(this, "Reload", "ResetContext");
}

API3DShader::~API3DShader()
{
	Dealloc();
}

void API3DShader::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mVertexShaderText.getLabelID()) || (labelid == mFragmentShaderText.getLabelID()))
	{
		Dealloc();
		// rebuild only if both shaders are set
		if ((((kstl::string)mVertexShaderText) != "") && (((kstl::string)mFragmentShaderText) != ""))
		{
			Rebuild();
		}
	}

	Drawable::NotifyUpdate(labelid);
}

void	API3DShader::PushUniform(CoreModifiable * a_Uniform)
{

	if (!isOKToUse())
	{
		return;
	}
	UNIFORM_NAME_TYPE l_UniformName = static_cast<API3DUniformBase*>(a_Uniform)->Get_ID();

#ifdef _DEBUG
	//if (getAttribute("TraceUniform"))
		//printf("push : %s, on %s\n", static_cast<API3DUniformBase*>(a_Uniform)->Get_Name().c_str(), getName().c_str());
#endif

	// get UniformList
	kstl::map<UNIFORM_NAME_TYPE, UniformList*>::iterator it = (*(mCurrentShader)->mUniforms).find(l_UniformName);
	UniformList * l_ul;
	if (it == (*(mCurrentShader)->mUniforms).end())
	{
		kstl::string lName = static_cast<API3DUniformBase*>(a_Uniform)->Get_Name();

		//auto id = CharToID::GetID(lName);

		l_ul = new UniformList(lName);
		(*(mCurrentShader)->mUniforms)[l_UniformName] = l_ul;

		// retreive mLocation
		l_ul->mLocation = glGetUniformLocation(mCurrentShader->mShaderProgram->mID, lName.c_str());
	}
	else
	{
		l_ul = it->second;
	}
	if (l_ul->mLocation == 0xffffffff)
	{
		//printf("Try to set unknow uniform '%s' on shader %s\n", static_cast<API3DUniformBase*>(a_Uniform)->Get_Name().c_str(), getName().c_str());
		return;
	}

	l_ul->Push(static_cast<API3DUniformBase*>(a_Uniform));
	static_cast<API3DUniformBase*>(a_Uniform)->Activate(l_ul->mLocation);
}

void	API3DShader::PopUniform(CoreModifiable * a_Uniform)
{

	if (!isOKToUse())
	{
		return;
	}

	UNIFORM_NAME_TYPE l_UniformName = static_cast<API3DUniformBase*>(a_Uniform)->Get_ID();

#ifdef _DEBUG
	//if (getAttribute(LABEL_TO_ID(TraceUniform)))
		//printf("pop : %s, on %s\n", static_cast<API3DUniformBase*>(a_Uniform)->Get_Name().c_str(), getName().c_str());
#endif

	// get UniformList
	auto found = (*(mCurrentShader)->mUniforms).find(l_UniformName);
	if (found == (*(mCurrentShader)->mUniforms).end())
	{
		//printf("Try to pop unknow uniform '%s' on shader %s\n", static_cast<API3DUniformBase*>(a_Uniform)->Get_Name().c_str(), getName().c_str());
		return;
	}

	UniformList * l_ul = found->second;
	if (l_ul->mLocation == 0xffffffff)
	{
		//printf("Try to pop unknow uniform '%s' on shader %s\n", static_cast<API3DUniformBase*>(a_Uniform)->Get_Name().c_str(), getName().c_str());
		return;
	}

	l_ul->Pop();

	API3DUniformBase* un = l_ul->Back();
	if (un)
	{
		un->Activate(l_ul->mLocation);
	}

}

void	API3DShader::Dealloc()
{

}



BuildShaderStruct*	API3DShader::Rebuild()
{

	kstl::string str;
	mVertexShaderText.getValue(str);

	// Compile the shader source

	const char* SrcTxt = 0;
	SP<CoreRawBuffer> rawbuffer;

	if (str[0] == '!') // load from file
	{
		const char* filename = (str.c_str() + 1);
		auto pathManager = KigsCore::Singleton<FilePathManager>();

		kstl::string fullfilename;
		if (pathManager)
		{
			SmartPointer<FileHandle> file = pathManager->FindFullName(filename);
			fullfilename = file->mFullFileName;
		}
		u64 length;
		rawbuffer = ModuleFileManager::LoadFileAsCharString(fullfilename.c_str(), length,1);
		if (rawbuffer)
		{
			SrcTxt = (const char*)rawbuffer->buffer();
		}


	}
	else
	{
		SrcTxt = str.c_str();
	}

	int vshaderName = glCreateShader(GL_VERTEX_SHADER); CHECK_GLERROR;
	if (vshaderName == 0xFFFFFFFF)
		return nullptr;

	glShaderSource(vshaderName, 1, &SrcTxt, 0); CHECK_GLERROR;
	glCompileShader(vshaderName); CHECK_GLERROR;
	rawbuffer = nullptr;

	mFragmentShaderText.getValue(str);
	if (str[0] == '!') // load from file
	{
		const char* filename = (str.c_str() + 1);
		auto pathManager = KigsCore::Singleton<FilePathManager>();

		kstl::string fullfilename;
		if (pathManager)
		{
			SmartPointer<FileHandle> file = pathManager->FindFullName(filename);
			fullfilename = file->mFullFileName;
		}
		u64 length;
		rawbuffer = ModuleFileManager::LoadFileAsCharString(fullfilename.c_str(), length,1);
		if (rawbuffer)
		{
			SrcTxt = (const char*)rawbuffer->buffer();
		}
	}
	else
	{
		SrcTxt = str.c_str();
	}

	int fshaderName = glCreateShader(GL_FRAGMENT_SHADER); CHECK_GLERROR;
	if (fshaderName == 0xFFFFFFFF)
		return nullptr;

	glShaderSource(fshaderName, 1, &SrcTxt, 0); CHECK_GLERROR;
	glCompileShader(fshaderName); CHECK_GLERROR;
	rawbuffer = nullptr;

	BuildShaderStruct* toReturn = new BuildShaderStruct();

	toReturn->mVertexShader = new GLSLShaderInfo();
	toReturn->mVertexShader->mID = vshaderName;
	toReturn->mVertexShader->mType = 1;
	toReturn->mFragmentShader = new GLSLShaderInfo();
	toReturn->mFragmentShader->mID = fshaderName;
	toReturn->mFragmentShader->mType = 2;

	toReturn->mShaderProgram = new GLSLShaderInfo();
	toReturn->mShaderProgram->mID = glCreateProgram(); CHECK_GLERROR;
	toReturn->mShaderProgram->mType = 0;
	//kigsprintf("%p rebuild shader %s (%d)\n",this, getName().c_str(), mShaderProgram);

	//Attach our shaders to our program
	glAttachShader(toReturn->mShaderProgram->mID, ((GLSLShaderInfo*)toReturn->mVertexShader)->mID); CHECK_GLERROR;
	glAttachShader(toReturn->mShaderProgram->mID, ((GLSLShaderInfo*)toReturn->mFragmentShader)->mID); CHECK_GLERROR;

	//Link our program
	glLinkProgram(toReturn->mShaderProgram->mID); CHECK_GLERROR;

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(toReturn->mShaderProgram->mID, GL_LINK_STATUS, (int *)&isLinked); CHECK_GLERROR;
	if (isLinked == GL_FALSE)
	{
#ifdef _DEBUG
		GLint maxLength = 0;
		glGetProgramiv(toReturn->mShaderProgram->mID, GL_INFO_LOG_LENGTH, &maxLength); CHECK_GLERROR;

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(toReturn->mShaderProgram->mID, maxLength, &maxLength, &infoLog[0]); CHECK_GLERROR;
		kstl::string txt = "";
		txt.reserve(maxLength);
		std::vector<char>::iterator itr = infoLog.begin();
		std::vector<char>::iterator end = infoLog.end();
		for (; itr != end; ++itr)
			txt += *itr;

		kigsprintf("KIGS kigs: %s\n", txt.c_str());
#endif
		//We don't need the program anymore.
		Dealloc();
		return nullptr;
	}

	//Always detach shaders after a successful link.
	glDetachShader(toReturn->mShaderProgram->mID, ((GLSLShaderInfo*)toReturn->mVertexShader)->mID); CHECK_GLERROR;
	glDetachShader(toReturn->mShaderProgram->mID, ((GLSLShaderInfo*)toReturn->mFragmentShader)->mID); CHECK_GLERROR;

	if (!toReturn->mUniforms)
	{
		toReturn->mUniforms = new kstl::map<UNIFORM_NAME_TYPE, UniformList*>();
	}

	// location is only for generic shaders
	if (isGeneric())
	{

		if (!toReturn->mLocations) // create locations if not set
		{
			toReturn->mLocations = new Locations();
		}

		toReturn->mLocations->projMatrix = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_MATRIX_PROJ);
		toReturn->mLocations->modelMatrix = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_MATRIX_MODEL);
		toReturn->mLocations->viewMatrix = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_MATRIX_VIEW);

		toReturn->mLocations->uvMatrix = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_MATRIX_UV);

		toReturn->mLocations->textureLocation[0] = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_TEXTURE_0);
		toReturn->mLocations->textureLocation[1] = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_TEXTURE_1);
		toReturn->mLocations->textureLocation[2] = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_TEXTURE_2);
		toReturn->mLocations->textureLocation[3] = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_TEXTURE_3);

		toReturn->mLocations->attribVertex = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_VERTEX);
		toReturn->mLocations->attribNormal = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_NORMAL);
		toReturn->mLocations->attribColor = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_COLOR);
		toReturn->mLocations->attribTexcoord = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_TEXCOORD);
		toReturn->mLocations->attribTangent = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_TANGENT);
		toReturn->mLocations->attribBoneWeight = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_BONE_WEIGHT);
		toReturn->mLocations->attribBoneIndex = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_BONE_INDEX);
		toReturn->mLocations->attribInstanceMatrix[0] = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX);
		if (toReturn->mLocations->attribInstanceMatrix[0] != 0xFFFFFFFF)
		{
			toReturn->mLocations->attribInstanceMatrix[1] = toReturn->mLocations->attribInstanceMatrix[0] + 1;
			toReturn->mLocations->attribInstanceMatrix[2] = toReturn->mLocations->attribInstanceMatrix[1] + 1;
		}
		toReturn->mLocations->tangentSpaceLOD = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_TANGENT_SPACE_LOD);
		toReturn->mLocations->farPlane = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_FAR_PLANE);
		toReturn->mLocations->fogScale = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_FOG_SCALE);
		toReturn->mLocations->fogColor = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_FOG_COLOR);

#ifdef WUP // holographics
		toReturn->mLocations->attribRenderTargetArrayIndex = glGetAttribLocation(toReturn->mShaderProgram->mID, KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX);
		toReturn->mLocations->holoViewMatrixLocation[0] = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_HOLO_VIEW_MATRIX_0);
		toReturn->mLocations->holoViewMatrixLocation[1] = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_HOLO_VIEW_MATRIX_1);
		toReturn->mLocations->holoViewMatrixLocation[2] = glGetUniformLocation(toReturn->mShaderProgram->mID, KIGS_HOLO_VIEW_MATRIX_2);
#endif
	}

	if (toReturn->mUniforms)
	{
		// reset uniform location
		auto itr = (*toReturn->mUniforms).begin();
		for (; itr != (*toReturn->mUniforms).end(); ++itr)
		{
			itr->second->mLocation = glGetUniformLocation(toReturn->mShaderProgram->mID, itr->second->mUniformName.c_str());
		}
		
	}
	CHECK_GLERROR;

	return toReturn;
}

void	API3DShader::InitModifiable()
{
	if (!IsInit())
	{
#ifdef NO_DELAYED_INIT
		DelayedInit(nullptr);
#else
		CoreModifiableAttribute* delayed = getAttribute("DelayedInit");
		if (delayed) // delayed init already asked
		{
			return;
		}

		// ask for delayed init
		CoreModifiableAttribute* newAttr = AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "DelayedInit");
		newAttr->setValue(true);
#endif
	}
}

void	API3DShader::DelayedInit(TravState* state)
{

	if ((((kstl::string)mVertexShaderText) != "") && (((kstl::string)mFragmentShaderText) != ""))
	{
		Drawable::InitModifiable();

		Rebuild();
		mVertexShaderText.changeNotificationLevel(Owner);
		mFragmentShaderText.changeNotificationLevel(Owner);
		Active(state);

		// add child unifor as default uniform
		kstl::vector<CMSP> instances;
		CoreModifiable::GetSonInstancesByType("API3DUniformBase", instances);

		if (instances.size())
		{
			kstl::vector<CMSP>::iterator itr = instances.begin();
			kstl::vector<CMSP>::iterator end = instances.end();
			for (; itr != end; ++itr)
			{
				PushUniform(static_cast<API3DUniformBase*>((*itr).get()));
			}
		}
		Deactive(state);
	}
}

void	API3DShader::DoPreDraw(TravState* state)
{

	if (PreDraw(state)) 
	{

		CoreModifiableAttribute* delayed = getAttribute("DelayedInit");
		if (delayed)
		{
			DelayedInit(state);
			RemoveDynamicAttribute("DelayedInit");
		}

		state->GetRenderer()->pushShader(this, state);
		// then PreDraw for sons
		kstl::vector<ModifiableItemStruct>::const_iterator it;

		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			if ((*it).mItem->isUserFlagSet(UserFlagDrawable))
			{
				SP<Drawable>& drawable = (SP<Drawable>&)(*it).mItem;
				drawable->CheckPreDraw(state);
			}
		}
	}
	
}

void	API3DShader::DoPostDraw(TravState* state)
{
	
	if (PostDraw(state)) // first PostDraw for this
	{
		// then PostDraw for sons
		kstl::vector<ModifiableItemStruct>::const_iterator it;

		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			if ((*it).mItem->isUserFlagSet(UserFlagDrawable))
			{
				SP<Drawable>& drawable = (SP<Drawable>&)(*it).mItem;
				drawable->CheckPostDraw(state);
			}
		}

		state->GetRenderer()->popShader(this,state);
	}
	

}

bool	API3DShader::PreDraw(TravState* travstate)
{

	if (Drawable::PreDraw(travstate))
	{
		if ((CoreModifiable*)mAttachedCamera)
		{
			if ((CoreModifiable*)mAttachedCamera != (CoreModifiable*)travstate->GetCurrentCamera())
			{
				return false;
			}
		}

		return true;
	}
	return false;
}

bool	API3DShader::PostDraw(TravState* travstate)
{
	if (Drawable::PostDraw(travstate))
	{
		if ((CoreModifiable*)mAttachedCamera)
		{
			if ((CoreModifiable*)mAttachedCamera != (CoreModifiable*)travstate->GetCurrentCamera())
			{
				return false;
			}
		}
		
		return true;
	}

	return false;
}

void	API3DShader::Active(TravState* state, bool resetUniform)
{
	if (isOKToUse())
	{
		state->GetRenderer()->setCurrentShaderProgram(GetCurrentShaderProgram());
		if (resetUniform && (mCurrentShader)->mUniforms)
		{
			// reset all uniform
			auto itr = (*(mCurrentShader)->mUniforms).begin();
			auto end = (*(mCurrentShader)->mUniforms).end();
			for (; itr != end; ++itr)
			{
				if (itr->second->mLocation == 0xffffffff)
					continue;

				API3DUniformBase * uni = itr->second->Back();
				if (uni)
					itr->second->Back()->Activate(itr->second->mLocation);
			}
		}
		if (museGenericLight && state)
		{
			RendererOpenGL* renderer = (RendererOpenGL*)state->GetRenderer();
			renderer->SendLightsInfo(state);
		}
	}
}

void	API3DShader::Deactive(TravState* state)
{
	if (isOKToUse())
	{
		if (museGenericLight && state)
		{
			RendererOpenGL* renderer = (RendererOpenGL*)state->GetRenderer();
			renderer->ClearLightsInfo(state);
		}
	}
}

void	API3DShader::PrepareExport(ExportSettings* settings)
{
	ParentClassType::PrepareExport(settings);
	if (!mCurrentShader)
		return;
	if (!(mCurrentShader)->mUniforms)
		return;
	auto itr = (*(mCurrentShader)->mUniforms).begin();
	for (; itr != (*(mCurrentShader)->mUniforms).end(); ++itr)
	{
		if (!itr->second->mList.empty())
		{
			bool L_bAlreadyAdded = false;
			auto first_uniform = itr->second->mList.front();
			kstl::vector<CoreModifiable*> L_parents = first_uniform->GetParents();
			for (int p = 0; p < first_uniform->GetParentCount(); p++)
			{
				if (L_parents[p] == this)
					L_bAlreadyAdded = true;
			}
			if (!L_bAlreadyAdded)
			{
				addItem(first_uniform);
			}
		}
	}
}

void	API3DShader::EndExport(ExportSettings* settings)
{
	if (mCurrentShader)
	if ((mCurrentShader)->mUniforms)
	{

		auto itr = (*(mCurrentShader)->mUniforms).begin();
		for (; itr != (*(mCurrentShader)->mUniforms).end(); ++itr)
		{
			if (!itr->second->mList.empty())
			{
				auto first_uniform = itr->second->mList.front();
				removeItem(first_uniform);
			}
		}
	}
	ParentClassType::EndExport(settings);
}

DEFINE_METHOD(API3DShader, Reload)
{
	Dealloc();
	// rebuild only if both shaders are set
	if ((((kstl::string)mVertexShaderText) != "") && (((kstl::string)mFragmentShaderText) != ""))
	{
		Rebuild();
	}
	else
	{
		//printf("%s not rebuild\n", getName().c_str());
	}
	return false;
}

DEFINE_METHOD(API3DShader, Active)
{
	Active(0);
	return true;
}
