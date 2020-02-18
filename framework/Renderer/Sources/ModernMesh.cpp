#include "PrecompiledHeaders.h"

#include <stdlib.h>
#include "ModernMeshBuilder.h"
#include "ModernMesh.h"
#include "BinMeshLoader.h"
#ifdef WIN32
#include "STLMeshLoader.h"
#include "ASEMeshLoader.h"
#endif
#include "FilePathManager.h"
#include "Core.h"
#include "Material.h"
#include "CoreVector.h"
#include "AsciiParserUtils.h"
#include "CoreValue.h"
#include "CoreMap.h"
#include "CoreItem.h"
#include "DynamicGrowingBuffer.h"
#include "NotificationCenter.h"
#include "ModuleSceneGraph.h"

#include "CorePackage.h"
//#include "BinarySerializer.h"
#include "Crc32.h"

#include "AttributePacking.h"

#include <algorithm>

#ifdef KIGS_TOOLS
#ifdef WIN32
#include "meow_hash/meow_intrinsics.h"
#include "meow_hash/meow_hash.h"
#endif
#endif

IMPLEMENT_CLASS_INFO(ModernMeshItemGroup);
IMPLEMENT_CLASS_INFO(ModernMesh);

IMPLEMENT_CONSTRUCTOR(ModernMesh)
{
	mSortable = true;
	mRenderPassMask = 1;
}

ModernMesh::~ModernMesh()
{
	delete myCurrentMeshBuilder;
}

IMPLEMENT_CONSTRUCTOR(ModernMeshItemGroup)
{
	KigsCore::GetNotificationCenter()->addObserver(this, "ResetContext", "ResetContext");
};

ModernMeshItemGroup::~ModernMeshItemGroup()
{
	if (myVertexBuffer != -1 && myOwnedBuffer)
	{
		ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
		scenegraph->AddDefferedItem((void*)myVertexBuffer, DefferedAction::DESTROY_BUFFER);
		scenegraph->AddDefferedItem((void*)myIndexBuffer, DefferedAction::DESTROY_BUFFER);
	}
}

SP<ModernMesh> ModernMesh::CreateClonedMesh(const std::string& name, bool reuse_materials)
{
	SP<ModernMesh> mesh = KigsCore::GetInstanceOf(name, "ModernMesh");
	mesh->myBoundingBox = myBoundingBox;
	mesh->myWasBuild = true;
	mesh->Init();
	for (auto& it : getItems())
	{
		SP<ModernMeshItemGroup> mesh_group = KigsCore::GetInstanceOf("group0", "ModernMeshItemGroup");
		mesh_group->SetupClonedMesh(it.myItem->as<ModernMeshItemGroup>());
		mesh->addItem((CMSP&)mesh_group);

		if (reuse_materials)
		{
			for (auto& mat : it.myItem->getItems())
			{
				if (mat.myItem->isSubType("Material"))
				{
					mesh_group->addItem((CMSP&)mat.myItem);
				}
			}
		}
	}
	return mesh;
}

void ModernMeshItemGroup::SetupClonedMesh(ModernMeshItemGroup* cloned_from)
{
	myTriangleCount = cloned_from->myTriangleCount;
	myVertexCount = cloned_from->myVertexCount;
	myVertexSize = cloned_from->myVertexSize;
	myCullMode = cloned_from->myCullMode;
	myTexCoordsScale = cloned_from->myTexCoordsScale;
	mBoundaries = cloned_from->mBoundaries;
	mBoundariesMask = cloned_from->mBoundariesMask;
	myVertexArrayMask = cloned_from->myVertexArrayMask;
	mInstanced = cloned_from->mInstanced;
	mNoLight = cloned_from->mNoLight;
	myVertexDesc = cloned_from->myVertexDesc;

	myVertexBuffer = cloned_from->myVertexBuffer;
	myIndexBuffer = cloned_from->myIndexBuffer;
	myIndexType = cloned_from->myIndexType;
	myOwnedBuffer = false;

}

void ModernMesh::InitModifiable()
{
	HDrawable::InitModifiable();
	if (IsInit())
	{
		bool initOk = true;
		bool is_exported = false;
		if (getValue("IsCreatedFromExport", is_exported))
		{
			RemoveDynamicAttribute("IsCreatedFromExport");
		}
		else if ((!myWasBuild) && (myFileName.const_ref() != ""))
		{
			auto& pathManager = KigsCore::Singleton<FilePathManager>();
			auto filename = myFileName.const_ref();

			/*if (filename.substr(filename.size() - 4) == ".xml" || filename.substr(filename.size() - 5) == ".kxml")
			{
				Import(filename, true);
				for (auto& it : getItems())
				{
					it.myItem->RecursiveInit(true);
				}
			}
			else*/
			{
				std::string fullfilename;
				SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(myFileName.const_ref());
				if (fullfilenamehandle)
				{
					fullfilename = fullfilenamehandle->myFullFileName;
					BinMeshLoader loader;
					if (loader.ImportFile(this, fullfilename) != 0)
					{
						initOk = false;
					}
				}
			}
		}
		else if (!myWasBuild)
		{
			initOk = false;
		}

		if (initOk)
		{
			InitBoundingBox();
		}
		else
		{
			UninitModifiable();
		}

		
		auto cm = CoreModifiable::GetFirstInstance("CollisionManager");
		auto tree = getAttribute("AABBTree");
		if (cm && tree)
		{
			if (tree->getType() == ATTRIBUTE_TYPE::STRING)
			{
				std::string path;
				tree->getValue(path);
				cm->SimpleCall("SetAABBTreeFromFile", path, this);
			}
			else
			{
				cm->SimpleCall("DeserializeAABBTree", ((maBuffer*)tree)->const_ref().get(), this);
			}
			SetCanFree();
			RemoveDynamicAttribute("AABBTree");
		}
	}
}

void ModernMesh::InitBoundingBox()
{
	if (IsInit())
	{
		bool first = true;
		std::vector<ModifiableItemStruct>::const_iterator	it;
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			if ((*it).myItem->isSubType(ModernMeshItemGroup::myClassID))
			{
				SP<ModernMeshItemGroup>& current = (SP<ModernMeshItemGroup>&)(*it).myItem;
				if (current->myVertexCount)
				{
					unsigned char* vertexStart = (unsigned char*)current->myVertexBufferArray.buffer();
					
					Point3D*	currentVertex = (Point3D*)vertexStart;
					int i;

					if (first)
					{
						myBoundingBox.Init(*currentVertex);
						first = false;
					}
					
					for (i = 0; i < current->myVertexCount; i++)
					{
						
						myBoundingBox.Update(*currentVertex);
					
						vertexStart += current->myVertexSize;
						currentVertex = (Point3D*)vertexStart;
					}
				}
			}
		}
		//myBoundingBox.Init(VertexArray,(int)VertexCount);
	}
}

void ModernMesh::StartMeshBuilder()
{
	if (myCurrentMeshBuilder)
	{
		KIGS_ERROR("StartMeshBuilder called but already mesh builder already existing\n", 1);
		return;
	}
	myCurrentMeshBuilder = new ModernMeshBuilder();

}

void ModernMesh::StartMeshGroup(CoreVector* description, int hintVertexBufferSize, int hintTriangleBufferSize)
{
	if (myCurrentMeshBuilder)
	{
		myCurrentMeshBuilder->StartGroup(description, hintVertexBufferSize, hintTriangleBufferSize);
	}
}

void ModernMesh::AddTriangle(void* v1, void* v2, void* v3)
{
	if (myCurrentMeshBuilder)
	{
		myCurrentMeshBuilder->AddTriangle(v1, v2, v3);
	}
}

void ModernMesh::PlaceMergeBarrier()
{
	if (myCurrentMeshBuilder)
	{
		myCurrentMeshBuilder->PlaceMergeBarrier();
	}
}

SP<ModernMeshItemGroup>	ModernMesh::EndMeshGroup()
{
	SP<ModernMeshItemGroup> createdGroup(nullptr);
	if (myCurrentMeshBuilder)
	{
		createdGroup = myCurrentMeshBuilder->EndGroup((bool)myOptimize);
		if (createdGroup)
		{
			addItem((CMSP&)createdGroup);
		}
	}
	return createdGroup;
}

SP<ModernMeshItemGroup>	ModernMesh::EndMeshGroup(void * vertex, int vertexCount, void * index, int indexCount)
{
	SP<ModernMeshItemGroup> createdGroup(nullptr);
	if (myCurrentMeshBuilder)
	{
		createdGroup = myCurrentMeshBuilder->EndGroup(vertex, vertexCount, index, indexCount);
		if (createdGroup)
		{
			addItem((CMSP&)createdGroup);
		}
	}
	return createdGroup;
}

SP<ModernMeshItemGroup> ModernMesh::EndMeshGroup(int vertex_count, v3f* vertices, v3f* normals, v4f* colors, v2f* texCoords, int face_count, v3u* faces, v3f offset)
{
	SP<ModernMeshItemGroup> createdGroup(nullptr);
	if (myCurrentMeshBuilder)
	{
		createdGroup = myCurrentMeshBuilder->EndGroup(vertex_count, vertices, normals, colors, texCoords, face_count, faces, offset);
		if (createdGroup)
		{
			addItem((CMSP&)createdGroup);
		}
	}
	return createdGroup;
}

void ModernMesh::EndMeshBuilder()
{
	if (myCurrentMeshBuilder)
	{
		myWasBuild = true;
		delete myCurrentMeshBuilder;
		myCurrentMeshBuilder = 0;
	}
	else
	{
		KIGS_ERROR("EndMeshBuilder called but no mesh builder existing\n", 1);
	}

}

#ifdef KIGS_TOOLS

// give a chance to the object to add dynamic attribute to its export
void ModernMesh::PrepareExport(ExportSettings* settings)
{
#ifdef WIN32
	HDrawable::PrepareExport(settings);
	if (myFileName.const_ref().empty())
	{
		AddDynamicAttribute(CoreModifiable::BOOL, "IsCreatedFromExport");
		setValue("IsCreatedFromExport", true);
		//RemoveDynamicAttribute("ExportingFile");
	}

	auto cm = CoreModifiable::GetFirstInstance("CollisionManager");
	bool b = false;
	bool export_colliders = cm && cm->getValue("ExportColliders", b) && b;
	if (export_colliders && getAttribute("AABBTree"))
	{
		auto path = getValue<std::string>("AABBTree");

		auto poskaabb = path.find(".kaabb");
		auto posaabb = path.find(".aabb");

		// Path is already known
		if (poskaabb != std::string::npos || posaabb != std::string::npos)
		{
			if (settings->external_files_exported.find(path) != settings->external_files_exported.end())
			{
				return;
			}
		}

		SmartPointer<CoreRawBuffer> crb = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, char>);
		if (cm->SimpleCall<bool>("SerializeAABBTree", crb.get(), this))
		{
			if (crb->size() >= settings->export_buffer_attribute_as_external_file_size_threshold)
			{
				CMSP& compressManager = KigsCore::GetSingleton("KXMLManager");

				auto poscrc = path.find("$CRCHASH$");
				auto posmeow = path.find("$MEOWHASH$");

				if (poscrc != std::string::npos)
				{
					path.erase(poscrc, 9);
					auto crc = crc32_bitwise(crb->buffer(), crb->size());
					path.insert(poscrc, std::to_string(crc));
				}
#ifdef WIN32
				else if (posmeow != std::string::npos)
				{
					path.erase(posmeow, 10);
					meow_hash Hash = MeowHash_Accelerated(0, crb->size(), crb->buffer());
					STACK_STRING(str, 256, "%08X-%08X-%08X-%08X", MeowU32From(Hash, 0), MeowU32From(Hash, 1), MeowU32From(Hash, 2), MeowU32From(Hash, 3));
					path.insert(posmeow, str);
				}
#endif
				path += (compressManager ? ".kaabb" : ".aabb");
				setValue("AABBTree", path);

				if (settings->external_files_exported.find(path) == settings->external_files_exported.end())
				{
					if (compressManager)
					{
						auto result = OwningRawPtrToSmartPtr(new CoreRawBuffer);
						compressManager->SimpleCall("CompressData", crb.get(), result.get());
						ModuleFileManager::SaveFile(path.c_str(), (u8*)result->data(), result->size());
					}
					else
					{
						ModuleFileManager::SaveFile(path.c_str(), (u8*)crb->data(), crb->length());
					}

					if(settings->current_package)
						settings->current_package->AddFile(path, path);

					settings->external_files_exported.insert(path);
				}
			}
			else
			{
				cm->RemoveDynamicAttribute("AABBTree");
				auto m = (maBuffer*)AddDynamicAttribute(COREBUFFER, "AABBTree", "");
				m->const_ref()->SetBuffer(std::move(*crb.get()));
			}
		}
			
	}
#endif // WIN32
}

// called after the object was exported (remove dynamics added before export)
void ModernMesh::EndExport(ExportSettings* settings)
{
	if (myFileName.const_ref().empty())
	{
		RemoveDynamicAttribute("IsCreatedFromExport");
	}
	HDrawable::EndExport(settings);
}

#endif

void ModernMeshItemGroup::ImportFromCData(const std::string& imported)
{
	std::string	head = imported.substr(0, 4);
	if (head == "vert")
	{
		std::string	buff = imported.substr(4, imported.size() - 4);
		unsigned int readSize;
		unsigned char* rawbuf = AsciiParserUtils::StringToBuffer(buff, readSize);
		myVertexBufferArray.SetBuffer(rawbuf, readSize);
	}
	else if (head == "tris")
	{
		std::string	buff = imported.substr(4, imported.size() - 4);
		unsigned int readSize;
		unsigned char* rawbuf = AsciiParserUtils::StringToBuffer(buff, readSize);
		myTriangleBuffer.SetBuffer(rawbuf, readSize);
	}
}
#ifdef KIGS_TOOLS
void ModernMeshItemGroup::PrepareExport(ExportSettings* settings)
{
	ParentClassType::PrepareExport(settings);
	settings->PushID(getFirstParent("ModernMesh")->getName());

	CoreItemSP	desc(nullptr);
	if (myVertexDesc.size())
	{

		desc = CoreItemSP(new CoreMap<std::string>(), StealRefTag{});

		auto s1 = myVertexBufferArray.const_ref()->size();
		auto s2 = myVertexCount * myVertexSize;
		KIGS_ASSERT(s1 == s2);
		
		//add general parameters
		desc->set("TriangleCount",CoreItemSP::getCoreValue((int)myTriangleCount));
		desc->set("VertexCount",CoreItemSP::getCoreValue((int)myVertexCount));
		desc->set("VertexSize",CoreItemSP::getCoreValue((int)myVertexSize));
		desc->set("VertexArrayMask",CoreItemSP::getCoreValue((int)myVertexArrayMask));

		std::vector<ModernMesh::VertexElem>::iterator itstart = myVertexDesc.begin();
		std::vector<ModernMesh::VertexElem>::iterator itend = myVertexDesc.end();

		while (itstart != itend)
		{
			CoreItemSP params = CoreItemSP::getCoreVector();
			desc->set((*itstart).name,params);
			
			params->set("",CoreItemSP::getCoreValue((int)(*itstart).size));
			params->set("",CoreItemSP::getCoreValue((int)(*itstart).startpos));
			params->set("",CoreItemSP::getCoreValue((int)(*itstart).mask));
			params->set("",CoreItemSP::getCoreValue((int)(*itstart).elemCount));

			++itstart;
		}

		desc->set("TexCoordsScale",CoreItemSP::getCoreValue((float)myTexCoordsScale));
	}

	if (!desc.isNil())
	{
		AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::COREITEM, "VertexDescription");
		setValue("VertexDescription", desc.get());
	}
}

// called after the object was exported (remove dynamics added before export)
void ModernMeshItemGroup::EndExport(ExportSettings* settings)
{
	settings->PopID();
	RemoveDynamicAttribute("VertexDescription");
	ParentClassType::EndExport(settings);
}

#endif

// check if VertexDescription is present
void ModernMeshItemGroup::InitModifiable()
{
	Drawable::InitModifiable();

	CoreItem* item = nullptr;
	if (getValue("IndexBoundaries", item))
	{
		mBoundaries.clear();
		mColliderBoundaries.clear();

		auto indices = (*item)["indices"];
		for (auto v : indices)
		{
			int value = -1;
			if (v->getValue(value))
			{
				mBoundaries.push_back(value);
			}
		} 

		auto indices_collider = (*item)["indices_collider"];
		if (!indices_collider.isNil())
		{
			for (auto v : indices_collider)
			{
				int value = -1;
				if (v->getValue(value))
				{
					mColliderBoundaries.push_back(value);
				}
			}
		}
	}

	CoreItem*	itdesc = 0;
	if (getValue("VertexDescription", (CoreItem*&)itdesc))
	{
		CoreMap<std::string>*	desc = (CoreMap<std::string>*)itdesc;
		myVertexDesc.clear();
		CoreItemSP val = desc->GetItem("TriangleCount");
		if (!val.isNil())
		{
			myTriangleCount = val;
		}

		val = desc->GetItem("VertexCount");
		if (!val.isNil())
		{
			myVertexCount = val;
		}

		val = desc->GetItem("VertexSize");
		if (!val.isNil())
		{
			myVertexSize = val;
		}

		val = desc->GetItem("VertexArrayMask");
		if (!val.isNil())
		{
			myVertexArrayMask = val;
		}

		val = desc->GetItem("TexCoordsScale");
		if (!val.isNil())
		{
			myTexCoordsScale = (float)val;
		}

		auto params = desc->GetItem("vertices");

		if (!params.isNil())
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "vertices";
			toAdd.size = params[0];
			toAdd.startpos = params[1];
			toAdd.mask = params[2];
			toAdd.elemCount = params[3];
			toAdd.type = ModernMesh::VertexElem::Type::Position3D;
			myVertexDesc.push_back(toAdd);
		}

		params = desc->GetItem("colors");
		if (!params.isNil())
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "colors";
			toAdd.size = params[0];
			toAdd.startpos = params[1];
			toAdd.mask = params[2];
			toAdd.elemCount = params[3];
			toAdd.type = toAdd.elemCount == 3 ? ModernMesh::VertexElem::Type::ColorRGB : ModernMesh::VertexElem::Type::ColorRGBA;
			myVertexDesc.push_back(toAdd);
		}

		params = desc->GetItem("texCoords");
		if (!params.isNil())
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "texCoords";
			toAdd.size = params[0];
			toAdd.startpos = params[1];
			toAdd.mask = params[2];
			toAdd.elemCount = params[3];
			toAdd.type = ModernMesh::VertexElem::Type::TextureCoordinate2D;
			myVertexDesc.push_back(toAdd);
		}

		params = desc->GetItem("normals");
		if (!params.isNil())
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "normals";
			toAdd.size = params[0];
			toAdd.startpos = params[1];
			toAdd.mask = params[2];
			toAdd.elemCount = params[3];
			toAdd.type = ModernMesh::VertexElem::Type::Normal3D;
			myVertexDesc.push_back(toAdd);

		}

		params = desc->GetItem("tangents");
		if (!params.isNil())
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "tangents";
			toAdd.size = params[0];
			toAdd.startpos = params[1];
			toAdd.mask = params[2];
			toAdd.elemCount = params[3];
			toAdd.type = ModernMesh::VertexElem::Type::Tangent3D;
			myVertexDesc.push_back(toAdd);

		}

		params = desc->GetItem("bone_weights");
		if (!params.isNil())
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "bone_weights";
			toAdd.size = params[0];
			toAdd.startpos = params[1];
			toAdd.mask = params[2];
			toAdd.elemCount = params[3];
			toAdd.type = ModernMesh::VertexElem::Type::BoneWeights;
			myVertexDesc.push_back(toAdd);

		}

		params = desc->GetItem("bone_indexes");
		if (!params.isNil())
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "bone_indexes";
			toAdd.size = params[0];
			toAdd.startpos = params[1];
			toAdd.mask = params[2];
			toAdd.elemCount = params[3];
			toAdd.type = ModernMesh::VertexElem::Type::BoneIndexes;
			myVertexDesc.push_back(toAdd);

		}

		//TODO(antoine) remove all the duplicate code

		RemoveDynamicAttribute("VertexDescription");

	}

	// check for parent modern mesh, and init parent bounding box if needed
	for (auto& p : GetParents())
	{
		if (p->isSubType("ModernMesh"))
		{
			if (p->IsInit())
			{
				p->as<ModernMesh>()->InitBoundingBox();
			}
		}
	}


}


void ModernMesh::ComputeTangents(bool useTextureCoords)
{
	// for each itemgroup
	std::vector<ModifiableItemStruct>::const_iterator	it;
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).myItem->isSubType(ModernMeshItemGroup::myClassID))
		{
			SP<ModernMeshItemGroup>& current = (SP<ModernMeshItemGroup>&)(*it).myItem;
			current->ComputeTangents(useTextureCoords);
		}
	}
}

void ModernMesh::ComputeNormals()
{
	// for each itemgroup
	std::vector<ModifiableItemStruct>::const_iterator	it;
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).myItem->isSubType(ModernMeshItemGroup::myClassID))
		{
			SP<ModernMeshItemGroup>& current = (SP<ModernMeshItemGroup>&)(*it).myItem;
			current->ComputeNormals();
		}
	}
}


void ModernMesh::ApplyScaleFactor(kfloat scaleFactor)
{
	// for each itemgroup
	std::vector<ModifiableItemStruct>::const_iterator	it;
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).myItem->isSubType(ModernMeshItemGroup::myClassID))
		{
			SP<ModernMeshItemGroup>& current = (SP<ModernMeshItemGroup>&)(*it).myItem;
			current->ApplyScaleFactor(scaleFactor);
		}
	}
}

void ModernMesh::FlipAxis(int axisX, int axisY, int axisZ)
{
	// for each itemgroup
	std::vector<ModifiableItemStruct>::const_iterator	it;
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).myItem->isSubType(ModernMeshItemGroup::myClassID))
		{
			SP<ModernMeshItemGroup>& current = (SP<ModernMeshItemGroup>&)(*it).myItem;
			current->FlipAxis(axisX,axisY,axisZ);
		}
	}
}

void ModernMesh::GetItemGroup(std::vector<ModernMeshItemGroup*>& list)
{
	// for each itemgroup
	std::vector<ModifiableItemStruct>::const_iterator	it;
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).myItem->isSubType(ModernMeshItemGroup::myClassID))
		{
			list.push_back((ModernMeshItemGroup*)((*it).myItem.get()));
		}
	}
}


void ModernMesh::GetTriangles(int & VCount, float *& VArray, int & ICount, void *& IArray)
{
	ICount = 0;
	VCount = 0;

	std::vector<ModernMeshItemGroup*> list;
	GetItemGroup(list);

	{
		// first pass (get buffer size)
		std::vector<ModernMeshItemGroup*>::iterator itr = list.begin();
		std::vector<ModernMeshItemGroup*>::iterator end = list.end();
		for (; itr != end; ++itr)
		{
			ICount += (*itr)->myTriangleCount * 3;
			VCount += (*itr)->myVertexCount;

			KIGS_ASSERT((*itr)->mCanFreeBuffers != 2);
		}
	}

	VArray = new float[3 * VCount];


	if (VCount < 65536) // if total vcount < 65536 then each group vcount is also < 65536
	{
		IArray = malloc(3 * ICount * sizeof(u16));
	}
	else
	{
		IArray = malloc(3 * ICount * sizeof(u32));
	}

	u8 * GlobalIwriter = (u8*)IArray;

	int cIndex  = 0;
	int vOffset = 0;
	int vStart = 0;
	int vSize = 0;
	char * vertexData;

	float * Vwriter = VArray;
	
	std::vector<ModernMesh::VertexElem>::iterator ditr;
	std::vector<ModernMesh::VertexElem>::iterator dend;

	{
		// second pass (fill the buffers)
		std::vector<ModernMeshItemGroup*>::iterator itr = list.begin();
		std::vector<ModernMeshItemGroup*>::iterator end = list.end();
		for (; itr != end; ++itr)
		{
#ifdef _DEBUG
			KIGS_ASSERT((*itr)->mCanFreeBuffers != 2);
#endif
			vertexData = reinterpret_cast<char*>((*itr)->myVertexBufferArray.buffer());

			vOffset = (*itr)->myVertexSize;

			//look for vertex desc
			ditr = (*itr)->myVertexDesc.begin();
			dend = (*itr)->myVertexDesc.end();
			for (; ditr != dend; ++ditr)
			{
				if ((*ditr).mask == ModuleRenderer::VERTEX_ARRAY_MASK)
				{
					vStart = (*ditr).startpos;
					vSize = (*ditr).size;
					break;
				}
			}

			char * Vreader = vertexData;
			for (int i = 0; i < (*itr)->myVertexCount; i++)
			{
				memcpy(Vwriter, Vreader + vStart, vSize);
				Vwriter += 3;
				Vreader += vOffset;
			}

			// fill VArray
			if (VCount < 65536) // if total vcount < 65536 then each group vcount is also < 65536
			{
			
				u16 * Iwriter = (u16*)GlobalIwriter;
				u16 * IReader = reinterpret_cast<u16*>((*itr)->myTriangleBuffer.buffer());
				for (int i = 0; i < (*itr)->myTriangleCount * 3; i++)
				{
					(*Iwriter++) = (*IReader++) + cIndex;
				}
				// set GlobalIwriter to it's new position
				GlobalIwriter = (u8*)Iwriter;
			}
			else
			{
				// check each group
				if ((*itr)->myVertexCount < 65536)
				{
					u16 * Iwriter = (u16*)GlobalIwriter;
					u16 * IReader = reinterpret_cast<u16*>((*itr)->myTriangleBuffer.buffer());
					for (int i = 0; i < (*itr)->myTriangleCount * 3; i++)
					{
						(*Iwriter++) = (*IReader++) + cIndex;
					}
					// set GlobalIwriter to it's new position
					GlobalIwriter = (u8*)Iwriter;
				}
				else
				{
					u32 * Iwriter = (u32*)GlobalIwriter;
					u32 * IReader = reinterpret_cast<u32*>((*itr)->myTriangleBuffer.buffer());
					for (int i = 0; i < (*itr)->myTriangleCount * 3; i++)
					{
						(*Iwriter++) = (*IReader++) + cIndex;
					}
					// set GlobalIwriter to it's new position
					GlobalIwriter = (u8*)Iwriter;
				}
			}
			cIndex += (*itr)->myVertexCount;
		}
	}
}

void ModernMesh::SetCanFree()
{
	for (auto it : getItems())
	{
		if (it.myItem->isSubType("ModernMeshItemGroup"))
		{
			int exp = 0;
			it.myItem->as<ModernMeshItemGroup>()->mCanFreeBuffers.compare_exchange_strong(exp, 1);
		}
	}
}

void ModernMesh::GetVertex(int & VCount, float *& VArray)
{
	VCount = 0;

	std::vector<ModernMeshItemGroup*> list;
	GetItemGroup(list);

	{
		// first pass (get buffer size)
		std::vector<ModernMeshItemGroup*>::iterator itr = list.begin();
		std::vector<ModernMeshItemGroup*>::iterator end = list.end();
		for (; itr != end; ++itr)
		{
			VCount += (*itr)->myTriangleCount * 3;
		}
	}

	VArray = new float[3 * VCount];
	memset(VArray, 0x00, 3 * VCount * sizeof(int));


	//int cIndex  = 0;
	int vOffset = 0;
	int vStart = 0;
	int vSize = 0;
	char * VReader;

	int * Vwriter = (int*)VArray;

	std::vector<ModernMesh::VertexElem>::iterator ditr;
	std::vector<ModernMesh::VertexElem>::iterator dend;

	{
		// second pass (fill the buffers)
		std::vector<ModernMeshItemGroup*>::iterator itr = list.begin();
		std::vector<ModernMeshItemGroup*>::iterator end = list.end();
		for (; itr != end; ++itr)
		{
			VReader = reinterpret_cast<char*>((*itr)->myVertexBufferArray.buffer());

			vOffset = (*itr)->myVertexSize;

			//look for vertex desc
			ditr = (*itr)->myVertexDesc.begin();
			dend = (*itr)->myVertexDesc.end();
			for (; ditr != dend; ++ditr)
			{
				if ((*ditr).mask == ModuleRenderer::VERTEX_ARRAY_MASK)
				{
					vStart = (*ditr).startpos;
					vSize = (*ditr).size;
					break;
				}
			}

			// fill VArray
			//int indexSize = 4;
			if ((*itr)->myVertexCount < 65536)
			{
				char * vertexData;
				u16 * IReader = reinterpret_cast<u16*>((*itr)->myTriangleBuffer.buffer());
				for (int i = 0; i < (*itr)->myTriangleCount * 3; i++)
				{
					vertexData = VReader + (vOffset*IReader[0]);
					memcpy(Vwriter, vertexData + vStart, vSize);

					Vwriter += 3;
					IReader++;
				}
			}
			else
			{
				char * vertexData;
				u32 * IReader = reinterpret_cast<u32*>((*itr)->myTriangleBuffer.buffer());
				for (int i = 0; i < (*itr)->myTriangleCount * 3; i++)
				{
					vertexData = VReader + (vOffset*IReader[0]);
					memcpy(Vwriter, vertexData + vStart, vSize);

					Vwriter += 3;
					IReader++;
				}
			}
		}
	}
}

void ModernMeshItemGroup::GetTangent(unsigned char* v1, unsigned char* v2, unsigned char* v3, unsigned int tcpos, Vector3D& sdir)
{
	const Point2D&		w1 = *(Point2D*)(v1 + tcpos);
	const Point2D&		w2 = *(Point2D*)(v2 + tcpos);
	const Point2D&		w3 = *(Point2D*)(v3 + tcpos);

	const Point3D&		vt1 = *(Point3D*)v1;
	const Point3D&		vt2 = *(Point3D*)v2;
	const Point3D&		vt3 = *(Point3D*)v3;

	float x1 = vt2.x - vt1.x;
	float x2 = vt3.x - vt1.x;
	float y1 = vt2.y - vt1.y;
	float y2 = vt3.y - vt1.y;
	float z1 = vt2.z - vt1.z;
	float z2 = vt3.z - vt1.z;

	float s1 = w2.x - w1.x;
	float s2 = w3.x - w1.x;
	float t1 = w2.y - w1.y;
	float t2 = w3.y - w1.y;

	float r = 1.0F / (s1 * t2 - s2 * t1);
	sdir.Set((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
		(t2 * z1 - t1 * z2) * r);


}


void ModernMeshItemGroup::ComputeTangents(bool useTextureCoords)
{
	// already have tangents ?
	if ((myVertexArrayMask & ModuleRenderer::TANGENT_ARRAY_MASK))
	{
		return;
	}

	
	// need normals and texture
	if ((myVertexArrayMask & (ModuleRenderer::NORMAL_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK)) != (ModuleRenderer::NORMAL_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK))
	{
		return;
	}


	unsigned int	textcoordpos = 0;
	unsigned int	normalpos = 0;

	// retreive texture coords pos and normal pos
	std::vector<ModernMesh::VertexElem>::iterator	descit = myVertexDesc.begin();
	std::vector<ModernMesh::VertexElem>::iterator	descitend = myVertexDesc.end();

	while (descit != descitend)
	{
		ModernMesh::VertexElem& currentdesc = *descit;

		if (currentdesc.name == "texCoords")
		{
			textcoordpos = currentdesc.startpos;
		}
		else if (currentdesc.name == "normals")
		{
			normalpos = currentdesc.startpos;
		}

		++descit;
	}

	// should check if tangent

	ModernMesh::VertexElem	toAdd;
	toAdd.name = "tangents";
	toAdd.size = 3 * 4;						// packed on an integer
	toAdd.startpos = myVertexSize;		// add it after current size
	toAdd.inSize = 3 * 4;
	toAdd.mask = ModuleRenderer::TANGENT_ARRAY_MASK;
	toAdd.elemCount = 3;
	myVertexDesc.push_back(toAdd);

	myVertexArrayMask |= toAdd.mask;
	myVertexSize += toAdd.size;

	// now compute tangents for each vertice
	unsigned char* vertexStart = (unsigned char*)myVertexBufferArray.buffer();
	unsigned int   vsize = myVertexSize - toAdd.inSize;

	Vector3D*	tangentArray = new Vector3D[myVertexCount];
	memset(tangentArray, 0, sizeof(Vector3D)*myVertexCount);

	Vector3D sdir;
	int i;
	if (useTextureCoords)
	{
		if (myVertexCount < 65536)
		{
			ModernMesh::Triangle<unsigned short>*	triangles = (ModernMesh::Triangle<unsigned short>*)myTriangleBuffer.buffer();

			// we have to use triangles to compute tangents, each vertice apart is not useful
			for (i = 0; i < myTriangleCount; i++)
			{
				unsigned char* v1 = vertexStart + vsize*triangles[i].indices[0];
				unsigned char* v2 = vertexStart + vsize*triangles[i].indices[1];
				unsigned char* v3 = vertexStart + vsize*triangles[i].indices[2];

				sdir.Set(0.0f, 0.0f, 0.0f);
				GetTangent(v1, v2, v3, textcoordpos, sdir);

				tangentArray[triangles[i].indices[0]] += sdir;
				tangentArray[triangles[i].indices[1]] += sdir;
				tangentArray[triangles[i].indices[2]] += sdir;

			}
		}
		else
		{
			ModernMesh::Triangle<unsigned int>*	triangles = (ModernMesh::Triangle<unsigned int>*)myTriangleBuffer.buffer();

			// we have to use triangles to compute tangents, each vertice apart is not useful
			for (i = 0; i < myTriangleCount; i++)
			{
				unsigned char* v1 = vertexStart + vsize*triangles[i].indices[0];
				unsigned char* v2 = vertexStart + vsize*triangles[i].indices[1];
				unsigned char* v3 = vertexStart + vsize*triangles[i].indices[2];

				sdir.Set(0.0f, 0.0f, 0.0f);
				GetTangent(v1, v2, v3, textcoordpos, sdir);

				tangentArray[triangles[i].indices[0]] += sdir;
				tangentArray[triangles[i].indices[1]] += sdir;
				tangentArray[triangles[i].indices[2]] += sdir;
			}
		}
	}

	unsigned char*	oldVertexArray = (unsigned char*)((const CoreRawBuffer*)myVertexBufferArray)->CopyBuffer();

	// create new vertex buffer with good size
	auto rawbuf = new unsigned char[myVertexCount*myVertexSize];
	myVertexBufferArray.SetBuffer((void*)rawbuf, myVertexCount*myVertexSize);
	delete[] rawbuf;
	memset(myVertexBufferArray.buffer(), 0, myVertexCount*myVertexSize);

	unsigned char* readVertex = oldVertexArray;

	unsigned char* writeVertex = (unsigned char*)myVertexBufferArray.buffer();
	// encode tangents
	for (i = 0; i < myVertexCount; i++)
	{

		memcpy(writeVertex, readVertex, vsize);

		char*				cn = (char*)(readVertex + normalpos);
		Vector3D			n(((float)(*cn))*(2.0f / 255.0f), ((float)(*(cn + 1)))*(2.0f / 255.0f), ((float)(*(cn + 2)))*(2.0f / 255.0f));
		
		Vector3D&			t = tangentArray[i];

		Vector3D	t1;
		if (!useTextureCoords)
		{
			n.Normalize();

			if (fabsf(n.z) > 0.8)
			{
				t.Set(1, 0, 0);
			}
			else
			{
				t.Set(0, 0, 1);
			}
	
		}
		else
		{
			t.Normalize();
		}
		
		t1.CrossProduct(n, t);

		t1.Normalize();
		// and t again
		t.CrossProduct(t1, n);
		t.Normalize();


		unsigned int*		writeTangent = (unsigned int*)(writeVertex + vsize);
		signed char*		component = (signed char*)writeTangent;
		 
		component[0] = (signed char)((t.x*255.0f) / 2.0f);
		component[1] = (signed char)((t.y*255.0f) / 2.0f);
		component[2] = (signed char)((t.z*255.0f) / 2.0f);

		writeVertex += myVertexSize;
		readVertex += vsize;
	}

	delete[] oldVertexArray;


	delete[] tangentArray;


}

void ModernMeshItemGroup::ComputeNormals()
{

	// already have normals ?
	if ((myVertexArrayMask & ModuleRenderer::NORMAL_ARRAY_MASK))
	{
		return;
	}

	ModernMesh::VertexElem	toAdd;
	toAdd.name = "normals";
	toAdd.size = 4;						// packed on an integer
	toAdd.startpos = myVertexSize;		// add it after current size
	toAdd.inSize = 4;
	toAdd.mask = ModuleRenderer::NORMAL_ARRAY_MASK;
	toAdd.elemCount = 3;
	myVertexDesc.push_back(toAdd);


	myVertexArrayMask |= toAdd.mask;
	myVertexSize += toAdd.size;

	// now compute normal for each vertice
	unsigned char* vertexStart = (unsigned char*)myVertexBufferArray.buffer();
	unsigned int   vsize = myVertexSize - toAdd.inSize;

	Vector3D*	nArray = new Vector3D[myVertexCount];
	memset(nArray, 0, sizeof(Vector3D)*myVertexCount);
	Vector3D N;
	
	if (myVertexCount < 65536)
	{
		ModernMesh::Triangle<unsigned short>* triangles = (ModernMesh::Triangle<unsigned short>*)myTriangleBuffer.buffer();
		// we have to use triangles to compute normals, each vertice apart is not useful
		for (int i = 0; i < myTriangleCount; i++)
		{
			Vector3D* v1 = (Vector3D*)(vertexStart + vsize*triangles[i].indices[0]);
			Vector3D* v2 = (Vector3D*)(vertexStart + vsize*triangles[i].indices[1]);
			Vector3D* v3 = (Vector3D*)(vertexStart + vsize*triangles[i].indices[2]);

			N.CrossProduct(*v2 - *v1, *v3 - *v1);
			N.Normalize();

			nArray[triangles[i].indices[0]] += N;
			nArray[triangles[i].indices[1]] += N;
			nArray[triangles[i].indices[2]] += N;
		}
	}
	else
	{
		ModernMesh::Triangle<unsigned int>*	triangles = (ModernMesh::Triangle<unsigned int>*)myTriangleBuffer.buffer();
		// we have to use triangles to compute tangents, each vertice apart is not useful
		for (int i = 0; i < myTriangleCount; i++)
		{
			Vector3D* v1 = (Vector3D*)(vertexStart + vsize*triangles[i].indices[0]);
			Vector3D* v2 = (Vector3D*)(vertexStart + vsize*triangles[i].indices[1]);
			Vector3D* v3 = (Vector3D*)(vertexStart + vsize*triangles[i].indices[2]);

			N.CrossProduct(*v2 - *v1, *v3 - *v1);
			N.Normalize();

			nArray[triangles[i].indices[0]] += N;
			nArray[triangles[i].indices[1]] += N;
			nArray[triangles[i].indices[2]] += N;
		}
	}

	unsigned char*	oldVertexArray = (unsigned char*)((const CoreRawBuffer*)myVertexBufferArray)->CopyBuffer();

	// create new vertex buffer with good size
	auto rawbuf = new unsigned char[myVertexCount*myVertexSize];
	myVertexBufferArray.SetBuffer((void*)rawbuf, myVertexCount*myVertexSize);
	delete[] rawbuf;
	memset(myVertexBufferArray.buffer(), 0, myVertexCount*myVertexSize);

	unsigned char* read = oldVertexArray;
	unsigned char* write = (unsigned char*)myVertexBufferArray.buffer();

	for (int i = 0; i < myVertexCount; i++)
	{
		memcpy(write, read, vsize);

		nArray[i].Normalize();

		unsigned int*		writeNormal = (unsigned int*)(write + vsize);
		signed char*		component = (signed char*)writeNormal;


		component[0] = (signed char)(nArray[i].x*127.5f);
		component[1] = (signed char)(nArray[i].y*127.5f);
		component[2] = (signed char)(nArray[i].z*127.5f);

		write += myVertexSize;
		read += vsize;
	}


	delete[] oldVertexArray;
	delete[] nArray;
}


void ModernMeshItemGroup::ApplyScaleFactor(kfloat scaleFactor)
{
	unsigned char* vertexStart = (unsigned char*)myVertexBufferArray.buffer();
	unsigned int   vsize = myVertexSize;

	for (int i = 0; i < myVertexCount; i++)
	{
		Vector3D& v1 = (Vector3D&)*((Vector3D*)(vertexStart + vsize*i));
		v1 *= scaleFactor;
	}
		
}

void ModernMeshItemGroup::FlipAxis(int axisX, int axisY, int axisZ)
{
	// TODO : check if we need to flip triangle index too
	
	unsigned char* vertexStart = (unsigned char*)myVertexBufferArray.buffer();
	unsigned int   vsize = myVertexSize;


	Matrix3x3 flipped;
	flipped.Clear();

	flipped.XAxis[abs(axisX) - 1] = (axisX < 0) ? -1 : 1;
	flipped.YAxis[abs(axisY) - 1] = (axisY < 0) ? -1 : 1;
	flipped.ZAxis[abs(axisZ) - 1] = (axisZ < 0) ? -1 : 1;

	// flip vertex
	for (int i = 0; i < myVertexCount; i++)
	{
		Vector3D& v1 = (Vector3D&)*((Vector3D*)(vertexStart + vsize*i));
		
		flipped.TransformVector(&v1);
	}

	// flip normals
	
	const ModernMesh::VertexElem* normaldesc = GetVertexDesc("normals");
	if (normaldesc)
	{
		unsigned char* normalStart = vertexStart + normaldesc->inStartPos;
		for (int i = 0; i < myVertexCount; i++)
		{
			Vector3D& n1 = (Vector3D&)*((Vector3D*)(normalStart + vsize*i));

			flipped.TransformVector(&n1);
		}
	}

	// flip tangents
	
	const ModernMesh::VertexElem* tangentdesc = GetVertexDesc("tangents");
	if (tangentdesc)
	{
		unsigned char* tangentStart = vertexStart + tangentdesc->inStartPos;
		for (int i = 0; i < myVertexCount; i++)
		{
			Vector3D& t1 = (Vector3D&)*((Vector3D*)(tangentStart + vsize*i));

			flipped.TransformVector(&t1);
		}
	}

}

void ModernMeshItemGroup::DoFirstDrawInit()
{
	auto renderer = ModuleRenderer::theGlobalRenderer;

	renderer->CreateBuffer(1, &myVertexBuffer);
	renderer->CreateBuffer(1, &myIndexBuffer);

	int size = 0;
	if (myVertexCount < 65536)
	{
		size = sizeof(unsigned short) * 3 * myTriangleCount;
		myIndexType = KIGS_UNSIGNED_SHORT;
	}
	else
	{
		size = sizeof(unsigned int) * 3 * myTriangleCount;
		myIndexType = KIGS_UNSIGNED_INT;
	}

	renderer->BufferData(myVertexBuffer, KIGS_BUFFER_TARGET_ARRAY, myVertexSize*myVertexCount, myVertexBufferArray.buffer(), KIGS_BUFFER_USAGE_STATIC);
	renderer->BufferData(myIndexBuffer, KIGS_BUFFER_TARGET_ELEMENT, size, myTriangleBuffer.buffer(), KIGS_BUFFER_USAGE_STATIC);
}

bool ModernMeshItemGroup::PreDraw(TravState* travstate)
{
	if (ParentClassType::PreDraw(travstate))
	{
		// first init if needed
		if (myVertexBuffer == -1)
			DoFirstDrawInit();

		int exp = 1;
		if (mCanFreeBuffers.compare_exchange_strong(exp, 2))
		{
			myTriangleBuffer = "";
			myVertexBufferArray = "";
		}

		auto renderer = travstate->GetRenderer();

		unsigned int render_mask = myVertexArrayMask & ~travstate->GetRenderDisableMask();
		if (mNoLight) render_mask |= ModuleRenderer::ShaderFlags::NO_LIGHT_MASK;

#ifdef KIGS_TOOLS
		auto parent = GetParents().front();
		bool drawnormals = false;
		bool drawuvs = false;
		if (parent->getValue("DrawNormals", drawnormals) && drawnormals)
		{
			render_mask |= ModuleRenderer::DRAW_NORMALS;
		}
		else if (parent->getValue("DrawUVs", drawuvs) && drawuvs)
		{
			render_mask |= ModuleRenderer::DRAW_UVS;
		}
#endif
		// create shader if none
		// mInstanced = true;
		const bool instanced = mInstanced && travstate->mCurrentPass->allow_instancing;
		if (instanced && !travstate->mDrawingInstances)
		{
			u64 mask = mBoundariesMask;
			if ((mask & 1) == 0)
				return false;
			auto& inst = travstate->mInstancing[this];
			if (!inst.shader) inst.shader = renderer->GetActiveShader();
			KIGS_ASSERT(inst.shader == renderer->GetActiveShader());

			inst.transforms.push_back(travstate->GetCurrentLocalToGlobalMatrix());
			inst.shader_variant = render_mask;
			inst.priority = travstate->CurrentNode ? travstate->CurrentNode->getDrawPriority() : 0;
			return false;
		}
		else renderer->GetActiveShader()->ChooseShader(travstate, render_mask);

		return true;
	}
	return false;
}

bool ModernMeshItemGroup::PostDraw(TravState* travstate)
{
	if (mInstanced && !travstate->mDrawingInstances) return false;
	return ParentClassType::PostDraw(travstate);
}

#ifdef KIGS_TOOLS
extern int gDrawCalls;
extern int gDrawCallsTriangleCount;
extern int gDrawCallsVertexCount;
#endif

bool ModernMeshItemGroup::Draw(TravState* travstate)
{
	if (ParentClassType::Draw(travstate))
	{
		if (!travstate->GetRenderer()->HasShader())
			return false;

		const bool instanced = mInstanced && travstate->mCurrentPass->allow_instancing;
		if (instanced && !travstate->mDrawingInstances) return false;

		auto renderer = travstate->GetRenderer();

		unsigned int render_mask = myVertexArrayMask & ~travstate->GetRenderDisableMask();

		auto shader = renderer->GetActiveShader();
		const Locations * locs = shader->GetLocation();

		if (travstate->OverrideCullMode != -1)
			renderer->SetCullMode((RendererCullMode)travstate->OverrideCullMode);
		else
			renderer->SetCullMode((RendererCullMode)(int)myCullMode);

#ifdef KIGS_TOOLS
		auto parent = (ModernMesh*)GetParents().front();
		bool wiremode = false;
		bool reset_polygon_mode = false;
		if (parent->getValue("WireMode", wiremode))
		{
			reset_polygon_mode = wiremode;
			if (wiremode)
				renderer->SetPolygonMode(RENDERER_LINE);
			else
				renderer->SetPolygonMode(RENDERER_FILL);
		}
#endif


		bool need_end_occlusion_query = false;
		bool disable_write = false;
		bool no_draw = false;
#if 1
		if (mTestOcclusion && (travstate->mCurrentPass->pass_mask & 1))
		{
			auto frame_number = travstate->GetFrameNumber();
			if (mOcclusionQueryId == -1)
			{
				if (frame_number - mLastFrameOcclusionTested > 2 * travstate->mFramesNeededForOcclusion)
				{
					mLastOcclusionResult = 1;
				}

				if (frame_number - mLastFrameOcclusionTested > travstate->mFramesNeededForOcclusion)
				{
					if (renderer->BeginOcclusionQuery(travstate, mOcclusionQueryId, RENDERER_QUERY_ANY_SAMPLES_PASSED))
					{
						need_end_occlusion_query = true;
						mLastFrameOcclusionTested = frame_number;
						no_draw = false;
					}
					else
					{
						//if(frame_number - mLastFrameOcclusionTested > 21)
						//	kigsprintf("last tested %d\n", frame_number - mLastFrameOcclusionTested);
					}
				}
			}
			else
			{
				if (!renderer->GetOcclusionQueryResult(travstate, mOcclusionQueryId, mLastOcclusionResult, 1))
				{
					//	kigsprintf("Failed to get occlusion query result\n");
				}
				else
				{
					mOcclusionQueryId = -1;
				}
			}
			disable_write = mLastOcclusionResult == 0;
			no_draw = mLastOcclusionResult == 0 && !need_end_occlusion_query;
		}
		else
		{
			mLastOcclusionResult = 1;
			mOcclusionQueryId = -1;
		}

		if (disable_write)
		{
			renderer->PushState();
			renderer->SetColorMask(false, false, false, false);
			renderer->SetDepthMaskMode(RendererDepthMaskMode::RENDERER_DEPTH_MASK_OFF);
		}
#endif

		if(!no_draw)
		{
			std::vector<ModernMesh::VertexElem>::iterator itbegin = myVertexDesc.begin();
			std::vector<ModernMesh::VertexElem>::iterator itend = myVertexDesc.end();
			while (itbegin != itend)
			{
				ModernMesh::VertexElem& current = *itbegin;
				if (current.mask & render_mask)
				{
					switch (current.mask)
					{
					case ModuleRenderer::VERTEX_ARRAY_MASK:
						renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_VERTEX_ID, current.elemCount, KIGS_FLOAT, false, myVertexSize, (void*)current.startpos, locs);
						break;
					case ModuleRenderer::NORMAL_ARRAY_MASK:
						renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_NORMAL_ID, current.elemCount, KIGS_BYTE, false, myVertexSize, (void*)current.startpos, locs);
						break;
					case ModuleRenderer::COLOR_ARRAY_MASK:
						renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_COLOR_ID, current.elemCount, KIGS_UNSIGNED_BYTE, true, myVertexSize, (void*)current.startpos, locs);
						break;
					case ModuleRenderer::TEXCOORD_ARRAY_MASK:
						renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, current.elemCount, KIGS_FLOAT, false, myVertexSize, (void*)current.startpos, locs);
						break;
					case ModuleRenderer::TANGENT_ARRAY_MASK:
						renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_TANGENT_ID, current.elemCount, KIGS_BYTE, false, myVertexSize, (void*)current.startpos, locs);
						break;
					case ModuleRenderer::BONE_WEIGHT_ARRAY_MASK:
						renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_BONE_WEIGHT_ID, current.elemCount, KIGS_UNSIGNED_BYTE, false, myVertexSize, (void*)current.startpos, locs);
						break;
					case ModuleRenderer::BONE_INDEX_ARRAY_MASK:
						renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_BONE_INDEX_ID, current.elemCount, KIGS_UNSIGNED_BYTE, false, myVertexSize, (void*)current.startpos, locs);
						break;
					}
				}
				++itbegin;
			}

			if (instanced)
			{
				size_t vec4size = sizeof(v4f);
				renderer->SetVertexAttrib(travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 0, 4, KIGS_FLOAT, false, 3 * vec4size, (void*)0);
				renderer->SetVertexAttrib(travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 1, 4, KIGS_FLOAT, false, 3 * vec4size, (void*)(vec4size));
				renderer->SetVertexAttrib(travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 2, 4, KIGS_FLOAT, false, 3 * vec4size, (void*)(2 * vec4size));

				renderer->SetVertexAttribDivisor(travstate, travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 0, 1);
				renderer->SetVertexAttribDivisor(travstate, travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 1, 1);
				renderer->SetVertexAttribDivisor(travstate, travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 2, 1);
			}

			renderer->SetArrayBuffer(myVertexBuffer, 0);
			renderer->SetElementBuffer(myIndexBuffer);
		

			if (mBoundaries.size() > 64)
			{
				//kigsprintf("MeshItemGroup has too many boundaries\n");
			}

			u64 mask = mBoundariesMask;
		
			if (mBoundaries.size() && mBoundariesMask != UINT64_MAX && !instanced)
			{
				int count = 0;

				int i = 0;
				auto sizeofindex = myIndexType == KIGS_UNSIGNED_INT ? 4 : 2;
				auto last_count = 0;
				bool last_draw = false;
				for (auto current_count : mBoundaries)
				{
					current_count *= 3;
					bool current_draw = ((mask >> i) & 1) == 1;
					if (current_draw != last_draw)
					{
						if (!current_draw)
						{
							renderer->DrawElements(travstate, KIGS_DRAW_MODE_TRIANGLES, count - last_count, myIndexType, (void*)(sizeofindex * last_count), false);
						}
						else
						{
							last_count = count;
						}
					}
					last_draw = current_draw;
					count = current_count;
					++i;
				}
				if (last_draw)
				{
					renderer->DrawElements(travstate, KIGS_DRAW_MODE_TRIANGLES, count - last_count, myIndexType, (void*)(sizeofindex * last_count));
				}
			}
			else
			{
				if (instanced) renderer->DrawElementsInstanced(travstate, KIGS_DRAW_MODE_TRIANGLES, myTriangleCount * 3, myIndexType, nullptr, travstate->mInstanceCount);
				else renderer->DrawElements(travstate, KIGS_DRAW_MODE_TRIANGLES, myTriangleCount * 3, myIndexType);
			}
		}

		if (disable_write)
		{
			renderer->PopState();
		}

#ifdef KIGS_TOOLS
		if (reset_polygon_mode)
		{
			renderer->SetPolygonMode(RENDERER_FILL);
		}
#endif
		if (need_end_occlusion_query)
		{
			renderer->EndOcclusionQuery(travstate, mOcclusionQueryId);
		}
		return true;
	}

	return false;
}

DEFINE_METHOD(ModernMeshItemGroup, ResetContext)
{
	// force creation of buffers
	myVertexBuffer = -1;
	myIndexBuffer = -1;

	return false;
}
