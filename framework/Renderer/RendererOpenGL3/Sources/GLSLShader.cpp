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

void API3DShader::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == myVertexShaderText.getLabelID()) || (labelid == myFragmentShaderText.getLabelID()))
	{
		Dealloc();
		// rebuild only if both shaders are set
		if ((((kstl::string)myVertexShaderText) != "") && (((kstl::string)myFragmentShaderText) != ""))
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
	kstl::map<UNIFORM_NAME_TYPE, UniformList*>::iterator it = (*(myCurrentShader)->myUniforms).find(l_UniformName);
	UniformList * l_ul;
	if (it == (*(myCurrentShader)->myUniforms).end())
	{
		kstl::string lName = static_cast<API3DUniformBase*>(a_Uniform)->Get_Name();

		//auto id = CharToID::GetID(lName);

		l_ul = new UniformList(lName);
		(*(myCurrentShader)->myUniforms)[l_UniformName] = l_ul;

		// retreive Location
		l_ul->Location = glGetUniformLocation(myCurrentShader->myShaderProgram->mID, lName.c_str());
	}
	else
	{
		l_ul = it->second;
	}
	if (l_ul->Location == 0xffffffff)
	{
		//printf("Try to set unknow uniform '%s' on shader %s\n", static_cast<API3DUniformBase*>(a_Uniform)->Get_Name().c_str(), getName().c_str());
		return;
	}

	l_ul->Push(static_cast<API3DUniformBase*>(a_Uniform));
	static_cast<API3DUniformBase*>(a_Uniform)->Activate(l_ul->Location);
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
	auto found = (*(myCurrentShader)->myUniforms).find(l_UniformName);
	if (found == (*(myCurrentShader)->myUniforms).end())
	{
		//printf("Try to pop unknow uniform '%s' on shader %s\n", static_cast<API3DUniformBase*>(a_Uniform)->Get_Name().c_str(), getName().c_str());
		return;
	}

	UniformList * l_ul = found->second;
	if (l_ul->Location == 0xffffffff)
	{
		//printf("Try to pop unknow uniform '%s' on shader %s\n", static_cast<API3DUniformBase*>(a_Uniform)->Get_Name().c_str(), getName().c_str());
		return;
	}

	l_ul->Pop();

	API3DUniformBase* un = l_ul->Back();
	if (un)
	{
		un->Activate(l_ul->Location);
	}

}


void API3DShader::ProtectedDestroy()
{
	Dealloc();
	Drawable::ProtectedDestroy();
}

void	API3DShader::Dealloc()
{

}



BuildShaderStruct*	API3DShader::Rebuild()
{

	kstl::string str;
	myVertexShaderText.getValue(str);

	// Compile the shader source

	const char* SrcTxt = 0;
	CoreRawBuffer* rawbuffer = nullptr;

	if (str[0] == '!') // load from file
	{
		const char* filename = (str.c_str() + 1);
		FilePathManager*	pathManager = (FilePathManager*)KigsCore::GetSingleton("FilePathManager");

		kstl::string fullfilename;
		if (pathManager)
		{
			SmartPointer<FileHandle> file = pathManager->FindFullName(filename);
			fullfilename = file->myFullFileName;
		}
		u64 length;
		rawbuffer = ModuleFileManager::LoadFileAsCharString(fullfilename.c_str(), length);
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

	if (rawbuffer)
		rawbuffer->Destroy();
	rawbuffer = nullptr;

	myFragmentShaderText.getValue(str);
	if (str[0] == '!') // load from file
	{
		const char* filename = (str.c_str() + 1);
		FilePathManager*	pathManager = (FilePathManager*)KigsCore::GetSingleton("FilePathManager");

		kstl::string fullfilename;
		if (pathManager)
		{
			SmartPointer<FileHandle> file = pathManager->FindFullName(filename);
			fullfilename = file->myFullFileName;
		}
		u64 length;
		rawbuffer = ModuleFileManager::LoadFileAsCharString(fullfilename.c_str(), length);
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

	if (rawbuffer)
		rawbuffer->Destroy();
	rawbuffer = nullptr;

	BuildShaderStruct* toReturn = new BuildShaderStruct();

	toReturn->myVertexShader = new GLSLShaderInfo();
	toReturn->myVertexShader->mID = vshaderName;
	toReturn->myVertexShader->mType = 1;
	toReturn->myFragmentShader = new GLSLShaderInfo();
	toReturn->myFragmentShader->mID = fshaderName;
	toReturn->myFragmentShader->mType = 2;

	toReturn->myShaderProgram = new GLSLShaderInfo();
	toReturn->myShaderProgram->mID = glCreateProgram(); CHECK_GLERROR;
	toReturn->myShaderProgram->mType = 0;
	//kigsprintf("%p rebuild shader %s (%d)\n",this, getName().c_str(), myShaderProgram);

	//Attach our shaders to our program
	glAttachShader(toReturn->myShaderProgram->mID, ((GLSLShaderInfo*)toReturn->myVertexShader)->mID); CHECK_GLERROR;
	glAttachShader(toReturn->myShaderProgram->mID, ((GLSLShaderInfo*)toReturn->myFragmentShader)->mID); CHECK_GLERROR;

	//Link our program
	glLinkProgram(toReturn->myShaderProgram->mID); CHECK_GLERROR;

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(toReturn->myShaderProgram->mID, GL_LINK_STATUS, (int *)&isLinked); CHECK_GLERROR;
	if (isLinked == GL_FALSE)
	{
#ifdef _DEBUG
		GLint maxLength = 0;
		glGetProgramiv(toReturn->myShaderProgram->mID, GL_INFO_LOG_LENGTH, &maxLength); CHECK_GLERROR;

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(toReturn->myShaderProgram->mID, maxLength, &maxLength, &infoLog[0]); CHECK_GLERROR;
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
	glDetachShader(toReturn->myShaderProgram->mID, ((GLSLShaderInfo*)toReturn->myVertexShader)->mID); CHECK_GLERROR;
	glDetachShader(toReturn->myShaderProgram->mID, ((GLSLShaderInfo*)toReturn->myFragmentShader)->mID); CHECK_GLERROR;

	if (!toReturn->myUniforms)
	{
		toReturn->myUniforms = new kstl::map<UNIFORM_NAME_TYPE, UniformList*>();
	}

	// location is only for generic shaders
	if (isGeneric())
	{

		if (!toReturn->myLocations) // create locations if not set
		{
			toReturn->myLocations = new Locations();
		}

		toReturn->myLocations->projMatrix = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_MATRIX_PROJ);
		toReturn->myLocations->modelMatrix = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_MATRIX_MODEL);
		toReturn->myLocations->viewMatrix = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_MATRIX_VIEW);

		toReturn->myLocations->textureLocation[0] = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_TEXTURE_0);
		toReturn->myLocations->textureLocation[1] = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_TEXTURE_1);
		toReturn->myLocations->textureLocation[2] = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_TEXTURE_2);
		toReturn->myLocations->textureLocation[3] = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_TEXTURE_3);

		toReturn->myLocations->attribVertex = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_VERTEX);
		toReturn->myLocations->attribNormal = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_NORMAL);
		toReturn->myLocations->attribColor = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_COLOR);
		toReturn->myLocations->attribTexcoord = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_TEXCOORD);
		toReturn->myLocations->attribTangent = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_TANGENT);
		toReturn->myLocations->attribBoneWeight = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_BONE_WEIGHT);
		toReturn->myLocations->attribBoneIndex = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_BONE_INDEX);
		toReturn->myLocations->attribInstanceMatrix[0] = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX);
		if (toReturn->myLocations->attribInstanceMatrix[0] != 0xFFFFFFFF)
		{
			toReturn->myLocations->attribInstanceMatrix[1] = toReturn->myLocations->attribInstanceMatrix[0] + 1;
			toReturn->myLocations->attribInstanceMatrix[2] = toReturn->myLocations->attribInstanceMatrix[1] + 1;
		}
		toReturn->myLocations->tangentSpaceLOD = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_TANGENT_SPACE_LOD);
		toReturn->myLocations->farPlane = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_FAR_PLANE);
		toReturn->myLocations->fogScale = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_FOG_SCALE);
		toReturn->myLocations->fogColor = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_FOG_COLOR);

#ifdef WUP // holographics
		toReturn->myLocations->attribRenderTargetArrayIndex = glGetAttribLocation(toReturn->myShaderProgram->mID, KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX);
		toReturn->myLocations->holoViewMatrixLocation[0] = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_HOLO_VIEW_MATRIX_0);
		toReturn->myLocations->holoViewMatrixLocation[1] = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_HOLO_VIEW_MATRIX_1);
		toReturn->myLocations->holoViewMatrixLocation[2] = glGetUniformLocation(toReturn->myShaderProgram->mID, KIGS_HOLO_VIEW_MATRIX_2);
#endif
	}

	if (toReturn->myUniforms)
	{
		// reset uniform location
		auto itr = (*toReturn->myUniforms).begin();
		for (; itr != (*toReturn->myUniforms).end(); ++itr)
		{
			itr->second->Location = glGetUniformLocation(toReturn->myShaderProgram->mID, itr->second->myUniformName.c_str());
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
		CoreModifiableAttribute* newAttr = AddDynamicAttribute(BOOL, "DelayedInit");
		newAttr->setValue(true);
#endif
	}
}

void	API3DShader::DelayedInit(TravState* state)
{

	if ((((kstl::string)myVertexShaderText) != "") && (((kstl::string)myFragmentShaderText) != ""))
	{
		Drawable::InitModifiable();

		Rebuild();
		myVertexShaderText.changeNotificationLevel(Owner);
		myFragmentShaderText.changeNotificationLevel(Owner);
		Active(state);

		// add child unifor as default uniform
		kstl::set<CoreModifiable*> instances;
		CoreModifiable::GetSonInstancesByType("API3DUniformBase", instances);

		if (instances.size())
		{
			kstl::set<CoreModifiable*>::iterator itr = instances.begin();
			kstl::set<CoreModifiable*>::iterator end = instances.end();
			for (; itr != end; ++itr)
			{
				PushUniform(static_cast<API3DUniformBase*>(*itr));
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
			if ((*it).myItem->isUserFlagSet(UserFlagDrawable))
			{
				Drawable* drawable = (Drawable*)(*it).myItem;
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
			if ((*it).myItem->isUserFlagSet(UserFlagDrawable))
			{
				Drawable* drawable = (Drawable*)(*it).myItem;
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
		if ((CoreModifiable*)myAttachedCamera)
		{
			if ((CoreModifiable*)myAttachedCamera != (CoreModifiable*)travstate->GetCurrentCamera())
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
		if ((CoreModifiable*)myAttachedCamera)
		{
			if ((CoreModifiable*)myAttachedCamera != (CoreModifiable*)travstate->GetCurrentCamera())
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
		if (resetUniform && (myCurrentShader)->myUniforms)
		{
			// reset all uniform
			auto itr = (*(myCurrentShader)->myUniforms).begin();
			auto end = (*(myCurrentShader)->myUniforms).end();
			for (; itr != end; ++itr)
			{
				if (itr->second->Location == 0xffffffff)
					continue;

				API3DUniformBase * uni = itr->second->Back();
				if (uni)
					itr->second->Back()->Activate(itr->second->Location);
			}
		}
		if (myUseGenericLight && state)
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
		if (myUseGenericLight && state)
		{
			RendererOpenGL* renderer = (RendererOpenGL*)state->GetRenderer();
			renderer->ClearLightsInfo(state);
		}
	}
}

void	API3DShader::PrepareExport(ExportSettings* settings)
{
	ParentClassType::PrepareExport(settings);
	if (!(myCurrentShader)->myUniforms)
		return;
	auto itr = (*(myCurrentShader)->myUniforms).begin();
	for (; itr != (*(myCurrentShader)->myUniforms).end(); ++itr)
	{
		if (!itr->second->List.empty())
		{
			bool L_bAlreadyAdded = false;
			kstl::vector<CoreModifiable*> L_parents = static_cast<CoreModifiable*>(*(itr->second->List.begin()))->GetParents();
			for (int p = 0; p < static_cast<CoreModifiable*>(*(itr->second->List.begin()))->GetParentCount(); p++)
			{
				if (L_parents[p] == this)
					L_bAlreadyAdded = true;
			}
			if (!L_bAlreadyAdded)
				addItem(static_cast<CoreModifiable*>(*(itr->second->List.begin())));
		}
	}
}

void	API3DShader::EndExport(ExportSettings* settings)
{
	if ((myCurrentShader)->myUniforms)
	{

		auto itr = (*(myCurrentShader)->myUniforms).begin();
		for (; itr != (*(myCurrentShader)->myUniforms).end(); ++itr)
		{
			if (!itr->second->List.empty())
				removeItem(static_cast<CoreModifiable*>(*(itr->second->List.begin())));
		}
	}
	ParentClassType::EndExport(settings);
}

DEFINE_METHOD(API3DShader, Reload)
{
	Dealloc();
	// rebuild only if both shaders are set
	if ((((kstl::string)myVertexShaderText) != "") && (((kstl::string)myFragmentShaderText) != ""))
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
