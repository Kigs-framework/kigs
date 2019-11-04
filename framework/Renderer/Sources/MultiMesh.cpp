#include "MultiMesh.h"

#include "ModernMesh.h"
#include "TravState.h"
#include "CullingObject.h"

#include <algorithm>

IMPLEMENT_CLASS_INFO(MultiMesh);


IMPLEMENT_CONSTRUCTOR(MultiMesh)
{
	_full_mesh_node = KigsCore::CreateInstance("full_mesh_node", "Node3D");
	_full_mesh_node->Init();
	ParentClassType::addItem(_full_mesh_node.get());
}

void MultiMesh::PrepareExport(ExportSettings* settings)
{
	ParentClassType::PrepareExport(settings);
	ParentClassType::removeItem(_full_mesh_node.get());
}

void MultiMesh::EndExport(ExportSettings* settings)
{
	ParentClassType::addItem(_full_mesh_node.get());
	ParentClassType::EndExport(settings);
}

void MultiMesh::TravDraw(TravState* state)
{
	ParentClassType::TravDraw(state);
	if (IsAllSonsVisible() || state->IsAllVisible())
	{
		for (auto n : _subnodes)
		{
			n->setValue("Show", false);
		}
		_full_mesh_node->setValue("Show", true);
	}
	else
	{
		for (auto n : _subnodes)
		{
			n->setValue("Show", true);
		}
		_full_mesh_node->setValue("Show", false);
	}
}

bool	MultiMesh::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType("Node3D"))
	{
		if (std::find(_subnodes.begin(), _subnodes.end(), (Node3D*)item) != _subnodes.end())
			return false;

		if (item == _full_mesh_node.get())
			return false;
		
		_subnodes.push_back((Node3D*)item);
		_need_full_mesh_recompute = true;
		NeedBoundingBoxUpdate();
	}
	return ParentClassType::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool MultiMesh::removeItem(CoreModifiable* item)
{
	if (item->isSubType("Node3D"))
	{
		auto it = std::find(_subnodes.begin(), _subnodes.end(), (Node3D*)item);
		if(it != _subnodes.end())
		{
			_subnodes.erase(it);
			_need_full_mesh_recompute = true;
			NeedBoundingBoxUpdate();
		}
	}
	return ParentClassType::removeItem(item);
}

void MultiMesh::RecomputeBoundingBox()
{
	if (_need_full_mesh_recompute)
	{
		_need_full_mesh_recompute = false;
		_full_mesh_node->EmptyItemList();

		struct MeshNode
		{
			CoreModifiable* mesh;
			CoreModifiable* node;
		};
		kstl::vector<MeshNode> meshes;
		kstl::set<CoreModifiable*> nodes;
		GetSonInstancesByType("Node3D", nodes, true);


		kstl::set<CoreModifiable*> already_inserted;
		for (auto cm : nodes)
		{
			for (auto& item : cm->getItems())
			{
				if (item.myItem->isSubType("ModernMesh"))
				{
					meshes.push_back(MeshNode{ item.myItem, cm });
				}
				else if(!item.myItem->isSubType("RendererMatrix") && already_inserted.find(item.myItem) == already_inserted.end())
				{
					_full_mesh_node->addItem(item.myItem); 
					already_inserted.insert(item.myItem);
				}
			}
		}
		auto full_mesh = KigsCore::CreateInstance("full_mesh", "ModernMesh");
		SmartPointer<ModernMeshItemGroup> full_mesh_item = KigsCore::CreateInstance("group_0", "ModernMeshItemGroup");

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
				if (full_mesh_item->myVertexSize != 0 && item->myVertexSize != full_mesh_item->myVertexSize)
				{
					KIGS_ERROR("Cannot create a multimesh with different vertex sizes", 3);
				}
				else
				{
					full_mesh_item->myVertexSize = item->myVertexSize;
					full_mesh_item->myVertexArrayMask = item->myVertexArrayMask;
					full_mesh_item->myVertexDesc = item->myVertexDesc;
					full_mesh_item->myTexCoordsScale = item->myTexCoordsScale;
					full_mesh_item->myCullMode = item->myCullMode;

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

		full_mesh_item->myTriangleCount = total_index_count / 3;
		full_mesh_item->myVertexCount = total_vertex_count;

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
					for (auto d : item->myVertexDesc)
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

					const auto vsize = item->myVertexSize;
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

		full_mesh_item->myVertexBufferArray.SetBuffer(vertex_buffer, total_vertex_buffer_size);
		full_mesh_item->myTriangleBuffer.SetBuffer(index_buffer, total_index_buffer_size);

		full_mesh_item->Init();
		full_mesh->addItem(full_mesh_item.get());
		full_mesh->Init();
		_full_mesh_node->addItem(full_mesh.get());
	}
	ParentClassType::RecomputeBoundingBox();
}