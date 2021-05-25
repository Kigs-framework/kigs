#include "MeshConvert.h"

//Modules
#include "CoreIncludes.h"
#include "CollisionModule.h"

#include "TimerIncludes.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "RendererIncludes.h"
#include "SceneGraphIncludes.h"
#include "GenericAnimationModule.h"

#include "GUIIncludes.h"
#include "OBJImport.h"
#include "ModernMesh.h"
#include "ColladaParser.h"
#include "FBXImport.h"
#include "STLImport.h"
#include "PLYImport.h"
#include <algorithm>

void	usage()
{
	printf("Usage : \n");
	printf("MeshConvert -i inputMeshPath [ -o outputMeshPath ] -t[g] [-n] [-f(i1,i2,i3)] [-s]  \n\n");
	printf("-i inputMeshPath : name of input mesh, can be .obj .mesh .dae .stl .ply or .fbx file \n");
	printf("-o outputMeshPath : name of ouput mesh, in xml format \n");
	printf("-t[g] : add vertex tangent - default is no vertex tangent, if g is used, use global tangent computation (based on normal) \n");
	printf("-n : add normals if not already there \n");
	printf("-f (i1,i2,i3) : i1, i2, i3 in {-1,-2,-3,1,2,3} mean {-X,-Y,-Z,X,Y,Z) \n");
	printf("   for freecad ifc => dae conversion use -f (1,-3,2)\n");
	printf("-s scaleFactor: multiply position (vertices and matrices) by scaleFactor > 0.0");
	printf("-w : export whole scene as a Scene3D");
}

void	MeshConvert::InitExternClasses()
{
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(),ColladaParser,ColladaParser, MeshConvert)
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(),OBJImport,OBJImport,MeshConvert)
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), STLImport, STLImport, MeshConvert)
#ifdef USE_FBXSDK
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), FBXImport, FBXImport, MeshConvert)
#endif
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), PLYImport, PLYImport, MeshConvert)


}

IMPLEMENT_CLASS_INFO(MeshConvert)

//-------------------------------------------------------------------------
//Constructor

MeshConvert::MeshConvert(const kstl::string& name, CLASS_NAME_TREE_ARG):
CoreBaseApplication(name, PASS_CLASS_NAME_TREE_ARG)
{
	myObjectsToDestroy.clear();
}

MeshConvert::~MeshConvert()
{
	myObjectsToDestroy.clear();
}

void	MeshConvert::RetreiveShortNameAndExt(const kstl::string& filename,kstl::string& shortname,kstl::string& fileext)
{
	int pos=static_cast<int>(filename.rfind("/"))+1;
	int pos1=static_cast<int>(filename.rfind("\\"))+1;

	if(pos1 > pos)
	{
		pos=pos1;
	}

	bool	filenameIsShortfilename=false;

	if(pos==0)
	{
		shortname=filename;
		filenameIsShortfilename=true;
	}
	else
	{
		shortname=filename.substr(static_cast<unsigned int>(pos),filename.length()-pos);
	}

	fileext="";
	pos=static_cast<int>(shortname.rfind("."))+1;
	if(pos)
	{
		fileext.append(shortname,static_cast<unsigned int>(pos),shortname.length()-pos);
		shortname=shortname.substr(0,pos-1);
	}

	// lower case fileext
	std::transform(fileext.begin(), fileext.end(), fileext.begin(), ::tolower);
}


//-------------------------------------------------------------------------
//ProtectedInit

void	MeshConvert::ProtectedInit()
{
	theRenderer=CoreCreateModule(ModuleRenderer,nullptr);
	theSceneGraph=CoreCreateModule(ModuleSceneGraph,nullptr);
	theGUI=CoreCreateModule(ModuleGUI,nullptr);

	CoreCreateModule(GenericAnimationModule, nullptr);

	CoreCreateModule(CollisionModule, nullptr);

	InitExternClasses();


	do // pseudo loop for exit management
	{
		// retreive args
		kstl::vector<kstl::string>::iterator itArgs = mArgs.begin();
		// skip app name
		itArgs++;

		int totalArgsCount = mArgs.size() - 1;

		if ((totalArgsCount < 2))
		{
			usage();
			continue; // exit
		}

		kstl::string fileNameIn = "";
		kstl::string fileNameOut = "";

		bool wholeScene=false;

		for (; itArgs != mArgs.end(); itArgs++)
		{
			kstl::string& current = (*itArgs);

			if (current.at(0) == '-')
			{
				char argtype = current.at(1);
				switch (argtype)
				{
				case 'i':
				{
					// in already there
					if (fileNameIn != "")
					{

						usage();
						continue; // exit
					}
					itArgs++;
					fileNameIn = (*itArgs);
				}
				break;
				case 'o':
				{
					// out already there
					if (fileNameOut != "")
					{
						usage();
						continue; // exit
					}
					itArgs++;
					fileNameOut = (*itArgs);
				}
				break;

				case 'w':
				{
					wholeScene = true;
				}
				break;
				case 't':
				{
					myParams.myAddVertexTangent = 1;
					if (current.length() > 2)
					{
						char globalTangent = current.at(2);
						if (globalTangent == 'g')
						{
							myParams.myAddVertexTangent = 2;
						}
					}

				}
				break;
				case 'n':
				{
					myParams.myAddVertexNormal = true;
				}
				break;
				case 'f':
				{
					itArgs++;
					kstl::string flipAxis = (*itArgs);
					int fa[3];
					int test = sscanf(flipAxis.c_str(), "(%d,%d,%d)", &fa[0], &fa[1], &fa[2]);
					if (test == 3)
					{
						// TODO : check for valid flip
						myParams.myImportFlipAxis[0] = fa[0];
						myParams.myImportFlipAxis[1] = fa[1];
						myParams.myImportFlipAxis[2] = fa[2];
					}
					else
					{
						usage();
						continue; // exit
					}


				}
				break;
				case 's':
				{
					itArgs++;
					kstl::string scaleFactor = (*itArgs);
					kfloat readSF;
					int test = sscanf(scaleFactor.c_str(), "%f", &readSF);
					if (test == 1)
					{
						if (readSF > 0.0f)
						{
							myParams.myImportScaleFactor = readSF;
						}
						else
						{
							usage();
							continue; // exit
						}
					}
					else
					{
						usage();
						continue; // exit
					}
				}
				break;
				default:
					break;
				}
			}
		}

		kstl::string shortname, ext = "";
		if (fileNameOut == "")
		{
			RetreiveShortNameAndExt(fileNameIn, shortname, ext);
			fileNameOut = shortname + ".xml";
		}
		else // check if fileNameOut is only a path
		{
			RetreiveShortNameAndExt(fileNameOut, shortname, ext);

			if (shortname == "")
			{
				RetreiveShortNameAndExt(fileNameIn, shortname, ext);
				fileNameOut = fileNameOut + shortname + ".xml";
			}
		}
		RetreiveShortNameAndExt(fileNameIn, shortname, ext);

		// needed to have a valid GL context

		CMSP theRenderingScreen = KigsCore::GetInstanceOf("theRenderingScreen", "RenderingScreen");
		theRenderingScreen->setValue(LABEL_TO_ID(DesignSizeX), 640);
		theRenderingScreen->setValue(LABEL_TO_ID(DesignSizeY), 480);
		theRenderingScreen->setValue(LABEL_TO_ID(BitsPerZ), 16);

		CMSP theWindow = KigsCore::GetInstanceOf("theWindow", "Window");
		theWindow->setValue(LABEL_TO_ID(PositionX), 0);
		theWindow->setValue(LABEL_TO_ID(PositionY), 0);
		theWindow->setValue(LABEL_TO_ID(SizeX), 640);
		theWindow->setValue(LABEL_TO_ID(SizeY), 480);
		theWindow->setValue(LABEL_TO_ID(FullScreen), false);
		theWindow->setValue(LABEL_TO_ID(Mouse Cursor), true);

		// add screen before init
		theWindow->addItem(theRenderingScreen);
		theWindow->Init();

		ExportSettings export_settings;
		export_settings.export_buffer_attribute_as_external_file_size_threshold = std::numeric_limits<size_t>::max();

		CMSP Scene;
		// ==========================================================
		//                       DEPRECATED
		// ==========================================================
		if (ext == "mesh")
		{

			SP<ModernMesh> mesh;

			mesh = KigsCore::GetInstanceOf("mesh", "ModernMesh");
			mesh->setValue(LABEL_TO_ID(FileName), fileNameIn);

			if (myParams.myImportScaleFactor != 1.0f)
			{
				mesh->ApplyScaleFactor(myParams.myImportScaleFactor);
			}

			if ((myParams.myImportFlipAxis[0] != 1) || (myParams.myImportFlipAxis[0] != 2) || (myParams.myImportFlipAxis[0] != 3))
			{
				mesh->FlipAxis(myParams.myImportFlipAxis[0], myParams.myImportFlipAxis[1], myParams.myImportFlipAxis[2]);
			}

			if (myParams.myAddVertexNormal)
			{
				mesh->ComputeNormals();
			}

			if (myParams.myAddVertexTangent)
			{
				mesh->ComputeNormals();
				mesh->ComputeTangents((myParams.myAddVertexTangent == 1) ? true : false);
			}

			mesh->Init();
			myObjectsToDestroy.push_back(mesh);

			printf("export\n");


			Export(fileNameOut, mesh.get(), true, &export_settings);

			// ==========================================================
			//                      DEPRECATED END
			// ==========================================================
		}
		else if (ext == "obj")
		{
			SP<ModernMesh> mesh;

			SP<OBJImport>	myObjImport = KigsCore::GetInstanceOf("objfile", "OBJImport");
			myObjImport->setValue(LABEL_TO_ID(FileName), fileNameIn);
			myObjImport->setValue(LABEL_TO_ID(ModernMesh), true);
			myObjImport->Init();
			mesh = *myObjImport->GetMeshes().begin();

			if (myParams.myImportScaleFactor != 1.0f)
			{
				mesh->ApplyScaleFactor(myParams.myImportScaleFactor);
			}

			if ((myParams.myImportFlipAxis[0] != 1) || (myParams.myImportFlipAxis[0] != 2) || (myParams.myImportFlipAxis[0] != 3))
			{
				mesh->FlipAxis(myParams.myImportFlipAxis[0], myParams.myImportFlipAxis[1], myParams.myImportFlipAxis[2]);
			}

			if (myParams.myAddVertexNormal)
			{
				mesh->ComputeNormals();
			}

			if (myParams.myAddVertexTangent)
			{
				mesh->ComputeNormals();
				mesh->ComputeTangents((myParams.myAddVertexTangent == 1) ? true : false);
			}

			Export(fileNameOut, mesh.get(), true, &export_settings);

			myObjectsToDestroy.push_back(mesh);

		}
		else if (ext == "ply")
		{
			SP<ModernMesh> mesh;

			SP<PLYImport>	myObjImport = KigsCore::GetInstanceOf("objfile", "PLYImport");
			myObjImport->setValue(LABEL_TO_ID(FileName), fileNameIn);
			myObjImport->Init();
			mesh = *myObjImport->GetMeshes().begin();

			if (myParams.myImportScaleFactor != 1.0f)
			{
				mesh->ApplyScaleFactor(myParams.myImportScaleFactor);
			}

			if ((myParams.myImportFlipAxis[0] != 1) || (myParams.myImportFlipAxis[0] != 2) || (myParams.myImportFlipAxis[0] != 3))
			{
				mesh->FlipAxis(myParams.myImportFlipAxis[0], myParams.myImportFlipAxis[1], myParams.myImportFlipAxis[2]);
			}

			if (myParams.myAddVertexNormal)
			{
				mesh->ComputeNormals();
			}

			if (myParams.myAddVertexTangent)
			{
				mesh->ComputeNormals();
				mesh->ComputeTangents((myParams.myAddVertexTangent == 1) ? true : false);
			}

			Export(fileNameOut, mesh.get(), true, &export_settings);

			myObjectsToDestroy.push_back(mesh);
		}
		else if (ext == "stl")
		{
			SP<ModernMesh> mesh;

			SP<STLImport>	myObjImport = KigsCore::GetInstanceOf("objfile", "STLImport");
			myObjImport->setValue(LABEL_TO_ID(FileName), fileNameIn);
			myObjImport->Init();
			mesh = *myObjImport->GetMeshes().begin();

			if (myParams.myImportScaleFactor != 1.0f)
			{
				mesh->ApplyScaleFactor(myParams.myImportScaleFactor);
			}

			if ((myParams.myImportFlipAxis[0] != 1) || (myParams.myImportFlipAxis[0] != 2) || (myParams.myImportFlipAxis[0] != 3))
			{
				mesh->FlipAxis(myParams.myImportFlipAxis[0], myParams.myImportFlipAxis[1], myParams.myImportFlipAxis[2]);
			}

			if (myParams.myAddVertexNormal)
			{
				mesh->ComputeNormals();
			}

			if (myParams.myAddVertexTangent)
			{
				mesh->ComputeNormals();
				mesh->ComputeTangents((myParams.myAddVertexTangent == 1) ? true : false);
			}

			Export(fileNameOut, mesh.get(), true, &export_settings);

			myObjectsToDestroy.push_back(mesh);
		}
#ifdef USE_FBXSDK
		else if (ext == "fbx")
		{
			SP<FBXImport>	parser = KigsCore::GetInstanceOf("FBXFile", "FBXImport");
			parser->setValue(LABEL_TO_ID(FileName), fileNameIn);

			parser->setParams(myParams);

			parser->Init();

			Scene = parser->getScene();

			CMSP	genericRenderer = KigsCore::GetInstanceOf("genericRenderer", "API3DGenericMeshShader");

			// add generic renderer
			Scene->addItem(genericRenderer);

			SP<Node3D> rootNode = parser->getRootNode();

			// flip axis is done directly on root matrix
			Matrix3x4 flipped(myParams.getFlipMatrix());

			Vector3D  pos = rootNode->GetLocal().Pos;
			flipped.TransformVector(&pos);
			flipped.Pos = pos;
			rootNode->ChangeMatrix(flipped);

			if (flipped.IsIdentity() && rootNode->getItems().size() == 1)
			{
				SP<Node3D> toset(rootNode->GetSon<Node3D>(), GetRefTag{});
				rootNode = toset;
			}

			kstl::vector<CMSP> instances;
			SP<Camera> SceneCamera = nullptr;
			Scene->GetSonInstancesByType("Camera", instances, true);
			if (instances.size() == 0)
			{
				SceneCamera = KigsCore::GetInstanceOf("FakeCamera", "Camera");
				SceneCamera->setValue(LABEL_TO_ID(RenderingScreen), "RenderingScreen:theRenderingScreen");
				Scene->addItem((CMSP&)SceneCamera);
				SceneCamera->Init();
			}
			else
			{
				SceneCamera = instances[0];
				SceneCamera->setValue(LABEL_TO_ID(RenderingScreen), "RenderingScreen:theRenderingScreen");
			}

			Scene->GetRef();
			theSceneGraph->addItem(Scene);
			theSceneGraph->Update(*mApplicationTimer.get(), nullptr);
			// update twice
			theSceneGraph->Update(*mApplicationTimer.get(), nullptr);
			
			if (wholeScene)
			{
				Export(fileNameOut, Scene.get(), true, &export_settings);
			}
			else
			{
				Export(fileNameOut, rootNode.get(), true, &export_settings);
			}

			printf("export %s \n", ext.data());
			Scene->Destroy();
		}
#endif
		else if (ext == "dae")
		{
			SP<ColladaParser>	parser = KigsCore::GetInstanceOf("colladaFile", "ColladaParser");
			parser->setValue(LABEL_TO_ID(FileName), fileNameIn);
			parser->setParams(myParams);
			parser->Init();

			// get scene
			Scene = parser->getScene();

			CMSP	genericRenderer = KigsCore::GetInstanceOf("genericRenderer", "API3DGenericMeshShader");

			// add generic renderer
			Scene->addItem(genericRenderer);


			SP<Node3D> rootNode = parser->getRootNode();

			// flip axis is done directly on root matrix
			Matrix3x4 flipped(myParams.getFlipMatrix());

			Vector3D  pos = rootNode->GetLocal().Pos;
			flipped.TransformVector(&pos);
			flipped.Pos = pos;

			rootNode->ChangeMatrix(flipped);

			kstl::vector<CMSP> instances;
			SP<Camera> SceneCamera = nullptr;

			Scene->GetSonInstancesByType("Camera", instances, true);
			if (instances.size() == 0)
			{
				SceneCamera = KigsCore::GetInstanceOf("FakeCamera", "Camera");
				SceneCamera->setValue(LABEL_TO_ID(RenderingScreen), "RenderingScreen:theRenderingScreen");
				Scene->addItem((CMSP&)SceneCamera);
				SceneCamera->Init();
			}
			else
			{
				SceneCamera = instances[0];
				SceneCamera->setValue(LABEL_TO_ID(RenderingScreen), "RenderingScreen:theRenderingScreen");
			}

			Scene->GetRef();

			theSceneGraph->addItem(Scene);
			theSceneGraph->Update(*mApplicationTimer.get(), nullptr);
			// update twice
			theSceneGraph->Update(*mApplicationTimer.get(), nullptr);

			if (wholeScene)
			{
				Export(fileNameOut, Scene.get(), true, &export_settings);
			}
			else
			{
				// export with sons
				Export(fileNameOut, rootNode.get(), true, &export_settings);
			}
			printf("export %s \n", ext.data());
			Scene->Destroy();
		}

	}while (0);// pseudo loop for exit management

	mNeedExit = true;
}

//-------------------------------------------------------------------------
//ProtectedUpdate

void	MeshConvert::ProtectedUpdate()
{
}

//-------------------------------------------------------------------------
//ProtectedClose

void	MeshConvert::ProtectedClose()
{
	CoreDestroyModule(CollisionModule);
	CoreDestroyModule(GenericAnimationModule);

	CoreDestroyModule(ModuleSceneGraph);
	CoreDestroyModule(ModuleRenderer);
	CoreDestroyModule(ModuleGUI);

}

bool	MeshConvert::ProtectedExternAskExit()
{
	mNeedExit = true;
	return true;
}