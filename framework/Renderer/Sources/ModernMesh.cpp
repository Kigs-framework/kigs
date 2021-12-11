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
#include "maCoreItem.h"

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
	delete mCurrentMeshBuilder;
}

IMPLEMENT_CONSTRUCTOR(ModernMeshItemGroup)
{
};

ModernMeshItemGroup::~ModernMeshItemGroup()
{
	if (mVertexBuffer != -1 && mOwnedBuffer)
	{
		ModuleSceneGraph* scenegraph = KigsCore::GetModule<ModuleSceneGraph>().get();
		if (scenegraph)
		{
			scenegraph->AddDefferedItem((void*)(uintptr_t)mVertexBuffer, DefferedAction::DESTROY_BUFFER);
			scenegraph->AddDefferedItem((void*)(uintptr_t)mIndexBuffer, DefferedAction::DESTROY_BUFFER);
		}
	}
}

SP<ModernMesh> ModernMesh::CreateClonedMesh(const std::string& name, bool reuse_materials)
{
	SP<ModernMesh> mesh = KigsCore::GetInstanceOf(name, "ModernMesh");
	mesh->mBoundingBox = mBoundingBox;
	mesh->mWasBuild = true;
	mesh->Init();
	for (auto& it : getItems())
	{
		SP<ModernMeshItemGroup> mesh_group = KigsCore::GetInstanceOf("group0", "ModernMeshItemGroup");
		mesh_group->SetupClonedMesh(it.mItem->as<ModernMeshItemGroup>());
		mesh->addItem(mesh_group);

		if (reuse_materials)
		{
			for (auto& mat : it.mItem->getItems())
			{
				if (mat.mItem->isSubType("Material"))
				{
					mesh_group->addItem(mat.mItem);
				}
			}
		}
	}
	return mesh;
}

void ModernMeshItemGroup::SetupClonedMesh(ModernMeshItemGroup* cloned_from)
{
	mTriangleCount = cloned_from->mTriangleCount;
	mVertexCount = cloned_from->mVertexCount;
	mVertexSize = cloned_from->mVertexSize;
	mCullMode = cloned_from->mCullMode;
	mTexCoordsScale = cloned_from->mTexCoordsScale;
	mBoundaries = cloned_from->mBoundaries;
	mBoundariesMask = cloned_from->mBoundariesMask;
	mVertexArrayMask = cloned_from->mVertexArrayMask;
	mInstanced = cloned_from->mInstanced;
	mNoLight = cloned_from->mNoLight;
	mVertexDesc = cloned_from->mVertexDesc;

	mVertexBuffer = cloned_from->mVertexBuffer;
	mIndexBuffer = cloned_from->mIndexBuffer;
	mIndexType = cloned_from->mIndexType;
	mOwnedBuffer = false;

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
		else if ((!mWasBuild) && (mFileName.const_ref() != ""))
		{
			auto pathManager = KigsCore::Singleton<FilePathManager>();
			auto filename = mFileName.const_ref();

			/*if (filename.substr(filename.size() - 4) == ".xml" || filename.substr(filename.size() - 5) == ".kxml")
			{
				Import(filename, true);
				for (auto& it : getItems())
				{
					it.mItem->RecursiveInit(true);
				}
			}
			else*/
			{
				std::string fullfilename;
				SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(mFileName.const_ref());
				if (fullfilenamehandle)
				{
					fullfilename = fullfilenamehandle->mFullFileName;
					BinMeshLoader loader;
					if (loader.ImportFile(this, fullfilename) != 0)
					{
						initOk = false;
					}
				}
			}
		}
		else if (!mWasBuild)
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
		auto simpleshape = getAttribute("SimpleShapeCollider");

		if (cm && tree && !simpleshape)
		{
			if (tree->getType() == ATTRIBUTE_TYPE::STRING)
			{
				std::string path;
				tree->getValue(path);
				cm->SimpleCall("SetAABBTreeFromFile", path, SharedFromThis());
			}
			else
			{
				cm->SimpleCall("DeserializeAABBTree", ((maBuffer*)tree)->const_ref().get(), SharedFromThis());
			}
			SetCanFree();
			RemoveDynamicAttribute("AABBTree");
		}
		
		if (cm && simpleshape)
		{
			CoreItemSP desc = (*static_cast<maCoreItem*>(simpleshape));

			cm->SimpleCall("AddSimpleShapeFromDescription", desc.get(), SharedFromThis() );

			SetCanFree();
			RemoveDynamicAttribute("SimpleShapeCollider");
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
			if ((*it).mItem->isSubType(ModernMeshItemGroup::mClassID))
			{
				ModernMeshItemGroup* current = it->mItem->as<ModernMeshItemGroup>();
				if (current->mVertexCount)
				{
					unsigned char* vertexStart = (unsigned char*)current->mVertexBufferArray.buffer();
					
					Point3D*	currentVertex = (Point3D*)vertexStart;
					int i;

					if (first)
					{
						mBoundingBox.Init(*currentVertex);
						first = false;
					}
					
					for (i = 0; i < current->mVertexCount; i++)
					{
						
						mBoundingBox.Update(*currentVertex);
					
						vertexStart += current->mVertexSize;
						currentVertex = (Point3D*)vertexStart;
					}
				}
			}
		}
		//mBoundingBox.Init(VertexArray,(int)VertexCount);
	}
}

void ModernMesh::StartMeshBuilder()
{
	if (mCurrentMeshBuilder)
	{
		KIGS_ERROR("StartMeshBuilder called but already mesh builder already existing\n", 1);
		return;
	}
	mCurrentMeshBuilder = new ModernMeshBuilder();

}

void ModernMesh::StartMeshGroup(CoreVector* description, int hintVertexBufferSize, int hintTriangleBufferSize)
{
	if (mCurrentMeshBuilder)
	{
		mCurrentMeshBuilder->StartGroup(description, hintVertexBufferSize, hintTriangleBufferSize);
	}
}

void ModernMesh::AddTriangle(void* v1, void* v2, void* v3)
{
	if (mCurrentMeshBuilder)
	{
		mCurrentMeshBuilder->AddTriangle(v1, v2, v3);
	}
}

void ModernMesh::PlaceMergeBarrier()
{
	if (mCurrentMeshBuilder)
	{
		mCurrentMeshBuilder->PlaceMergeBarrier();
	}
}

SP<ModernMeshItemGroup>	ModernMesh::EndMeshGroup()
{
	SP<ModernMeshItemGroup> createdGroup(nullptr);
	if (mCurrentMeshBuilder)
	{
		createdGroup = mCurrentMeshBuilder->EndGroup((bool)mOptimize);
		if (createdGroup)
		{
			addItem(createdGroup);
		}
	}
	return createdGroup;
}

SP<ModernMeshItemGroup>	ModernMesh::EndMeshGroup(void * vertex, int vertexCount, void * index, int indexCount)
{
	SP<ModernMeshItemGroup> createdGroup(nullptr);
	if (mCurrentMeshBuilder)
	{
		createdGroup = mCurrentMeshBuilder->EndGroup(vertex, vertexCount, index, indexCount);
		if (createdGroup)
		{
			addItem(createdGroup);
		}
	}
	return createdGroup;
}

SP<ModernMeshItemGroup> ModernMesh::EndMeshGroup(int vertex_count, v3f* vertices, v3f* normals, v4f* colors, v2f* texCoords, int face_count, v3u* faces, v3f offset, SP<ModernMeshItemGroup> reuse_group)
{
	SP<ModernMeshItemGroup> createdGroup(nullptr);
	if (mCurrentMeshBuilder)
	{
		createdGroup = mCurrentMeshBuilder->EndGroup(vertex_count, vertices, normals, colors, texCoords, face_count, faces, offset, reuse_group);
		if (createdGroup)
		{
			addItem(createdGroup);
		}
	}
	return createdGroup;
}

void ModernMesh::EndMeshBuilder()
{
	if (mCurrentMeshBuilder)
	{
		mWasBuild = true;
		delete mCurrentMeshBuilder;
		mCurrentMeshBuilder = 0;
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
	if (mFileName.const_ref().empty())
	{
		AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL, "IsCreatedFromExport");
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

		SmartPointer<CoreRawBuffer> crb = MakeRefCounted<AlignedCoreRawBuffer<16, char>>();
		if (cm->SimpleCall<bool>("SerializeAABBTree", crb.get(), this))
		{
			if (crb->size() >= settings->export_buffer_attribute_as_external_file_size_threshold)
			{
				CMSP compressManager = KigsCore::GetSingleton("KXMLManager");

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
					auto filepath = settings->working_directory + path;
					if (compressManager)
					{
						auto result = MakeRefCounted<CoreRawBuffer>();
						compressManager->SimpleCall("CompressData", crb.get(), result.get());
						ModuleFileManager::SaveFile(filepath.c_str(), (u8*)result->data(), result->size());
					}
					else
					{
						ModuleFileManager::SaveFile(filepath.c_str(), (u8*)crb->data(), crb->length());
					}

					if(settings->current_package)
						settings->current_package->AddFile(filepath, path);

					settings->external_files_exported.insert(path);
				}
			}
			else
			{
				cm->RemoveDynamicAttribute("AABBTree");
				auto m = (maBuffer*)AddDynamicAttribute(ATTRIBUTE_TYPE::COREBUFFER, "AABBTree", "");
				m->const_ref()->SetBuffer(std::move(*crb.get()));
			}
		}
			
	}
#endif // WIN32
}

// called after the object was exported (remove dynamics added before export)
void ModernMesh::EndExport(ExportSettings* settings)
{
	if (mFileName.const_ref().empty())
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
		mVertexBufferArray.SetBuffer(rawbuf, readSize);
	}
	else if (head == "tris")
	{
		std::string	buff = imported.substr(4, imported.size() - 4);
		unsigned int readSize;
		unsigned char* rawbuf = AsciiParserUtils::StringToBuffer(buff, readSize);
		mTriangleBuffer.SetBuffer(rawbuf, readSize);
	}
}
#ifdef KIGS_TOOLS
void ModernMeshItemGroup::PrepareExport(ExportSettings* settings)
{
	ParentClassType::PrepareExport(settings);
	settings->PushID(getFirstParent("ModernMesh")->getName());

	CoreItemSP	desc(nullptr);
	if (mVertexDesc.size())
	{
		desc = MakeCoreMap();

		auto s1 = mVertexBufferArray.const_ref()->size();
		auto s2 = mVertexCount * mVertexSize;
		KIGS_ASSERT(s1 == s2);
		
		//add general parameters
		desc->set("TriangleCount", MakeCoreValue((int)mTriangleCount));
		desc->set("VertexCount", MakeCoreValue((int)mVertexCount));
		desc->set("VertexSize", MakeCoreValue((int)mVertexSize));
		desc->set("VertexArrayMask", MakeCoreValue((int)mVertexArrayMask));

		std::vector<ModernMesh::VertexElem>::iterator itstart = mVertexDesc.begin();
		std::vector<ModernMesh::VertexElem>::iterator itend = mVertexDesc.end();

		while (itstart != itend)
		{
			CoreItemSP params = MakeCoreVector();
			desc->set((*itstart).name,params);
			
			params->set("", MakeCoreValue((int)(*itstart).size));
			params->set("", MakeCoreValue((int)(*itstart).startpos));
			params->set("", MakeCoreValue((int)(*itstart).mask));
			params->set("", MakeCoreValue((int)(*itstart).elemCount));

			++itstart;
		}

		desc->set("TexCoordsScale",MakeCoreValue((float)mTexCoordsScale));
	}

	if (desc)
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
		if (indices_collider)
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
		mVertexDesc.clear();
		CoreItemSP val = desc->GetItem("TriangleCount");
		if (val)
		{
			mTriangleCount = *val;
		}

		val = desc->GetItem("VertexCount");
		if (val)
		{
			mVertexCount = *val;
		}

		val = desc->GetItem("VertexSize");
		if (val)
		{
			mVertexSize = *val;
		}

		val = desc->GetItem("VertexArrayMask");
		if (val)
		{
			mVertexArrayMask = *val;
		}

		val = desc->GetItem("TexCoordsScale");
		if (val)
		{
			mTexCoordsScale = (float)*val;
		}

		auto params = desc->GetItem("vertices");

		if (params)
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "vertices";
			toAdd.size = *params[0];
			toAdd.startpos = *params[1];
			toAdd.mask = *params[2];
			toAdd.elemCount = *params[3];
			toAdd.type = ModernMesh::VertexElem::Type::Position3D;
			mVertexDesc.push_back(toAdd);
		}

		params = desc->GetItem("colors");
		if (params)
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "colors";
			toAdd.size = *params[0];
			toAdd.startpos = *params[1];
			toAdd.mask = *params[2];
			toAdd.elemCount = *params[3];
			toAdd.type = toAdd.elemCount == 3 ? ModernMesh::VertexElem::Type::ColorRGB : ModernMesh::VertexElem::Type::ColorRGBA;
			mVertexDesc.push_back(toAdd);
		}

		params = desc->GetItem("texCoords");
		if (params)
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "texCoords";
			toAdd.size = *params[0];
			toAdd.startpos = *params[1];
			toAdd.mask = *params[2];
			toAdd.elemCount = *params[3];
			toAdd.type = ModernMesh::VertexElem::Type::TextureCoordinate2D;
			mVertexDesc.push_back(toAdd);
		}

		params = desc->GetItem("normals");
		if (params)
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "normals";
			toAdd.size = *params[0];
			toAdd.startpos = *params[1];
			toAdd.mask = *params[2];
			toAdd.elemCount = *params[3];
			toAdd.type = ModernMesh::VertexElem::Type::Normal3D;
			mVertexDesc.push_back(toAdd);

		}

		params = desc->GetItem("tangents");
		if (params)
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "tangents";
			toAdd.size = *params[0];
			toAdd.startpos = *params[1];
			toAdd.mask = *params[2];
			toAdd.elemCount = *params[3];
			toAdd.type = ModernMesh::VertexElem::Type::Tangent3D;
			mVertexDesc.push_back(toAdd);

		}

		params = desc->GetItem("bone_weights");
		if (params)
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "bone_weights";
			toAdd.size = *params[0];
			toAdd.startpos = *params[1];
			toAdd.mask = *params[2];
			toAdd.elemCount = *params[3];
			toAdd.type = ModernMesh::VertexElem::Type::BoneWeights;
			mVertexDesc.push_back(toAdd);

		}

		params = desc->GetItem("bone_indexes");
		if (params)
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "bone_indexes";
			toAdd.size = *params[0];
			toAdd.startpos = *params[1];
			toAdd.mask = *params[2];
			toAdd.elemCount = *params[3];
			toAdd.type = ModernMesh::VertexElem::Type::BoneIndexes;
			mVertexDesc.push_back(toAdd);

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
		if ((*it).mItem->isSubType(ModernMeshItemGroup::mClassID))
		{
			ModernMeshItemGroup* current = it->mItem->as<ModernMeshItemGroup>();
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
		if ((*it).mItem->isSubType(ModernMeshItemGroup::mClassID))
		{
			ModernMeshItemGroup* current = it->mItem->as<ModernMeshItemGroup>();
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
		if ((*it).mItem->isSubType(ModernMeshItemGroup::mClassID))
		{
			ModernMeshItemGroup* current = it->mItem->as<ModernMeshItemGroup>();
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
		if ((*it).mItem->isSubType(ModernMeshItemGroup::mClassID))
		{
			ModernMeshItemGroup* current = it->mItem->as<ModernMeshItemGroup>();
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
		if ((*it).mItem->isSubType(ModernMeshItemGroup::mClassID))
		{
			list.push_back((ModernMeshItemGroup*)((*it).mItem.get()));
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
			ICount += (*itr)->mTriangleCount * 3;
			VCount += (*itr)->mVertexCount;

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
			vertexData = reinterpret_cast<char*>((*itr)->mVertexBufferArray.buffer());

			vOffset = (*itr)->mVertexSize;

			//look for vertex desc
			ditr = (*itr)->mVertexDesc.begin();
			dend = (*itr)->mVertexDesc.end();
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
			for (int i = 0; i < (*itr)->mVertexCount; i++)
			{
				memcpy(Vwriter, Vreader + vStart, vSize);
				Vwriter += 3;
				Vreader += vOffset;
			}

			// fill VArray
			if (VCount < 65536) // if total vcount < 65536 then each group vcount is also < 65536
			{
			
				u16 * Iwriter = (u16*)GlobalIwriter;
				u16 * IReader = reinterpret_cast<u16*>((*itr)->mTriangleBuffer.buffer());
				for (int i = 0; i < (*itr)->mTriangleCount * 3; i++)
				{
					(*Iwriter++) = (*IReader++) + cIndex;
				}
				// set GlobalIwriter to it's new position
				GlobalIwriter = (u8*)Iwriter;
			}
			else
			{
				// check each group
				if ((*itr)->mVertexCount < 65536)
				{
					u16 * Iwriter = (u16*)GlobalIwriter;
					u16 * IReader = reinterpret_cast<u16*>((*itr)->mTriangleBuffer.buffer());
					for (int i = 0; i < (*itr)->mTriangleCount * 3; i++)
					{
						(*Iwriter++) = (*IReader++) + cIndex;
					}
					// set GlobalIwriter to it's new position
					GlobalIwriter = (u8*)Iwriter;
				}
				else
				{
					u32 * Iwriter = (u32*)GlobalIwriter;
					u32 * IReader = reinterpret_cast<u32*>((*itr)->mTriangleBuffer.buffer());
					for (int i = 0; i < (*itr)->mTriangleCount * 3; i++)
					{
						(*Iwriter++) = (*IReader++) + cIndex;
					}
					// set GlobalIwriter to it's new position
					GlobalIwriter = (u8*)Iwriter;
				}
			}
			cIndex += (*itr)->mVertexCount;
		}
	}
}

void ModernMesh::SetCanFree()
{
	for (auto it : getItems())
	{
		if (it.mItem->isSubType("ModernMeshItemGroup"))
		{
			int exp = 0;
			it.mItem->as<ModernMeshItemGroup>()->mCanFreeBuffers.compare_exchange_strong(exp, 1);
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
			VCount += (*itr)->mTriangleCount * 3;
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
			VReader = reinterpret_cast<char*>((*itr)->mVertexBufferArray.buffer());

			vOffset = (*itr)->mVertexSize;

			//look for vertex desc
			ditr = (*itr)->mVertexDesc.begin();
			dend = (*itr)->mVertexDesc.end();
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
			if ((*itr)->mVertexCount < 65536)
			{
				char * vertexData;
				u16 * IReader = reinterpret_cast<u16*>((*itr)->mTriangleBuffer.buffer());
				for (int i = 0; i < (*itr)->mTriangleCount * 3; i++)
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
				u32 * IReader = reinterpret_cast<u32*>((*itr)->mTriangleBuffer.buffer());
				for (int i = 0; i < (*itr)->mTriangleCount * 3; i++)
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
	if ((mVertexArrayMask & ModuleRenderer::TANGENT_ARRAY_MASK))
	{
		return;
	}

	
	// need normals and texture
	if ((mVertexArrayMask & (ModuleRenderer::NORMAL_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK)) != (ModuleRenderer::NORMAL_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK))
	{
		return;
	}


	unsigned int	textcoordpos = 0;
	unsigned int	normalpos = 0;

	// retreive texture coords pos and normal pos
	std::vector<ModernMesh::VertexElem>::iterator	descit = mVertexDesc.begin();
	std::vector<ModernMesh::VertexElem>::iterator	descitend = mVertexDesc.end();

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
	toAdd.startpos = mVertexSize;		// add it after current size
	toAdd.inSize = 3 * 4;
	toAdd.mask = ModuleRenderer::TANGENT_ARRAY_MASK;
	toAdd.elemCount = 3;
	mVertexDesc.push_back(toAdd);

	mVertexArrayMask |= toAdd.mask;
	mVertexSize += toAdd.size;

	// now compute tangents for each vertice
	unsigned char* vertexStart = (unsigned char*)mVertexBufferArray.buffer();
	unsigned int   vsize = mVertexSize - toAdd.inSize;

	Vector3D*	tangentArray = new Vector3D[mVertexCount];
	memset(tangentArray, 0, sizeof(Vector3D)*mVertexCount);

	Vector3D sdir;
	int i;
	if (useTextureCoords)
	{
		if (mVertexCount < 65536)
		{
			ModernMesh::Triangle<unsigned short>*	triangles = (ModernMesh::Triangle<unsigned short>*)mTriangleBuffer.buffer();

			// we have to use triangles to compute tangents, each vertice apart is not useful
			for (i = 0; i < mTriangleCount; i++)
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
			ModernMesh::Triangle<unsigned int>*	triangles = (ModernMesh::Triangle<unsigned int>*)mTriangleBuffer.buffer();

			// we have to use triangles to compute tangents, each vertice apart is not useful
			for (i = 0; i < mTriangleCount; i++)
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

	unsigned char*	oldVertexArray = (unsigned char*)((const CoreRawBuffer*)mVertexBufferArray)->CopyBuffer();

	// create new vertex buffer with good size
	auto rawbuf = new unsigned char[mVertexCount*mVertexSize];
	mVertexBufferArray.SetBuffer((void*)rawbuf, mVertexCount*mVertexSize);
	memset(mVertexBufferArray.buffer(), 0, mVertexCount*mVertexSize);

	unsigned char* readVertex = oldVertexArray;

	unsigned char* writeVertex = (unsigned char*)mVertexBufferArray.buffer();
	// encode tangents
	for (i = 0; i < mVertexCount; i++)
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

		writeVertex += mVertexSize;
		readVertex += vsize;
	}

	delete[] oldVertexArray;


	delete[] tangentArray;


}

void ModernMeshItemGroup::ComputeNormals()
{

	// already have normals ?
	if ((mVertexArrayMask & ModuleRenderer::NORMAL_ARRAY_MASK))
	{
		return;
	}

	ModernMesh::VertexElem	toAdd;
	toAdd.name = "normals";
	toAdd.size = 4;						// packed on an integer
	toAdd.startpos = mVertexSize;		// add it after current size
	toAdd.inSize = 4;
	toAdd.mask = ModuleRenderer::NORMAL_ARRAY_MASK;
	toAdd.elemCount = 3;
	mVertexDesc.push_back(toAdd);


	mVertexArrayMask |= toAdd.mask;
	mVertexSize += toAdd.size;

	// now compute normal for each vertice
	unsigned char* vertexStart = (unsigned char*)mVertexBufferArray.buffer();
	unsigned int   vsize = mVertexSize - toAdd.inSize;

	Vector3D*	nArray = new Vector3D[mVertexCount];
	memset(nArray, 0, sizeof(Vector3D)*mVertexCount);
	Vector3D N;
	
	if (mVertexCount < 65536)
	{
		ModernMesh::Triangle<unsigned short>* triangles = (ModernMesh::Triangle<unsigned short>*)mTriangleBuffer.buffer();
		// we have to use triangles to compute normals, each vertice apart is not useful
		for (int i = 0; i < mTriangleCount; i++)
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
		ModernMesh::Triangle<unsigned int>*	triangles = (ModernMesh::Triangle<unsigned int>*)mTriangleBuffer.buffer();
		// we have to use triangles to compute tangents, each vertice apart is not useful
		for (int i = 0; i < mTriangleCount; i++)
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

	unsigned char*	oldVertexArray = (unsigned char*)((const CoreRawBuffer*)mVertexBufferArray)->CopyBuffer();

	// create new vertex buffer with good size
	auto rawbuf = new unsigned char[mVertexCount*mVertexSize];
	mVertexBufferArray.SetBuffer((void*)rawbuf, mVertexCount*mVertexSize,false);
	delete[] rawbuf;
	memset(mVertexBufferArray.buffer(), 0, mVertexCount*mVertexSize);

	unsigned char* read = oldVertexArray;
	unsigned char* write = (unsigned char*)mVertexBufferArray.buffer();

	for (int i = 0; i < mVertexCount; i++)
	{
		memcpy(write, read, vsize);

		nArray[i].Normalize();

		unsigned int*		writeNormal = (unsigned int*)(write + vsize);
		signed char*		component = (signed char*)writeNormal;


		component[0] = (signed char)(nArray[i].x*127.5f);
		component[1] = (signed char)(nArray[i].y*127.5f);
		component[2] = (signed char)(nArray[i].z*127.5f);

		write += mVertexSize;
		read += vsize;
	}


	delete[] oldVertexArray;
	delete[] nArray;
}


void ModernMeshItemGroup::ApplyScaleFactor(kfloat scaleFactor)
{
	unsigned char* vertexStart = (unsigned char*)mVertexBufferArray.buffer();
	unsigned int   vsize = mVertexSize;

	for (int i = 0; i < mVertexCount; i++)
	{
		Vector3D& v1 = (Vector3D&)*((Vector3D*)(vertexStart + vsize*i));
		v1 *= scaleFactor;
	}
		
}

void ModernMeshItemGroup::FlipAxis(int axisX, int axisY, int axisZ)
{
	// TODO : check if we need to flip triangle index too
	
	unsigned char* vertexStart = (unsigned char*)mVertexBufferArray.buffer();
	unsigned int   vsize = mVertexSize;


	Matrix3x3 flipped;
	flipped.Clear();

	flipped.XAxis[abs(axisX) - 1] = (axisX < 0) ? -1 : 1;
	flipped.YAxis[abs(axisY) - 1] = (axisY < 0) ? -1 : 1;
	flipped.ZAxis[abs(axisZ) - 1] = (axisZ < 0) ? -1 : 1;

	// flip vertex
	for (int i = 0; i < mVertexCount; i++)
	{
		Vector3D& v1 = (Vector3D&)*((Vector3D*)(vertexStart + vsize*i));
		
		flipped.TransformVector(&v1);
	}

	// flip normals
	
	const ModernMesh::VertexElem* normaldesc = GetVertexDesc("normals");
	if (normaldesc)
	{
		unsigned char* normalStart = vertexStart + normaldesc->inStartPos;
		for (int i = 0; i < mVertexCount; i++)
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
		for (int i = 0; i < mVertexCount; i++)
		{
			Vector3D& t1 = (Vector3D&)*((Vector3D*)(tangentStart + vsize*i));

			flipped.TransformVector(&t1);
		}
	}

}

void ModernMeshItemGroup::DoFirstDrawInit()
{
	auto renderer = ModuleRenderer::mTheGlobalRenderer;

	renderer->CreateBuffer(1, &mVertexBuffer);
	renderer->CreateBuffer(1, &mIndexBuffer);

	int size = 0;
	if (mVertexCount < 65536)
	{
		size = sizeof(unsigned short) * 3 * mTriangleCount;
		mIndexType = KIGS_UNSIGNED_SHORT;
	}
	else
	{
		size = sizeof(unsigned int) * 3 * mTriangleCount;
		mIndexType = KIGS_UNSIGNED_INT;
	}

	renderer->BufferData(mVertexBuffer, KIGS_BUFFER_TARGET_ARRAY, mVertexSize*mVertexCount, mVertexBufferArray.buffer(), KIGS_BUFFER_USAGE_STATIC);
	renderer->BufferData(mIndexBuffer, KIGS_BUFFER_TARGET_ELEMENT, size, mTriangleBuffer.buffer(), KIGS_BUFFER_USAGE_STATIC);
}

bool ModernMeshItemGroup::PreDraw(TravState* travstate)
{
	if (ParentClassType::PreDraw(travstate))
	{
		// first init if needed
		if (mVertexBuffer == -1)
			DoFirstDrawInit();

		int exp = 1;
		if (mCanFreeBuffers.compare_exchange_strong(exp, 2))
		{
			mTriangleBuffer = "";
			mVertexBufferArray = "";
		}

		auto renderer = travstate->GetRenderer();

		unsigned int render_mask = mVertexArrayMask & ~travstate->GetRenderDisableMask();
		if (mNoLight) render_mask |= ModuleRenderer::ShaderFlags::NO_LIGHT_MASK;

#if defined(KIGS_TOOLS) && 0
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
			inst.priority = travstate->mCurrentNode ? travstate->mCurrentNode->getDrawPriority() : 0;
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

		unsigned int render_mask = mVertexArrayMask & ~travstate->GetRenderDisableMask();

		auto shader = renderer->GetActiveShader();
		const Locations * locs = shader->GetLocation();

		if (travstate->mOverrideCullMode != -1)
			renderer->SetCullMode((RendererCullMode)travstate->mOverrideCullMode);
		else
			renderer->SetCullMode((RendererCullMode)(int)mCullMode);

#if defined(KIGS_TOOLS) && 0
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
			std::vector<ModernMesh::VertexElem>::iterator itbegin = mVertexDesc.begin();
			std::vector<ModernMesh::VertexElem>::iterator itend = mVertexDesc.end();
			while (itbegin != itend)
			{
				ModernMesh::VertexElem& current = *itbegin;
				if (current.mask & render_mask)
				{
					switch (current.mask)
					{
					case ModuleRenderer::VERTEX_ARRAY_MASK:
						renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_VERTEX_ID, current.elemCount, KIGS_FLOAT, false, mVertexSize, (void*)(uintptr_t)current.startpos, locs);
						break;
					case ModuleRenderer::NORMAL_ARRAY_MASK:
						renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_NORMAL_ID, current.elemCount, KIGS_BYTE, false, mVertexSize, (void*)(uintptr_t)current.startpos, locs);
						break;
					case ModuleRenderer::COLOR_ARRAY_MASK:
						renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_COLOR_ID, current.elemCount, KIGS_UNSIGNED_BYTE, true, mVertexSize, (void*)(uintptr_t)current.startpos, locs);
						break;
					case ModuleRenderer::TEXCOORD_ARRAY_MASK:
						renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, current.elemCount, KIGS_FLOAT, false, mVertexSize, (void*)(uintptr_t)current.startpos, locs);
						break;
					case ModuleRenderer::TANGENT_ARRAY_MASK:
						renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_TANGENT_ID, current.elemCount, KIGS_BYTE, false, mVertexSize, (void*)(uintptr_t)current.startpos, locs);
						break;
					case ModuleRenderer::BONE_WEIGHT_ARRAY_MASK:
						// normalize needs to be set to true for weight to be in [0.0,1.0] GPU side 
						renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_BONE_WEIGHT_ID, current.elemCount, KIGS_UNSIGNED_BYTE, true, mVertexSize, (void*)(uintptr_t)current.startpos, locs);
						break;
					case ModuleRenderer::BONE_INDEX_ARRAY_MASK:
						renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_BONE_INDEX_ID, current.elemCount, KIGS_UNSIGNED_BYTE, false, mVertexSize, (void*)(uintptr_t)current.startpos, locs);
						break;
					}
				}
				++itbegin;
			}

			if (instanced)
			{
				size_t vec4size = sizeof(v4f);
				renderer->SetVertexAttrib(travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 0, 4, KIGS_FLOAT, false, (unsigned int)(3 * vec4size), (void*)(uintptr_t)0);
				renderer->SetVertexAttrib(travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 1, 4, KIGS_FLOAT, false, (unsigned int)(3 * vec4size), (void*)(uintptr_t)(vec4size));
				renderer->SetVertexAttrib(travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 2, 4, KIGS_FLOAT, false, (unsigned int)(3 * vec4size), (void*)(uintptr_t)(2 * vec4size));

				renderer->SetVertexAttribDivisor(travstate, travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 0, 1);
				renderer->SetVertexAttribDivisor(travstate, travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 1, 1);
				renderer->SetVertexAttribDivisor(travstate, travstate->mInstanceBufferIndex, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID + 2, 1);
			}

			renderer->SetArrayBuffer(mVertexBuffer, 0);
			renderer->SetElementBuffer(mIndexBuffer);
		

			if (mBoundaries.size() > 64)
			{
				//kigsprintf("MeshItemGroup has too many boundaries\n");
			}

			u64 mask = mBoundariesMask;
		
			if (mBoundaries.size() && mBoundariesMask != UINT64_MAX && !instanced)
			{
				int count = 0;

				int i = 0;
				auto sizeofindex = mIndexType == KIGS_UNSIGNED_INT ? 4 : 2;
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
							renderer->DrawElements(travstate, KIGS_DRAW_MODE_TRIANGLES, count - last_count, mIndexType, (void*)(intptr_t)(sizeofindex * last_count));
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
					renderer->DrawElements(travstate, KIGS_DRAW_MODE_TRIANGLES, count - last_count, mIndexType, (void*)(intptr_t)(sizeofindex * last_count));
				}
			}
			else
			{
				if (instanced) renderer->DrawElementsInstanced(travstate, KIGS_DRAW_MODE_TRIANGLES, mTriangleCount * 3, mIndexType, nullptr, travstate->mInstanceCount);
				else renderer->DrawElements(travstate, KIGS_DRAW_MODE_TRIANGLES, mTriangleCount * 3, mIndexType);
			}
		}

		if (disable_write)
		{
			renderer->PopState();
		}

#if defined(KIGS_TOOLS) && 0
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

