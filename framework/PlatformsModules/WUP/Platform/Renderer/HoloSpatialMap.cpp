#include "PrecompiledHeaders.h"

#include <Unknwn.h>

#include "Platform/Main/BaseApp.h"

#include "Platform/Renderer/HoloSpatialMap.h"

#include "DrawVertice.h"
#include "ModernMesh.h"
#include "Node3D.h"
#include "RendererMatrix.h"
#include "Timer.h"
#include "CollisionManager.h"
#include "AABBTree.h"
#include "Camera.h"

#include "ContinuousMatching.h"

#include <winrt/Windows.Storage.h>
#include <robuffer.h>

#include <imgui.h>
#include "KigsTools.h"



#include "concurrentqueue.h"
#include <functional>

#include "SpacialMeshBVH.h"

#include "utf8.h"

//#include <winrt/Windows.Foundation.h>
//#include <winrt/Windows.Storage.h>
//#include <winrt/Windows.Storage.Streams.h>
//#include <winrt/Windows.UI.Core.h>
//#include <pplawait.h>




IMPLEMENT_CLASS_INFO(HoloSpatialMapShader);


using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Windows::Foundation;

#define UPDATE_DELAY (3'000'000'000/100)

namespace serializer_detail
{
	template<typename PacketStream>
	bool serialize(PacketStream& stream, GuidComp& value)
	{
		CHECK_SERIALIZE(serialize_object(stream, value.Data1));
		CHECK_SERIALIZE(serialize_object(stream, value.Data2));
		CHECK_SERIALIZE(serialize_object(stream, value.Data3));
		CHECK_SERIALIZE(serialize_bytes(stream, value.Data4, 8 * sizeof(u8)));
		return true;
	}
}


struct SpatialMapSurfaceRecord
{
	s64 update_time;
	GuidComp guid;
	mat3x4 matrix;
	std::vector<u8> vertex_data;
	u32 indices_count;
	std::vector<u8> indices_data;
	v3f vertex_position_scale;

	template<typename PacketStream>
	bool Serialize(PacketStream& stream)
	{
		CHECK_SERIALIZE(serialize_object(stream, update_time));
		CHECK_SERIALIZE(serialize_object(stream, guid));
		CHECK_SERIALIZE(serialize_object(stream, matrix));
		CHECK_SERIALIZE(serialize_object(stream, vertex_data));
		CHECK_SERIALIZE(serialize_object(stream, indices_count));
		CHECK_SERIALIZE(serialize_object(stream, indices_data));
		CHECK_SERIALIZE(serialize_object(stream, vertex_position_scale));
		return true;
	}
	
	// Not serialized
	bool recording = true;
};

struct SpatialMapCallbackRecord
{
	s64 callback_time;
	std::vector<SpatialMapSurfaceRecord> surfaces;
	std::vector<GuidComp> deleted_surfaces;


	template<typename PacketStream>
	bool Serialize(PacketStream& stream)
	{
		CHECK_SERIALIZE(serialize_object(stream, callback_time));
		CHECK_SERIALIZE(serialize_object(stream, surfaces));
		CHECK_SERIALIZE(serialize_object(stream, deleted_surfaces));
		return true;
	}

};

struct SpatialMapRecording
{
	std::list<SpatialMapCallbackRecord> callbacks;
	std::vector<std::pair<s64, mat3x4>> frame_of_ref_changes;

	template<typename PacketStream>
	bool Serialize(PacketStream& stream)
	{
		CHECK_SERIALIZE(serialize_object(stream, callbacks));
		serialize_object(stream, frame_of_ref_changes);
		return true;
	}
};


static SpatialMapRecording gMapRecording;



// Retrieves the raw pixel data from the provided IBuffer object. 
// Warning: The lifetime of the returned buffer is controlled by 
// the lifetime of the buffer object that's passed to this method.
// When the buffer has been released, the pointer becomes invalid 
// and must not be used. 
static u8* GetPointerToPixelData(winrt::Windows::Storage::Streams::IBuffer const& pixelBuffer, unsigned int *length)
{
	if (length != nullptr)
	{
		*length = pixelBuffer.Length();
	}
	auto byteAccess = pixelBuffer.try_as<::Windows::Storage::Streams::IBufferByteAccess>();
	if (byteAccess)
	{
		u8* pixels = nullptr;
		byteAccess->Buffer(&pixels);
		return pixels;
	}
	if (length != nullptr)
	{
		*length = 0;
	}
	return nullptr;
}

IMPLEMENT_CLASS_INFO(HoloSpatialMap);

static GuidComp FromGUID(winrt::guid s)
{
	GuidComp r;
	winrt::guid ss = s;
	r.Data1 = ss.Data1;
	r.Data2 = ss.Data2;
	r.Data3 = ss.Data3;
	for (int i = 0; i < 8; ++i)
		r.Data4[i] = ss.Data4[i];
	return r;
}

static moodycamel::BlockingConcurrentQueue<std::function<IAsyncAction()>> sToProcess;
static std::atomic_bool sContinueProcess;
static std::thread sProcessThread;
extern bool gIsVR;
IMPLEMENT_CONSTRUCTOR(HoloSpatialMap)
{
	// ask for access right
	auto requestReturn = Surfaces::SpatialSurfaceObserver::RequestAccessAsync();
	while (requestReturn.Status() != AsyncStatus::Completed)
		Sleep(10);
	
	SpatialPerceptionAccessStatus res = requestReturn.GetResults();
	if (res == SpatialPerceptionAccessStatus::Allowed && !gIsVR && gIsHolographic)
	{
		mAllowed = true;
		
		sContinueProcess = true;
		sProcessThread = std::thread([]()
		{
			while (sContinueProcess)
			{
				std::function<IAsyncAction()> action;
				if (sToProcess.try_dequeue(action))
				{
					action().get();
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}
		});
	
		mSurfaceObserver = Surfaces::SpatialSurfaceObserver();
		mPrecision = 32; // 320 * 64.0;

		SpatialBoundingSphere bs;
		bs.Center.x = 0.0f;
		bs.Center.y = 0.0f;
		bs.Center.z = 0.0f;
		bs.Radius = 0.5f;
		mSurfaceObserver.SetBoundingVolume(SpatialBoundingVolume::FromSphere(App::GetApp()->GetStationaryReferenceFrame().CoordinateSystem(), bs));
		
		StartListening();
	}
}

HoloSpatialMap::~HoloSpatialMap()
{
	sContinueProcess = false;
	if (sProcessThread.joinable()) sProcessThread.join();
}

static std::atomic<int> sIndex = 0;
void HoloSpatialMap::CreateMesh(winrt::Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh const& inMesh, SpatialMeshInfo& outInfo, SpatialMapSurfaceRecord* record_surface)
{	

	simdVal val; // 16 aligned struct for simd operation

	if (outInfo.node)
	{
		outInfo.old_node = outInfo.node;
	}
	int index = sIndex.fetch_add(1);
	outInfo.node = KigsCore::CreateInstance("SpacialMapNode"  + std::to_string(index), "Node3D");
	ModernMesh* mesh = KigsCore::GetInstanceOf("SpacialMapMesh" + std::to_string(index), "ModernMesh")->as<ModernMesh>();
	
	Node3D* node = outInfo.node->as<Node3D>();
	//mesh->setValue("DrawNormals", true);
	//mesh->setValue("WireMode", true);
	mesh->setValue("Optimize", false);
	mesh->setValue("Show", true);

	node->Init();
	
	Matrix3x4	meshMat;
	meshMat.SetIdentity();
	// retreive mesh position
	if (inMesh)
	{
		auto matrix = inMesh.CoordinateSystem().TryGetTransformTo(App::GetApp()->GetStationaryReferenceFrame().CoordinateSystem());
		if (matrix)
		{
			auto matVal = matrix.Value();
			Matrix3x4 view(
				matVal.m11, matVal.m12, matVal.m13,
				matVal.m21, matVal.m22, matVal.m23,
				matVal.m31, matVal.m32, matVal.m33,
				matVal.m41, matVal.m42, matVal.m43);

			kfloat sx = 1.0f / (view.e[0][0] * view.e[0][0] + view.e[0][1] * view.e[0][1] + view.e[0][2] * view.e[0][2]);
			kfloat sy = 1.0f / (view.e[1][0] * view.e[1][0] + view.e[1][1] * view.e[1][1] + view.e[1][2] * view.e[1][2]);
			kfloat sz = 1.0f / (view.e[2][0] * view.e[2][0] + view.e[2][1] * view.e[2][1] + view.e[2][2] * view.e[2][2]);

			if (NormSquare(v3f{ sx - 1.0f, sy - 1.0f, sz - 1.0f }) > 0.001f)
			{
				kigsprintf("matrix has scale\n");
			}

			node->ChangeMatrix(view);
			meshMat = view;
		}
		else
		{
			kigsprintf("no matrix\n");
		}
	}

	if (record_surface)
	{
		if(record_surface->recording)
			record_surface->matrix = meshMat;
		else
		{
			meshMat = record_surface->matrix;
			node->ChangeMatrix(meshMat);
		}
	}

	mesh->StartMeshBuilder();

	CoreVector* desc = new CoreVector();
	auto cnv_vertices = new CoreNamedVector("vertices");
	desc->push_back(cnv_vertices); cnv_vertices->Destroy();
	auto cnv_normals = new CoreNamedVector("generate_normals");
	desc->push_back(cnv_normals); cnv_normals->Destroy();

	auto no_merge = new CoreNamedVector("no_merge");
	desc->push_back(no_merge); no_merge->Destroy();

	mesh->StartMeshGroup(desc);

	desc->Destroy();


	// TODO : optimize spacial map retreiving and mesh creation (vertex buffer is well indexed and we just reconstruct a 3 vertex per triangle
	// unoptimized mesh)

	// compute vertex position
	u32 vertices_count = inMesh ? inMesh.VertexPositions().ElementCount() : 0;
	u32 size;

	
	u8* buf = inMesh ? GetPointerToPixelData(inMesh.VertexPositions().Data(), &size) : nullptr;


	if (record_surface)
	{
		if (record_surface->recording)
		{
			record_surface->vertex_data.resize(size);
			memcpy(record_surface->vertex_data.data(), buf, size);
		}
		else
		{
			buf = record_surface->vertex_data.data();
			size = record_surface->vertex_data.size();
		}
	}

	int alignsize = (size + 16) / 16;

	//auto vertices_format = inMesh.VertexPositions().Format();
	
	// allign buf on 128
	__m128i* in128 = reinterpret_cast<__m128i*>(_mm_malloc(alignsize * sizeof(__m128i), 16));
	memcpy(in128, buf, size);

	float* vertex_buffer = new float[alignsize * 6];

	// retreive position scale
	

	if (inMesh)
	{
		auto scale = inMesh.VertexPositionScale();
		val.ratio = { scale.x / SHRT_MAX,scale.y / SHRT_MAX,scale.z / SHRT_MAX,0 };
	}
	
	if (record_surface && !record_surface->recording)
	{
		val.ratio = { record_surface->vertex_position_scale.x / SHRT_MAX, record_surface->vertex_position_scale.y / SHRT_MAX, record_surface->vertex_position_scale.z / SHRT_MAX,0 };
	}
	
	float* writer = vertex_buffer;
	__m128i* read = in128;
	for (int i = 0; i < alignsize; i++)
	{
		val.cmpl = _mm_cmplt_epi16(read[i], val.vec0);
		val.xlo = _mm_unpacklo_epi16(read[i], val.cmpl);
		val.xhi = _mm_unpackhi_epi16(read[i], val.cmpl);
		val.tmpF = _mm_mul_ps(_mm_cvtepi32_ps(val.xlo), val.ratio);
		*writer++ = val.tmpF.m128_f32[0];
		*writer++ = val.tmpF.m128_f32[1];
		*writer++ = val.tmpF.m128_f32[2];

		val.tmpF = _mm_mul_ps(_mm_cvtepi32_ps(val.xhi), val.ratio);
		*writer++ = val.tmpF.m128_f32[0];
		*writer++ = val.tmpF.m128_f32[1];
		*writer++ = val.tmpF.m128_f32[2];
	}
	
	_mm_free(in128);

	u32 indices_count = inMesh ? inMesh.TriangleIndices().ElementCount() : 0;
	u16* indices = inMesh ? (u16*)GetPointerToPixelData(inMesh.TriangleIndices().Data(), &size) : nullptr;

	if (record_surface)
	{
		if (record_surface->recording)
		{
			if (indices_count == 0)
			{
				__debugbreak();
			}
			auto scale = inMesh.VertexPositionScale();
			record_surface->vertex_position_scale = v3f(scale.x, scale.y, scale.z);
			record_surface->indices_count = indices_count;
			record_surface->indices_data.resize(size);
			memcpy(record_surface->indices_data.data(), indices, size);
		}
		else
		{
			indices_count = record_surface->indices_count;
			indices = (u16*)record_surface->indices_data.data();
		}
	}


	for (u32 i = 0; i < indices_count; i+=3)
	{
		mesh->AddTriangle(
			(void*)&vertex_buffer[3 * indices[i]],
			(void*)&vertex_buffer[3 * indices[i + 2]],
			(void*)&vertex_buffer[3 * indices[i + 1]]);
	}

	delete[] vertex_buffer;

	auto group = mesh->EndMeshGroup();


#ifdef KIGS_TOOLS
	group->mCanFreeBuffers = 3; // KEEP DATA FOR EXPORT
#endif
	//group->setValue("CullMode", 1);
	mesh->EndMeshBuilder();

	mesh->Init();

	node->addItem(mesh);
	mesh->Destroy();

	//mesh->AddDynamicAttribute(BOOL, "BVH", true);
	auto collisionBVH = SpacialMeshBVH::BuildFromMesh(mesh, meshMat, true);
	if (collisionBVH)
	{
		mCollisionManager->SetCollisionObject(mesh, collisionBVH);
		mContinuousMatching->RegisterSpatialMeshNode(SmartPointer<Node3D>(node, GetRefTag{}));
	}
	
	/*auto collision = AABBTree::BuildFromMesh(mesh);
	if (collision)
	{
		CollisionManager::Get()->SetCollisionObject(mesh, collision);
	}*/

}

winrt::Windows::Foundation::IAsyncAction HoloSpatialMap::ExportTimedScan()
{
	mRecordEnabled = false;

	SpatialMapRecording to_save;
	while (mRecordUses > 0)
	{
		co_await winrt::resume_after(std::chrono::milliseconds(33));
	}
	std::swap(to_save, gMapRecording);
	
	mRecordEnabled = true;

	std::vector<u32> data;
	VectorWriteStream stream{data};
	bool success = serialize_object(stream, to_save);
	stream.Flush();

	auto local_folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path();
	std::string utf8_path;
	utf8::utf16to8(local_folder.begin(), local_folder.end(), std::back_inserter(utf8_path));
	utf8_path += "\\timed_spatial_map_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".bin";
	ModuleFileManager::Get()->SaveFile(utf8_path.c_str(), (u8*)data.data(), data.size()*sizeof(u32));
}

winrt::Windows::Foundation::IAsyncAction HoloSpatialMap::ReplayRecordedSpatialMap()
{
	winrt::apartment_context ctx;

	if (gMapRecording.callbacks.empty()) co_return;
	auto start_time_record = gMapRecording.callbacks.front().callback_time;
	auto start_time_replay = std::chrono::steady_clock::now();
	int i = -1;

	static std::atomic_int end_index = 50;
	RegisterWidget("Replay", [&i, size = gMapRecording.callbacks.size()]()
	{
		ImGui::Text("Callback %d/%d", i, size);
		if (ImGui::Button("Set End Index"))
		{
			end_index = i;
		}

		int index = end_index;
		if (ImGui::SliderInt("End Index", &index, -1, size-1))
		{
			end_index = index;
		}
	});
	
	int current_matrix_index = 0;

	mat3x4 transform_for_next_nodes = mat3x4::IdentityMatrix();
	
	for (auto& cb : gMapRecording.callbacks)
	{
		++i;

		if (end_index != -1 && i >= end_index) break;

		auto current_time_replay = std::chrono::steady_clock::now();
		auto current_time_record = cb.callback_time;
		auto elapsed_record = current_time_record - start_time_record;
		auto elapsed_replay = current_time_replay - start_time_replay;


		while (current_matrix_index < gMapRecording.frame_of_ref_changes.size() 
			&& gMapRecording.frame_of_ref_changes[current_matrix_index].first < current_time_record)
		{
			auto& next_matrix = gMapRecording.frame_of_ref_changes[current_matrix_index];
			co_await ctx;
			transform_for_next_nodes = Inv(next_matrix.second) * transform_for_next_nodes;
			++current_matrix_index;
		}
		
		if (elapsed_record > elapsed_replay.count())
		{
			co_await winrt::resume_after(std::chrono::milliseconds((elapsed_record - elapsed_replay.count()) / 1000000));
		}

		std::unique_lock<std::mutex> lock{ mListMtx, std::defer_lock };

		for (auto& surface : cb.surfaces)
		{
			auto itfind = mMeshList.find(surface.guid);
			SpatialMeshInfo* info = nullptr;

			if (itfind != mMeshList.end())
			{
				info = &itfind->second;
			}
			else
			{
				lock.lock();
				info = &mMeshList[surface.guid];
				lock.unlock();
			}

			if (surface.vertex_data.size())
			{
				surface.recording = false;
				winrt::Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh mesh = nullptr;
				CreateMesh(mesh, *info, &surface);
				info->node->as<Node3D>()->ChangeMatrix(transform_for_next_nodes * info->node->as<Node3D>()->GetLocal());

				if (info->old_node)
					info->op = SpatialMeshInfo::Op::Update;
				else
					info->op = SpatialMeshInfo::Op::Add;
			}
			else
			{
				info->op = SpatialMeshInfo::Op::None;
			}

			for (auto id : cb.deleted_surfaces)
			{
				auto it = mMeshList.find(id);
				if (it != mMeshList.end())
				{
					auto exp = SpatialMeshInfo::Op::None;
					it->second.op.compare_exchange_strong(exp, SpatialMeshInfo::Op::Remove);
				}
			}
		}
	}

	RegisterWidget("Replay", {});

	SpatialMapRecording recording;
	std::swap(gMapRecording, recording);
	co_await ctx;
	co_await ResetTimedScan();
	std::swap(gMapRecording, recording);
	ReplayRecordedSpatialMap();
}

winrt::Windows::Foundation::IAsyncAction HoloSpatialMap::ResetTimedScan()
{
	winrt::apartment_context ctx;

	mRecordEnabled = false;

	SpatialMapRecording to_save;
	while (mRecordUses > 0)
	{
		co_await winrt::resume_after(std::chrono::milliseconds(33));
	}
	gMapRecording = {};

	mRecordEnabled = true;
	co_await ctx;
	{
		std::unique_lock<std::mutex> lk{ mListMtx };
		CoreModifiable* attach = (CoreModifiable*)mAttachNode;
		if (attach == nullptr) co_return;

		for (auto&& it : mMeshList)
		{
			if (it.second.node)
			{
				attach->removeItem(it.second.node.get());
			}
			if (it.second.old_node)
			{
				attach->removeItem(it.second.old_node.get());
			}
		}
		mMeshList.clear();
	}

}

void HoloSpatialMap::StartListening()
{
	if (!mAllowed) return;

	if (!mIsListening)
	{
		mEventToken = mSurfaceObserver.ObservedSurfacesChanged([this](Surfaces::SpatialSurfaceObserver const& sender, winrt::Windows::Foundation::IInspectable const& args)
		{
			bool exp = false;
			if (!gInUpdate.compare_exchange_strong(exp, true))
			{
				kigsprintf("skipping update\n");
				return;
			}

			std::unique_lock<std::mutex> lock{ mListMtx };



			for (auto&& it : mMeshList)
			{
				it.second.deleted = true;
			}

			auto surfaces = sender.GetObservedSurfaces();

#ifdef KIGS_TOOLS
			
			SpatialMapCallbackRecord* cb = nullptr;
			if (mRecordEnabled)
			{
				mRecordUses++;
				if (!mRecordEnabled)
				{
					mRecordUses--;
				}
				else
				{
					cb = &gMapRecording.callbacks.emplace_back();
					cb->callback_time = std::chrono::steady_clock::now().time_since_epoch().count();
					cb->surfaces.resize(surfaces.Size());
				}
			}
#endif

			int count = surfaces.Size();
			int delay = count ? 2800 / count : 1;


			lock.unlock();

			int surface_index = 0;
			for (auto& current : surfaces)
			{
				SpatialMeshInfo* info = nullptr;
				auto itfind = mMeshList.find(FromGUID(current.Value().Id()));
				
				auto update_time = current.Value().UpdateTime().time_since_epoch().count();
				SpatialMapSurfaceRecord* current_surface = nullptr;
#ifdef KIGS_TOOLS
				if (cb)
				{
					current_surface = &cb->surfaces[surface_index];
					current_surface->update_time = update_time;
					current_surface->guid = FromGUID(current.Value().Id());
				}
#endif

				if (itfind != mMeshList.end())
				{
					info = &itfind->second;
				}
				else
				{
					lock.lock();
					info = &mMeshList[FromGUID(current.Value().Id())];
					lock.unlock();
				}

				info->deleted = false;
				
				if (update_time - info->time <= UPDATE_DELAY)
				{
					//++surface_index;
					continue;
				}

				info->time = update_time;

				// cancel older if any
				auto exp = SpatialMeshInfo::Op::None;

				if (!info->op.compare_exchange_strong(exp, SpatialMeshInfo::Op::InProcess))
				{
					kigsprintf("busy\n");
					//++surface_index;
					continue;
				}

				//Sleep(delay);
				
				// use async task
				if (current_surface)
					mRecordUses++;
				sToProcess.enqueue([this, info, val = current.Value(), current_surface]() -> IAsyncAction
				{
					Surfaces::SpatialSurfaceMeshOptions options = Surfaces::SpatialSurfaceMeshOptions();
					options.IncludeVertexNormals(false); // (mMode == 2);
					auto pMesh = co_await val.TryComputeLatestMeshAsync(mPrecision, options);
					if (pMesh != nullptr)
					{
						CreateMesh(pMesh, *info, current_surface);
						if (info->old_node)
							info->op = SpatialMeshInfo::Op::Update;
						else
							info->op = SpatialMeshInfo::Op::Add;
					}
					else
					{
						info->op = SpatialMeshInfo::Op::None;
					}
					if(current_surface)
						mRecordUses--;

				});

				++surface_index;
			}


			for (auto&& it : mMeshList)
			{
				if (it.second.deleted)
				{
#ifdef KIGS_TOOLS
					if(cb)
						cb->deleted_surfaces.push_back(it.first);
#endif
					auto exp = SpatialMeshInfo::Op::None;
					it.second.op.compare_exchange_strong(exp, SpatialMeshInfo::Op::Remove);
				}
			}


#ifdef KIGS_TOOLS
			if (cb)
			{
				cb->surfaces.resize(surface_index);
				if (surface_index == 0)
					gMapRecording.callbacks.pop_back();
				mRecordUses--;
			}
#endif

			gInUpdate = false;
		});
		mIsListening = true;
	}
}

void HoloSpatialMap::StopListening()
{
	if (!mAllowed) return;

	if (mIsListening)
	{
		mSurfaceObserver.ObservedSurfacesChanged(mEventToken);
		mIsListening = false;
	}
}

void HoloSpatialMap::Update(const Timer& timer, void* addParam)
{
	//if (!mAllowed) return;
	// add new mesh to scene
	
	CoreModifiable* attach = (CoreModifiable*)mAttachNode;
	if (attach == nullptr) return;

	std::unique_lock<std::mutex> lk{ mListMtx, std::defer_lock_t{} };

	if (lk.try_lock())
	{
		for(auto&& it : mMeshList)
		{
			auto exp = SpatialMeshInfo::Op::Add;
			if (it.second.op.compare_exchange_strong(exp, SpatialMeshInfo::Op::None))
			{
				auto mesh = it.second.node->GetFirstSonByType("ModernMesh");
				mesh->setValue("Show", mShowMeshes);
				//kigsprintf("adding spatial mesh %u\n", mesh->getUID());
				attach->addItem(it.second.node.get());
			}

			exp = SpatialMeshInfo::Op::Remove;
			if (it.second.op.compare_exchange_strong(exp, SpatialMeshInfo::Op::None))
			{
				if (it.second.node)
				{
					attach->removeItem(it.second.node.get());
					//it.second.node->GetRef(); ///TEST
					//kigsprintf("deleting spatial mesh %u\n", it.second.node->GetFirstSonByType("ModernMesh")->getUID());
				}
				it.second.node = nullptr;
			}

			exp = SpatialMeshInfo::Op::Update;
			if (it.second.op.compare_exchange_strong(exp, SpatialMeshInfo::Op::None))
			{
				attach->removeItem(it.second.old_node.get());
				auto mesh = it.second.node->GetFirstSonByType("ModernMesh");
				mesh->setValue("Show", mShowMeshes);
				attach->addItem(it.second.node.get());
				//it.second.old_node->GetRef(); ///TEST
				//kigsprintf("deleting2 spatial mesh %u\n", it.second.old_node->GetFirstSonByType("ModernMesh")->getUID());
				/*for (auto& kv : it.second.old_node->GetFirstSonByType("ModernMesh")->GetLazyContentNoCreate()->ConnectedTo)
				{
					kigsprintf(" - %s\n", kv.first._id_name.c_str());
				}*/

				it.second.old_node = nullptr;
			}
			
		}
	}
}

void HoloSpatialMap::InitModifiable()
{
	ParentClassType::InitModifiable();
	
	mCollisionManager = CollisionManager::Get();
	mContinuousMatching = ContinuousMatching::Get();
	//mShowMeshes = true;

	if (mAllowed) return;

#ifdef KIGS_TOOLS
	mShowMeshes = true;

	auto import_scan = [&](const std::string& filename)
	{
		auto debugmap = Import(filename, false, false, nullptr, "debugmap");
		if (debugmap)
		{
			mat3x4 ref_matrix = mat3x4::IdentityMatrix();
			/*if (debugmap->getArrayValue("ReferenceMatrix", &ref_matrix.e[0][0], 12))
			{
				//ref_matrix = Inv(ref_matrix);
			}*/

			while (debugmap->getItems().size())
			{
				auto item = debugmap->getItems().front().myItem;
				item->GetRef();
				debugmap->removeItem(item);

				auto m = item->as<Node3D>()->GetLocal();
				m = ref_matrix * m;
				item->as<Node3D>()->ChangeMatrix(m);

				auto mesh = item->GetFirstSonByType("ModernMesh");
				mesh->AddDynamicAttribute(BOOL, "BVH", true);
				mesh->setValue("Show", mShowMeshes);
				mAttachNode->addItem(item);
				item->Destroy();
			}
			return;
		}
	};

	auto import_timed_scan = [&](const std::string& filename)
	{
		u64 len;
		auto crb = OwningRawPtrToSmartPtr(ModuleFileManager::Get()->LoadFile(filename.c_str(), len));
		if (crb)
		{
			PacketReadStream stream{ crb->data(), crb->size() };
			bool success = serialize_object(stream, gMapRecording);
			if (success)
			{
				ReplayRecordedSpatialMap();
			}
		}
	};

	//import_timed_scan("spatial_map_local_assoria.bin");

	//import_timed_scan("spatial_map_kcomk_antoine_cantine.bin");
	//import_timed_scan("spatial_map_kcomk_antoine_appart.bin");

	//import_timed_scan("spatial_map_kcomk_stephane_cantine.bin");
	//import_timed_scan("spatial_map_kcomk_stephane_appart.bin");
	
	
	//import_scan("spatial_map_1023_n1.xml");
	///import_scan("spatial_map_1028_n1.xml");
	//import_scan("spatial_map_1040_n1.xml");

	//import_scan("spacialmap.xml");
	//import_scan("local_assoria.xml");
	//import_scan("spatial_map_11156423923300.xml");

	//import_scan("couloir.xml");
	//import_scan("salle_de_classe.xml");
	//import_scan("grande_piece_1.xml");
	//import_scan("grande_piece_2.xml");
	
#endif
}

void HoloSpatialMap::SetShowMeshes(bool show)
{
	if (mShowMeshes != show)
	{
		mShowMeshes = show;

		std::set<CoreModifiable*> meshes;
		mAttachNode->GetSonInstancesByType("ModernMesh", meshes, true);
		for (auto m : meshes)
		{
			m->setValue("Show", mShowMeshes);
		}
	}
}

void HoloSpatialMap::SetObservedBoundingSphere(v3f pos, float radius)
{
	if (!mAllowed) return;

	Numerics::float3 p;
	p.x = pos.x;
	p.y = pos.y;
	p.z = pos.z;

	SpatialBoundingSphere bs;
	bs.Center = p;
	bs.Radius = radius;
	mSurfaceObserver.SetBoundingVolume(SpatialBoundingVolume::FromSphere(App::GetApp()->GetStationaryReferenceFrame().CoordinateSystem(), bs));

	
}

void HoloSpatialMap::SetObservedBoundingFrustum(Camera* camera)
{
	if (!mAllowed) return;

	auto planes = camera->GetFrustum();

	auto set_plane = [](const Camera::FrustumPlanes::Plane& p, winrt::Windows::Foundation::Numerics::plane& to_set)
	{
		to_set.d = -Dot(p.o, p.n);
		to_set.normal = { -p.n.x, -p.n.y, -p.n.z };
	};
	SpatialBoundingFrustum bf;
	set_plane(planes.Up, bf.Top);
	set_plane(planes.Down, bf.Bottom);
	set_plane(planes.Left, bf.Left);
	set_plane(planes.Right, bf.Right);
	set_plane(planes.Near, bf.Near);
	set_plane(planes.Far, bf.Far);

	mSurfaceObserver.SetBoundingVolume(SpatialBoundingVolume::FromFrustum(App::GetApp()->GetStationaryReferenceFrame().CoordinateSystem(), bf));
}

void HoloSpatialMap::TransformAllNodes(const mat3x4& mat)
{
#ifdef KIGS_TOOLS
	if (mRecordEnabled)
	{
		gMapRecording.frame_of_ref_changes.push_back({ std::chrono::steady_clock::now().time_since_epoch().count(), mat });
	}
#endif
	std::set<CoreModifiable*> nodes;
	mAttachNode->GetSonInstancesByType("Node3D", nodes);
	for (auto n : nodes)
	{
		n->as<Node3D>()->ChangeMatrix(mat * n->as<Node3D>()->GetLocal());
	}
}