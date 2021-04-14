#include "MultiMesh.h"

#include "ModernMesh.h"
#include "TravState.h"
#include "CullingObject.h"

#include <algorithm>

IMPLEMENT_CLASS_INFO(MultiMesh);


IMPLEMENT_CONSTRUCTOR(MultiMesh)
{
	mFullMeshNode = KigsCore::GetInstanceOf("full_mesh_node", "Node3D");
	mFullMeshNode->Init();
	ParentClassType::addItem(mFullMeshNode);
}

void MultiMesh::PrepareExport(ExportSettings* settings)
{
	ParentClassType::PrepareExport(settings);
	ParentClassType::removeItem(mFullMeshNode);
}

void MultiMesh::EndExport(ExportSettings* settings)
{
	ParentClassType::addItem(mFullMeshNode);
	ParentClassType::EndExport(settings);
}

void MultiMesh::TravDraw(TravState* state)
{
	ParentClassType::TravDraw(state);
	if (IsAllSonsVisible() || state->IsAllVisible())
	{
		for (auto n : mSubNodes)
		{
			n->setValue("Show", false);
		}
		mFullMeshNode->setValue("Show", true);
	}
	else
	{
		for (auto n : mSubNodes)
		{
			n->setValue("Show", true);
		}
		mFullMeshNode->setValue("Show", false);
	}
}

bool	MultiMesh::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType("Node3D"))
	{
		if (std::find(mSubNodes.begin(), mSubNodes.end(), (Node3D*)item.get()) != mSubNodes.end())
			return false;

		if (item == mFullMeshNode.get())
			return false;
		
		mSubNodes.push_back((Node3D*)item.get());
		mNeedFullMeshRecompute = true;
		NeedBoundingBoxUpdate();
	}
	return ParentClassType::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool MultiMesh::removeItem(const CMSP& item)
{
	if (item->isSubType("Node3D"))
	{
		auto it = std::find(mSubNodes.begin(), mSubNodes.end(), (Node3D*)item.get());
		if(it != mSubNodes.end())
		{
			mSubNodes.erase(it);
			mNeedFullMeshRecompute = true;
			NeedBoundingBoxUpdate();
		}
	}
	return ParentClassType::removeItem(item);
}

void MultiMesh::RecomputeBoundingBox()
{
	if (mNeedFullMeshRecompute)
	{
		mNeedFullMeshRecompute = false;
		mFullMeshNode->EmptyItemList();

		struct MeshNode
		{
			CoreModifiable* mesh;
			CoreModifiable* node;
		};
		kstl::vector<MeshNode> meshes;
		kstl::vector<CMSP> nodes;
		GetSonInstancesByType("Node3D", nodes, true);


		kstl::set<CoreModifiable*> already_inserted;
		for (auto cm : nodes)
		{
			for (auto item : cm->getItems())
			{
				if (item.mItem->isSubType("ModernMesh"))
				{
					meshes.push_back(MeshNode{ item.mItem.Pointer(), cm.get() });
				}
				else if(!item.mItem->isSubType("RendererMatrix") && already_inserted.find(item.mItem.get()) == already_inserted.end())
				{
					mFullMeshNode->addItem(item.mItem); 
					already_inserted.insert(item.mItem.get());
				}
			}
		}
		auto full_mesh = KigsCore::GetInstanceOf("full_mesh", "ModernMesh");
		SmartPointer<ModernMeshItemGroup> full_mesh_item = KigsCore::GetInstanceOf("group_0", "ModernMeshItemGroup");

		unsigned int total_vertex_buffer_size = 0;
		unsigned int total_index_count = 0;
		unsigned int total_vertex_count = 0;

		for (auto m : meshes)
		{
			auto mesh = (ModernMesh*)m.mesh;
			kstl::vector<ModernMeshItemGroup*> group;
			mesh->GetItemGroup(group);

			for (auto item : group)
			{
				if (full_mesh_item->mVertexSize != 0 && item->mVertexSize != full_mesh_item->mVertexSize)
				{
					KIGS_ERROR("Cannot create a multimesh with different vertex sizes", 3);
				}
				else
				{
					full_mesh_item->mVertexSize = item->mVertexSize;
					full_mesh_item->mVertexArrayMask = item->mVertexArrayMask;
					full_mesh_item->mVertexDesc = item->mVertexDesc;
					full_mesh_item->mTexCoordsScale = item->mTexCoordsScale;
					full_mesh_item->mCullMode = item->mCullMode;

					auto mat = item->GetFirstSonByType("Material");
					if (mat)
						full_mesh_item->addItem(mat);
				}

				int vertex_buffer_size;
				item->GetVertexBuffer(vertex_buffer_size);

				total_vertex_count += item->getVertexCount();

				total_vertex_buffer_size += vertex_buffer_size;
				total_index_count += item->getTriangleCount() * 3;
			}
		}

		full_mesh_item->mTriangleCount = total_index_count / 3;
		full_mesh_item->mVertexCount = total_vertex_count;

		char* vertex_buffer = new char[total_vertex_buffer_size];
		int total_index_buffer_size = total_vertex_count < 65536 ? total_index_count * sizeof(u16) : total_index_count * sizeof(u32);
		char* index_buffer = new char[total_index_buffer_size];

		unsigned int current_index = 0;
	
		char* vertex_buffer_write = vertex_buffer;
		char* index_buffer_write = index_buffer;

		const bool is_u16 = total_vertex_count < 65536;

		auto gtl = GetGlobalToLocal();

		for (auto m : meshes)
		{
			auto mesh = (ModernMesh*)m.mesh;
			kstl::vector<ModernMeshItemGroup*> group;
			mesh->GetItemGroup(group);

			Node3D* parent = (Node3D*)m.node;
			Matrix3x4 mat; mat.SetIdentity();
			
			while (parent && parent != this)
			{
				mat = parent->GetLocal() * mat;
				parent = (Node3D*)parent->getFirstParent("Node3D");
			}

			const bool need_vertex_update = !mat.IsIdentity();

			for (auto item : group)
			{
				int vertex_buffer_size;
				void * vbuffer = item->GetVertexBuffer(vertex_buffer_size);
				memcpy(vertex_buffer_write, vbuffer, vertex_buffer_size);
				if (need_vertex_update)
				{
					int offset_pos = 0;
					int offset_normals = -1;
					int offset_tangents = -1;
					for (auto d : item->mVertexDesc)
					{
						if (d.type == ModernMesh::VertexElem::Type::Position3D)
						{
							offset_pos = d.startpos;
						}
						if (d.type == ModernMesh::VertexElem::Type::Normal3D)
						{
							offset_normals = d.startpos;
						}
						if (d.type == ModernMesh::VertexElem::Type::Tangent3D)
						{
							offset_tangents = d.startpos;
						}
					}

					const auto vsize = item->mVertexSize;
					for (int i = 0; i < item->getVertexCount(); ++i)
					{
						auto vertex = vertex_buffer_write + i*vsize + offset_pos;
						*(v3f*)vertex = mat * *(v3f*)vertex;

						if (offset_normals != -1)
						{
							auto normal = vertex_buffer_write + i*vsize + offset_normals;

							Vector3D n;
							n.x = *(signed char*)(normal+0) / 127.5f;
							n.y = *(signed char*)(normal+1) / 127.5f;
							n.z = *(signed char*)(normal+2) / 127.5f;
							mat.TransformVector(&n);
							n.Normalize();
							*(signed char*)(normal + 0) = n.x * 127.5f;
							*(signed char*)(normal + 1) = n.y * 127.5f;
							*(signed char*)(normal + 2) = n.z * 127.5f;
						}

						if (offset_tangents != -1)
						{
							auto tangent = vertex_buffer_write + i*vsize + offset_tangents;

							Vector3D n;
							n.x = *(signed char*)(tangent + 0) / 127.5f;
							n.y = *(signed char*)(tangent + 1) / 127.5f;
							n.z = *(signed char*)(tangent + 2) / 127.5f;
							mat.TransformVector(&n);
							n.Normalize();
							*(signed char*)(tangent + 0) = n.x * 127.5f;
							*(signed char*)(tangent + 1) = n.y * 127.5f;
							*(signed char*)(tangent + 2) = n.z * 127.5f;
						}


					}
				}
				vertex_buffer_write += vertex_buffer_size;
				

				int index_buffer_size;
				void * ibuffer = item->GetIndexBuffer(index_buffer_size);

				const bool is_item_u16 = item->getVertexCount() < 65536;
				for (int i = 0; i < item->getTriangleCount() * 3; ++i)
				{
					u32 index;
					if(is_item_u16)
						index = *((u16*)ibuffer + i);
					else
						index = *((u32*)ibuffer + i);

					if (is_u16)
					{
						*(u16*)index_buffer_write = (u16)index + current_index;
						index_buffer_write += sizeof(u16);
					}
					else
					{
						*(u32*)index_buffer_write = (u32)index + current_index;
						index_buffer_write += sizeof(u32);
					}
				}
				current_index += item->getVertexCount();
			}
		}

		full_mesh_item->mVertexBufferArray.SetBuffer(vertex_buffer, total_vertex_buffer_size);
		full_mesh_item->mTriangleBuffer.SetBuffer(index_buffer, total_index_buffer_size);

		full_mesh_item->Init();
		full_mesh->addItem(full_mesh_item);
		full_mesh->Init();
		mFullMeshNode->addItem(full_mesh);
	}
	ParentClassType::RecomputeBoundingBox();
}