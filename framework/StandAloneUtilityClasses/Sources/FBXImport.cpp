#ifdef USE_FBXSDK

#include "FBXImport.h"
#include "ModernMesh.h"
#include "Texture.h"
#include "Camera.h"
#include "OpenGLMaterial.h"
#include "TextureFileManager.h"
#include "Bones/AObjectSkeletonResource.h"
#include "Bones/APRSKeyStream.h"

#include <queue>
#include <fstream>

using namespace Kigs::Utils;

IMPLEMENT_CLASS_INFO(FBXImport)

FBXImport::FBXImport(const std::string& name, CLASS_NAME_TREE_ARG) : Base3DImporter(name, PASS_CLASS_NAME_TREE_ARG)
, m_FileName(*this, true, "FileName")
, m_ExportPath(*this, true, "ExportPath", "")
{
	nbrUniqueMesh = 0;
	nbrInstanceMesh = 0;
	nbrVertex = 0;
	nbrTriangle = 0;
}

FBXImport::~FBXImport()
{

}

void	FBXImport::InitModifiable()
{
	Base3DImporter::InitModifiable();
	if (_isInit)
	{

		FbxManager* lSdkManager = FbxManager::Create();
		FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		m_Importer = FbxImporter::Create(lSdkManager, "");

		if (!m_Importer->Initialize(m_FileName.c_str(), -1, lSdkManager->GetIOSettings())) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", m_Importer->GetStatus().GetErrorString());
			exit(0);
		}

		printf("Start Import \n");
		m_Scene = FbxScene::Create(lSdkManager, "myScene");
		myScene = KigsCore::GetInstanceOf(m_Scene->GetName(), "Scene3D");
		myScene->Init();
		m_Importer->Import(m_Scene);
		printf("End Import\n");

		
		printf("Start Triangulation \n");
		FbxGeometryConverter clsConverter(lSdkManager);
		clsConverter.Triangulate(m_Scene, true);
		if (m_Scene == nullptr)
		{
			printf("Error in the triangulation\n");
			exit(0);
		}

		printf("End Triangulation \n");

		clsConverter.SplitMeshesPerMaterial(m_Scene, true);

		FbxNode* lRootNode = m_Scene->GetRootNode();

		RootNode = ParseNode(lRootNode);
		RootNode->Init();
		myScene->addItem((CMSP&)RootNode);
		RootNode->NeedLocalToGlobalMatrixUpdate();

		m_Importer->Destroy();
		printf("nbr triangle: %d\n", nbrTriangle);
		printf("nbr vertex: %d\n", nbrVertex);
		printf("nbr uniqueMesh: %d\n", nbrUniqueMesh);
		printf("nbr instanceMesh: %d\n", nbrInstanceMesh);

		ios->Destroy();
		lSdkManager->Destroy();
		return;

	}
	Base3DImporter::UninitModifiable();

}

enum class ColliderType
{
	None,
	Box,
	Sphere,
	Capsule,
	Cylinder
};


CMSP	FBXImport::ParseNode(FbxNode* fatherNode)
{
	SP<Node3D> myNode = KigsCore::GetInstanceOf(fatherNode->GetName(), "Node3D");

	int numMesh = 0;
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();

	const FbxVector4 lT = fatherNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = fatherNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = fatherNode->GetGeometricScaling(FbxNode::eSourcePivot);
	matrixGeo.SetT(lT);
	matrixGeo.SetR(lR);
	matrixGeo.SetS(lS);

	FbxAMatrix globalMatrix = fatherNode->EvaluateLocalTransform();

	FbxAMatrix matrix = globalMatrix*matrixGeo;

	Matrix3x4 transform;
	transform.SetIdentity();

	for (auto i = 0; i < 4; i++)
	{
		for (auto j = 0; j < 3; j++) 
		{
			transform.e[i][j] = matrix[i][j];
		}
	}

	

	if (myConvertParams.myImportScaleFactor != 1.0f)
	{
		// apply scale on position only
		transform.Pos *= myConvertParams.myImportScaleFactor;
	}

	myNode->ChangeMatrix(transform);
	myNode->ComputeLocalToGlobal();

	auto visibility_prop = fatherNode->FindProperty("Visibility");
	if(visibility_prop.IsValid())
		myNode->setValue("Show", fatherNode->GetVisibility());

	ColliderType collider_type = ColliderType::None;
	bool no_mesh = false;
	auto user_prop = fatherNode->FindProperty("UDP3DSMAX");
	if (user_prop.IsValid())
	{
		std::string user_props = user_prop.Get<FbxString>().Buffer();
	

		auto vec = SplitStringByCharacter(user_props, '\n');

		for (auto& s : vec)
		{
			auto equal_at = s.find_first_of('=');
			auto name = s.substr(0, equal_at);
			auto value = equal_at == std::string::npos ? "" : s.substr(equal_at+1);
			if (value.size() && value.back() == '\r') value.pop_back();
			
			if (myNode->getAttribute(name)) myNode->setValue(name, value);
			else
			{
				if (name == "NoMesh")
				{
					if(value.empty() || value == "true")
					no_mesh = true;
				}
				else if (name == "ColliderType")
				{
					if (value == "Box") collider_type = ColliderType::Box;
					else if (value == "Capsule") collider_type = ColliderType::Capsule;
					else if (value == "Cylinder") collider_type = ColliderType::Cylinder;
					else if (value == "Sphere") collider_type = ColliderType::Sphere;
					else
					{
						printf("Unknown collider type: %s\n", value.c_str());
						collider_type = ColliderType::Box;
					}
				}
				else
				{
					auto type = CoreModifiable::ATTRIBUTE_TYPE::STRING;
					if (name == "CollideMask")
						type = CoreModifiable::ATTRIBUTE_TYPE::UINT;
					myNode->AddDynamicAttribute(type, name, value.c_str());
				}
			}

		}
	}

	for (auto m = 0; m < fatherNode->GetMaterialCount(); m++)
	{
		ParseMaterial(fatherNode->GetMaterial(m));
	}

	for (auto i = 0; i < fatherNode->GetNodeAttributeCount(); i++)
	{
		CMSP toAdd=ParseAttribute(fatherNode->GetNodeAttributeByIndex(i), fatherNode, numMesh);
		if (toAdd != nullptr)
		{
			if (toAdd->isSubType("Light")) // keep the same orientation as collada
			{
				transform.PreRotateX(-PI / 2.0f);
				myNode->ChangeMatrix(transform);
				myNode->ComputeLocalToGlobal();
			}
			else if(toAdd->isSubType("Camera")) // keep the same orientation as collade
			{
				transform.PreRotateY(-PI / 2.0f);
				myNode->ChangeMatrix(transform);
				myNode->ComputeLocalToGlobal();
			}
			myNode->addItem(toAdd);
		}
	}
	for (std::multimap<Material*, ModernMesh*>::iterator it = sortedMeshes.begin(); it != sortedMeshes.end(); ++it)
	{
		CMSP toAdd(it->second, GetRefTag{});
		myNode->addItem(toAdd);
		it->second = nullptr;
	}

	sortedMeshes.clear();

	for (int i = 0; i < fatherNode->GetChildCount(); i++)
	{
		if (fatherNode->GetChild(i) != nullptr)
		{
			CMSP toAdd = ParseNode(fatherNode->GetChild(i));
			myNode->addItem(toAdd);
		}
	}
	myNode->Init();

	if (collider_type != ColliderType::None)
	{
		auto bbox = myNode->GetLocalBoundingBox();
		if (collider_type == ColliderType::Sphere)
		{
			auto sphere_collider = KigsCore::GetInstanceOf("collider", "BSphere");
			v3f pts[8];
			bbox.ConvertToPoint(pts);
			float maxdist = 0.0f;
			for (int i = 0; i < 8; ++i)
			{
				float dist = NormSquare(pts[i]);
				if (dist > maxdist)
					maxdist = dist;
			}
			sphere_collider->setValue("Radius", sqrtf(maxdist));
			sphere_collider->Init();
			myNode->addItem(sphere_collider);
		}
		else if (collider_type == ColliderType::Box)
		{
			auto box_collider = KigsCore::GetInstanceOf("collider", "BoxCollider");

			box_collider->setValue("Size", bbox.Size());
			if (bbox.Center() != v3f(0, 0, 0))
			{
				auto box_collider_center = KigsCore::GetInstanceOf("collider_offset", "Node3D");
				mat3x4 m = mat3x4::IdentityMatrix();
				m.SetTranslation(bbox.Center());
				box_collider_center->as<Node3D>()->ChangeMatrix(m);
				box_collider->Init();
				box_collider_center->addItem(box_collider);
				myNode->addItem(box_collider_center);
			}
			else
			{
				box_collider->Init();
				myNode->addItem(box_collider);
			}
		}
	}

	if (no_mesh)
	{
		auto copy = myNode->getItems();
		for (auto it : copy)
		{
			if (it.mItem->isSubType("ModernMesh")) 
				myNode->removeItem(it.mItem);
		}
	}


	return myNode;
}

CMSP	FBXImport::ParseAttribute(FbxNodeAttribute* attribute, FbxNode* fatherNode, int& numMesh)
{
	switch (attribute->GetAttributeType()) 
	{
	case FbxNodeAttribute::eUnknown:
		printf("Unknow Node Attribute %d\n", attribute->GetAttributeType());
		return nullptr;
	case FbxNodeAttribute::eNull:
		printf("Empty Node Attribute %d\n", attribute->GetAttributeType());
		return nullptr;
	case FbxNodeAttribute::eMesh:
		return ParseMesh(static_cast<FbxMesh*>(attribute), fatherNode, numMesh);
	case FbxNodeAttribute::eCamera:
		return ParseCamera((FbxCamera*)attribute);
	case FbxNodeAttribute::eLight:
		return  ParseLight(static_cast<FbxLight*>(attribute));
	default:
		printf("Unparsed attribute %d (TO DO) \n", attribute->GetAttributeType());
		return nullptr;
	}
}

FBXImport::SkinController	FBXImport::ParseSkinInfos(FbxSkin* pSkin,FbxMesh* pMesh)
{
	SkinController result;
	int totalPointCount=pMesh->GetControlPointsCount();

	struct tmpW
	{
		float totalW = 0.0f;
		// bone index and weight vector
		std::vector<std::pair<int,float> >	w;
	};

	std::vector<tmpW>	allW;
	allW.resize(totalPointCount);

	// bone count
	int ncBones = pSkin->GetClusterCount();

	SP<AObjectSkeletonResource> skeleton_resource(nullptr);

	skeleton_resource = KigsCore::GetInstanceOf("skeleton_resource", "AObjectSkeletonResource");
	// Init skeleton resource
	skeleton_resource->initSkeleton(ncBones, sizeof(PRSKey));

	// construct boneslist
	for (int boneIndex = 0; boneIndex < ncBones; ++boneIndex)
	{
		// cluster
		FbxCluster* cluster = pSkin->GetCluster(boneIndex);
		// bone ref
		FbxNode* pBone = cluster->GetLink();
		result.boneList.push_back(pBone);
	}
	// construct skeleton and weights
	for (int boneIndex = 0; boneIndex < ncBones; ++boneIndex)
	{
		FbxCluster* cluster = pSkin->GetCluster(boneIndex);
		std::string boneName = result.boneList[boneIndex]->GetName();
		unsigned int uid = CharToID::GetID(boneName);

		// Get the bind pose
		FbxAMatrix bindPoseMatrix, transformMatrix;
		cluster->GetTransformMatrix(transformMatrix);
		cluster->GetTransformLinkMatrix(bindPoseMatrix);

		int* pVertexIndices = cluster->GetControlPointIndices();
		double* pVertexWeights = cluster->GetControlPointWeights();

		// Iterate through all the vertices, which are affected by the bone
		int ncVertexIndices = cluster->GetControlPointIndicesCount();

		for (int iBoneVertexIndex = 0; iBoneVertexIndex < ncVertexIndices; iBoneVertexIndex++)
		{
			// vertex
			int niVertex = pVertexIndices[iBoneVertexIndex];
			// weight
			float fWeight = (float)pVertexWeights[iBoneVertexIndex];

			allW[niVertex].totalW += fWeight;
			allW[niVertex].w.push_back({ boneIndex ,fWeight });
		}
	
		// search parent bone
		FbxNode* pBone = result.boneList[boneIndex]->GetParent();
		int parent_id = 0;
		for (int pindex=0;pindex< result.boneList.size(); pindex++)
		{
			if (pBone == result.boneList[pindex])
			{
				parent_id = pindex+1;
			}
		}

		Matrix3x4 joints_inv_bind_matrix, transform;
		joints_inv_bind_matrix.SetIdentity();
		transform.SetIdentity();


		for (auto i = 0; i < 4; i++)
		{
			for (auto j = 0; j < 3; j++)
			{
				joints_inv_bind_matrix.e[i][j] = bindPoseMatrix[i][j];
				transform.e[i][j] = transformMatrix[i][j];

			}
		}

		skeleton_resource->addBone(boneIndex, uid, boneIndex+1, parent_id, joints_inv_bind_matrix);

		reinterpret_cast<PRSKey*>(skeleton_resource->getStandData(boneIndex))->set(transform);
		reinterpret_cast<PRSKey*>(skeleton_resource->getStandData(boneIndex))->m_RotationKey.Normalize();

	}

	// Export skeleton resource (.SKL)
	skeleton_resource->setValue("SkeletonFileName", "TestSkeleton.skl");
	skeleton_resource->Export();

	// copy in return struct after limiting to 4 w per vertices

	result.weights.resize(allW.size());
	int k = 0;
	for (auto& w : allW)
	{
		for(int wresize=w.w.size();wresize < 4;wresize++)
		{
			w.w.push_back({ w.w[0].first , 0.0f });
		}

		if (w.w.size() > 4)
		{
			std::sort(w.w.begin(), w.w.end(),
				[](const std::pair<int, float>& a, const std::pair<int, float>& b) { return a.second > b.second; });
			w.w.resize(4);
			w.totalW = 0.0f;
			for (auto& p : w.w)
				w.totalW += p.second;
		}

		for (int j = 0; j < 4; ++j)
		{
			result.weights[k].index[j] = w.w[j].first;
			result.weights[k].weight[j] = (unsigned char)(0.5f + 255.0f * (w.w[j].second / w.totalW));
		}
		k++;
	}

	return result;
}

void	FBXImport::ParseAnimations(FBXImport::SkinController& skin)
{
	int numStacks = m_Scene->GetSrcObjectCount<FbxAnimStack>();

	for (int i = 0; i < numStacks; ++i)
	{
		FbxAnimStack* lAnimStack = m_Scene->GetSrcObject<FbxAnimStack>(i);
		int numLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();

		for (int j = 0; j < numLayers; ++j)
		{
			FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(j);

			FbxString layerName = "Animation Stack Name: ";
			layerName += lAnimLayer->GetName();
			std::string sLayerName = layerName.Buffer();


			for(auto n: skin.boneList)
			{
				FbxAnimCurve* lAnimCurve = n->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);

				if (lAnimCurve != NULL)
				{
					// TODO
					FbxTakeInfo* lTakeInfo = m_Importer->GetTakeInfo(0);
					FbxTime start = lTakeInfo->mLocalTimeSpan.GetStart();
					FbxTime end = lTakeInfo->mLocalTimeSpan.GetStop();
					
					int kc=lAnimCurve->KeyGetCount();
					for (i = 0; i < kc; i++)
					{
						auto key=lAnimCurve->KeyGet(i);
						FbxTime ktime=key.GetTime();

					}
				}
			}
		}

	}
}

CMSP	FBXImport::ParseMesh(FbxMesh* pMesh, FbxNode* myNode, int &numMesh)
{
	char value[33];
	sprintf_s(value, "%d", nbrInstanceMesh);
	std::string name = value;
	name = pMesh->GetName();
	name += "_ModernMesh";
	name += value;
	printf("New mesh Parsing... %s\n", name.c_str());
	nbrInstanceMesh++;
	if (meshParsed.find(pMesh) != meshParsed.end())
	{
		Material* myMat=(materialOfMesh.find(meshParsed.find(pMesh)->second))->second;
		sortedMeshes.insert(std::pair<Material*, ModernMesh*>(myMat, static_cast<ModernMesh*>(meshParsed.find(pMesh)->second)));
		return nullptr;
	}

	SP<ModernMesh> newmesh = KigsCore::GetInstanceOf(name.c_str(), "ModernMesh");
	meshParsed[pMesh] = newmesh.get();
	newmesh->StartMeshBuilder();
	nbrUniqueMesh++;
	int structSize = 0;

	SP<Material> myMaterial = nullptr;
	bool	  hasNormalMap = false;
	bool	  hasTexture = false;
	if (myNode->GetMaterial(numMesh) != nullptr)
	{
		std::map<std::string, MaterialStruct>::iterator matfound = m_materialList.find(myNode->GetMaterial(numMesh)->GetNameOnly().Buffer());
		if (matfound != m_materialList.end())
		{
			myMaterial = (*matfound).second.kigsMaterial;
			hasNormalMap = (*matfound).second.hasNormalMap;

			// check if texture are available
			std::vector<CMSP>	instances;
			myMaterial->GetSonInstancesByType("Texture", instances, true);
			if (instances.size())
			{
				hasTexture = true;
			}
		}
	}
	else
	{
		//printf("no material for mesh %d\n",numMesh);
		//myMaterial=static_cast<Material*>(KigsCore::GetInstanceOf("emptyMat", "Material"));
	}


	CoreItemSP	description = CoreItemSP::getCoreVector();
	CoreItemSP	vertices = CoreItemSP::getCoreItemOfType<CoreNamedVector>("vertices");
	description->set("", vertices);

	structSize += 3 * sizeof(float);


	// check for skin
	FBXImport::SkinController skinstruct;
	int dcount = pMesh->GetDeformerCount();
	if (dcount)
	{
		for (int i = 0; i < dcount; ++i)
		{
			FbxSkin* pSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);
			if (pSkin)
			{
				skinstruct = ParseSkinInfos(pSkin, pMesh);
				ParseAnimations(skinstruct);
			}
		}
	}

	

	nbrVertex +=pMesh->GetControlPointsCount();
	bool hasNormal = pMesh->GetElementNormalCount() > 0;
	bool hasTextureCoord = pMesh->GetElementUVCount() > 0;
	bool hasColor = pMesh->GetElementVertexColorCount() >0;

	if (hasColor)
	{
		CoreItemSP	colors = CoreItemSP::getCoreItemOfType<CoreNamedVector>("colors");
		description->set("", colors);
		structSize += 4 * sizeof(float);
	}
	if (hasNormal)
	{
		CoreItemSP	normal = CoreItemSP::getCoreItemOfType<CoreNamedVector>("normals");
		description->set("", normal);
		structSize += 3 * sizeof(float);
	}
	FbxStringList lUVNames;

	if (hasTextureCoord && hasTexture)
	{
		CoreItemSP	texCoords = CoreItemSP::getCoreItemOfType<CoreNamedVector>("texCoords");
		description->set("", texCoords);
		structSize += 2 * sizeof(float);
		pMesh->GetUVSetNames(lUVNames);
	}

	FbxGeometryElementVertexColor* colorElem = pMesh->GetElementVertexColor(0);

	if (colorElem)
	{
		int testControlPointCount=pMesh->GetControlPointsCount();
		printf("Mesh Control Point Count %d\n", testControlPointCount);

		int testColorCount  =colorElem->GetDirectArray().GetCount();
		int testIndexCount = colorElem->GetIndexArray().GetCount();
		printf("Color array size %d\n", testColorCount);
		printf("Color array index size %d\n", testIndexCount);

	}
	unsigned char* v[3];
	v[0] = new unsigned char[structSize];
	v[1] = new unsigned char[structSize];
	v[2] = new unsigned char[structSize];

	newmesh->StartMeshGroup((CoreVector*)description.get());

	nbrTriangle += pMesh->GetPolygonCount();

	int colorIndex = 0;

	for (int i = 0; i < pMesh->GetPolygonCount(); i++)
	{

		std::vector<IndexTrio>	indexlist;

		for (int j = 0; j < pMesh->GetPolygonSize(i); j++)
		{
			IndexTrio toAdd;
			int indiceVertex = pMesh->GetPolygonVertex(i, j);


			toAdd.posVertex = pMesh->GetControlPointAt(indiceVertex);

			if (hasColor)
			{
				if (colorElem)
				{
					switch (colorElem->GetMappingMode())
					{
					case FbxLayerElement::eByPolygonVertex:
					{

						switch (colorElem->GetReferenceMode())
						{
							case FbxGeometryElement::eDirect:
							{
								toAdd.color = colorElem->GetDirectArray().GetAt(indiceVertex);

								
							}
							break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int id = colorElem->GetIndexArray().GetAt(colorIndex);
								toAdd.color = colorElem->GetDirectArray().GetAt(id);

							}
							break;
						default:
							break;
						}

					}
					break;
					}
				}
				else
				{
					toAdd.color.Set(1.0, 1.0, 1.0);
				}
				colorIndex++;
			}
			if (hasNormal)
			{
				pMesh->GetPolygonVertexNormal(i, j, toAdd.posNormal);

			}
			if (hasTextureCoord && hasTexture)
			{
				bool unmapped;
				pMesh->GetPolygonVertexUV(i, j, lUVNames[0], toAdd.posTexture, unmapped);
			}


			indexlist.push_back(toAdd);
		}

		int copysize = 3 * sizeof(float);
		int decal = 0;

		if (indexlist.size() == 3)
		{
			Point3D vertex1;
			vertex1.x = indexlist.at(0).posVertex[0];
			vertex1.y = indexlist.at(0).posVertex[1];
			vertex1.z = indexlist.at(0).posVertex[2];

			Point3D vertex2;
			vertex2.x = indexlist.at(1).posVertex[0];
			vertex2.y = indexlist.at(1).posVertex[1];
			vertex2.z = indexlist.at(1).posVertex[2];

			Point3D vertex3;
			vertex3.x = indexlist.at(2).posVertex[0];
			vertex3.y = indexlist.at(2).posVertex[1];
			vertex3.z = indexlist.at(2).posVertex[2];

			memcpy(v[0], &vertex1, copysize);
			memcpy(v[1], &vertex2, copysize);
			memcpy(v[2], &vertex3, copysize);

			if (hasColor)
			{
				decal += copysize;
				copysize = 4 * sizeof(float);

				Vector4D vertexColor1;
				vertexColor1.x = indexlist.at(0).color[0];
				vertexColor1.y = indexlist.at(0).color[1];
				vertexColor1.z = indexlist.at(0).color[2];
				vertexColor1.w = indexlist.at(0).color[3];

				Vector4D vertexColor2;
				vertexColor2.x = indexlist.at(1).color[0];
				vertexColor2.y = indexlist.at(1).color[1];
				vertexColor2.z = indexlist.at(1).color[2];
				vertexColor2.w = indexlist.at(1).color[3];

				Vector4D vertexColor3;
				vertexColor3.x = indexlist.at(2).color[0];
				vertexColor3.y = indexlist.at(2).color[1];
				vertexColor3.z = indexlist.at(2).color[2];
				vertexColor3.w = indexlist.at(2).color[3];

				memcpy(v[0] + decal, &vertexColor1, copysize);
				memcpy(v[1] + decal, &vertexColor2, copysize);
				memcpy(v[2] + decal, &vertexColor3, copysize);
			}

			if (hasNormal)
			{
				decal += copysize;
				copysize = 3 * sizeof(float);

				Point3D vertexNormal1;
				vertexNormal1.x = indexlist.at(0).posNormal[0];
				vertexNormal1.y = indexlist.at(0).posNormal[1];
				vertexNormal1.z = indexlist.at(0).posNormal[2];

				Point3D vertexNormal2;
				vertexNormal2.x = indexlist.at(1).posNormal[0];
				vertexNormal2.y = indexlist.at(1).posNormal[1];
				vertexNormal2.z = indexlist.at(1).posNormal[2];

				Point3D vertexNormal3;
				vertexNormal3.x = indexlist.at(2).posNormal[0];
				vertexNormal3.y = indexlist.at(2).posNormal[1];
				vertexNormal3.z = indexlist.at(2).posNormal[2];

				memcpy(v[0] + decal, &vertexNormal1, copysize);
				memcpy(v[1] + decal, &vertexNormal2, copysize);
				memcpy(v[2] + decal, &vertexNormal3, copysize);

			}

			if (hasTextureCoord && hasTexture)
			{
				decal += copysize;
				copysize = 2 * sizeof(float);

				Point2D vertexTexCoord1;
				vertexTexCoord1.x = indexlist.at(0).posTexture[0];
				vertexTexCoord1.y = 1- indexlist.at(0).posTexture[1];

				Point2D vertexTexCoord2;
				vertexTexCoord2.x = indexlist.at(1).posTexture[0];
				vertexTexCoord2.y = 1- indexlist.at(1).posTexture[1];

				Point2D vertexTexCoord3;
				vertexTexCoord3.x = indexlist.at(2).posTexture[0];
				vertexTexCoord3.y = 1- indexlist.at(2).posTexture[1];

				memcpy(v[0] + decal, &vertexTexCoord1, copysize);
				memcpy(v[1] + decal, &vertexTexCoord2, copysize);
				memcpy(v[2] + decal, &vertexTexCoord3, copysize);
			}

		}
		else
		{
			printf("Error on the triangulation\n");
			exit(0);
		}
		newmesh->AddTriangle(v[0], v[1], v[2]);

	}


	delete[] v[0];
	delete[] v[1];
	delete[] v[2];
	newmesh->setValue("Optimize", true);

	SP<ModernMeshItemGroup> created = newmesh->EndMeshGroup();
	
	numMesh++;

	if (created)
	{
		if (!hasNormal && myConvertParams.myAddVertexNormal)
		{
			created->ComputeNormals();
		}

		if(hasNormalMap || myConvertParams.myAddVertexTangent)
			created->ComputeTangents((myConvertParams.myAddVertexTangent == 1) ? true : false);
		
		if(myMaterial) created->addItem((CMSP&)myMaterial);
	}
	newmesh->EndMeshBuilder();

	newmesh->Init();

	if (myConvertParams.myImportScaleFactor != 1.0f)
	{
		newmesh->ApplyScaleFactor(myConvertParams.myImportScaleFactor);
	}

	m_MeshList.push_back(newmesh);
	materialOfMesh[newmesh.get()]= myMaterial.get();
	sortedMeshes.insert(std::pair<Material*, ModernMesh*>(myMaterial.get(), newmesh.get()));

	printf("Done\n");

	return nullptr;
}

void FBXImport::ParseMaterial(FbxSurfaceMaterial* material)
{
	std::map<std::string, MaterialStruct>::iterator matfound=m_materialList.find(material->GetNameOnly().Buffer());

	if (matfound != m_materialList.end())
		return;

	std::string materialName = material->GetNameOnly().Buffer();

	SP<Material> lmaterial = KigsCore::GetInstanceOf(materialName.c_str(), "Material");

	FbxDouble3 lResult;

	std::string textureName = "";

	lResult = ParseMaterialProperty(material, material->sDiffuse, material->sDiffuseFactor, textureName);
	lmaterial->SetDiffuseColor(lResult[0], lResult[1], lResult[2]);
	ImportTexture(lmaterial.get(), textureName);
	textureName = "";

	lResult = ParseMaterialProperty(material, material->sAmbient, material->sAmbientFactor, textureName);
	lmaterial->SetAmbientColor(lResult[0], lResult[1], lResult[2]);
	//ImportTexture(myMaterial, textureName);
	//textureName = "";

	lResult = ParseMaterialProperty(material, material->sEmissive, material->sEmissiveFactor, textureName);
	lmaterial->SetEmissionColor(lResult[0], lResult[1], lResult[2]);
	//ImportTexture(myMaterial, textureName);
	//textureName = "";

	lResult = ParseMaterialProperty(material, material->sSpecular, material->sSpecularFactor, textureName);
	lmaterial->SetSpecularColor(lResult[0], lResult[1], lResult[2]);
	//ImportTexture(myMaterial, textureName);
	//textureName = "";

	MaterialStruct	toAdd;
	toAdd.hasNormalMap = false;


	FbxProperty prop = material->FindProperty(material->sBump);
	if (prop.GetSrcObjectCount<FbxFileTexture>() > 0)
	{
		FbxFileTexture* layeredTexture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxFileTexture>(0));
		textureName = layeredTexture->GetFileName();
		std::string shortname = "";
		std::string voidString = "";
		std::string extentionString = "";
		RetreiveShortNameAndExt(textureName, shortname, extentionString, voidString);
		ImportTexture(lmaterial.get(), shortname + "." + extentionString, 1);
		toAdd.hasNormalMap = true;
	}

	prop=material->FindProperty(material->sNormalMap);
	if (prop.GetSrcObjectCount<FbxFileTexture>() > 0)
	{
		FbxFileTexture* layeredTexture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxFileTexture>(0));
		textureName = layeredTexture->GetFileName();
		std::string shortname = "";
		std::string voidString = "";
		std::string extentionString = "";
		RetreiveShortNameAndExt(textureName, shortname, extentionString, voidString);
		ImportTexture(lmaterial.get(), shortname+"."+extentionString, 1);
		toAdd.hasNormalMap = true;
	}


	FbxDouble shininess = material->FindProperty(material->sShininess).Get<FbxDouble>();
	// prevent shininess = -inf
	if( (shininess > 100000) || (shininess < -100000) )
	{
		printf("shininess out of bound = %f\n", shininess);
		shininess = 1;
	}
	lmaterial->setValue("Shininess", shininess);

	FbxDouble transparency = material->FindProperty(material->sTransparencyFactor).Get<FbxDouble>();
	lmaterial->setValue("Transparency", transparency);
	

	toAdd.kigsMaterial = lmaterial;

	m_materialList[material->GetNameOnly().Buffer()] = toAdd;

}

void FBXImport::ImportTexture(Material* material, std::string textureName, int texture_channel)
{
	if (textureName != "")
	{

		m_TextureList.push_back(textureName);

		// retreive material sons

		int stageCount=material->getItems().size();
		char charstagecount[33];

		sprintf(charstagecount, "%d", stageCount+1);

		SP<MaterialStage> MatStage = KigsCore::GetInstanceOf((material->getName() + std::string("_MatStage") + charstagecount).c_str(), "MaterialStage");
		if (textureName != "")
		{
			SP<TextureFileManager>	fileManager = KigsCore::GetSingleton("TextureFileManager");
			SP<Texture> Tex = fileManager->GetTexture(textureName, false);
			Tex->setValue("ForcePow2", true);
			Tex->setValue("HasMipmap", true);
			Tex->Init();

			material->setValue("BlendEnabled", true);

			MatStage->addItem((CMSP&)Tex);
			MatStage->setValue("TextureChannel", texture_channel);
			
		}

		MatStage->Init();
		material->addItem((CMSP&)MatStage);
	
		//MatStage = nullptr;
	}
}


// Get specific property value and connected texture if any.
// Value = Property value * Factor property value (if no factor property, multiply by 1).
FbxDouble3 FBXImport::ParseMaterialProperty(const FbxSurfaceMaterial * pMaterial, const char * pPropertyName, const char * pFactorPropertyName, std::string & pTextureName)
{
	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
	if (lProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
	}
	if (lProperty.IsValid() && lFactorProperty.IsValid())
	{
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if (lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}
	}
	if (lProperty.IsValid())
	{
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		if (lTextureCount)
		{
			FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>();
			if (lTexture)
			{
				pTextureName = lTexture->GetRelativeFileName();
				std::string shortname = "";
				std::string voidString="";
				std::string extentionString = "";
				RetreiveShortNameAndExt(pTextureName, shortname, extentionString, voidString);
				pTextureName = shortname + "." + extentionString;
				printf("Texture: %s\n", pTextureName.c_str());
			}
		}
	}
	return lResult;
}


CMSP FBXImport::ParseCamera(FbxCamera* fbxcamera)
{
	SP<Camera> myCamera = KigsCore::GetInstanceOf(fbxcamera->GetName(), "Camera");
	/*FbxDouble3 readProperty3 = fbxcamera->Position.Get();
	myCamera->SetPosition(readProperty3[0], readProperty3[1], readProperty3[2]);
	readProperty3 = fbxcamera->UpVector.Get();
	myCamera->SetUpVector(readProperty3[0], readProperty3[1], readProperty3[2]);
	readProperty3 = fbxcamera->BackgroundColor.Get();
	myCamera->SetClearColor(readProperty3[0], readProperty3[1], readProperty3[2]);*/
	FbxDouble readPropertyDouble = fbxcamera->FieldOfViewY.Get();
	myCamera->SetUpVector(0, 1, 0);
	myCamera->SetViewVector(0, 0, -1.0f);
	myCamera->setValue("VerticalFOV", readPropertyDouble);
	myCamera->SetNearAndFar(fbxcamera->NearPlane.Get(), fbxcamera->FarPlane.Get());
	return myCamera;
}


//Finish with: http://help.autodesk.com/view/FBX/2016/ENU/?guid=__cpp_ref_class_fbx_light_html
CMSP FBXImport::ParseLight(FbxLight* fbxlight)
{

	CMSP myLight = KigsCore::GetInstanceOf(fbxlight->GetName(), "Light");
	
	FbxDouble3 color = fbxlight->Color.Get();
	myLight->setArrayValue("DiffuseColor", color[0], color[1], color[2]);
	myLight->setArrayValue("SpecularColor", color[0], color[1], color[2]);

	printf("%s: %f,%f,%f\n", fbxlight->GetName(), color[0], color[1], color[2]);
	
	switch (fbxlight->LightType.Get())
	{
	case FbxLight::eDirectional:
		myLight->setValue("LightType", "DIRECTIONAL");
		break;
	case FbxLight::ePoint:
		myLight->setValue("LightType", "POINT");
		break;
	case FbxLight::eSpot:
		myLight->setValue("LightType", "SPOT");
		myLight->setValue("SpotCutOff", fbxlight->OuterAngle.Get()*PI/360.0f);
		break;
	default:
		break;
	}

	switch (fbxlight->DecayType.Get())
	{
	case FbxLight::eLinear:
		myLight->setValue("LinAttenuation", 1);
		break;
	case FbxLight::eQuadratic:
	case FbxLight::eCubic:
		myLight->setValue("quadAttenuation", 1);
		break;
	default:
		break;
	}
	myLight->Init();
	return myLight;
}

void	FBXImport::RetreiveShortNameAndExt(const std::string& filename, std::string& shortname, std::string& fileext, std::string& filepath)
{
	int pos = static_cast<int>(filename.rfind("/")) + 1;
	int pos1 = static_cast<int>(filename.rfind("\\")) + 1;

	if (pos1 > pos)
	{
		pos = pos1;
	}

	bool	filenameIsShortfilename = false;
	filepath = filename.substr(0, pos);
	if (pos == 0)
	{
		shortname = filename;
		filenameIsShortfilename = true;
	}
	else
	{
		shortname = filename.substr(static_cast<unsigned int>(pos), filename.length() - pos);
	}

	fileext = "";
	pos = static_cast<int>(shortname.rfind(".")) + 1;
	if (pos)
	{
		fileext.append(shortname, static_cast<unsigned int>(pos), shortname.length() - pos);
		shortname = shortname.substr(0, pos - 1);
	}

	// lower case fileext
	std::transform(fileext.begin(), fileext.end(), fileext.begin(), ::tolower);
}

#endif // USE_FBXSDK