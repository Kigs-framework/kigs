#pragma once
#include "CoreModifiable.h"
#include "maReference.h"

#ifdef USE_D3D
#include "HLSLGenericMeshShader.h"
#else
#include "GLSLGenericMeshShader.h"
#endif

#include "AttributePacking.h"

#include <atomic>
#include <thread>
#include <functional>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Perception.Spatial.Surfaces.h>

#include "concurrentqueue.h"
#include "blockingconcurrentqueue.h"

class CollisionManager;

struct SpatialMeshInfo
{
	enum class Op
	{
		None,
		InProcess,
		Add,
		Update,
		Remove
	};

	std::atomic<Op> op{ Op::None };
	SmartPointer<CoreModifiable> node;
	SmartPointer<CoreModifiable> old_node;
	//Windows::Foundation::IAsyncOperation<Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh^>^ op;
	long long time = 0;
	bool deleted;
};

struct GuidComp
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];

	template<typename PacketStream>
	bool Serialize(PacketStream& stream)
	{
		CHECK_SERIALIZE(serialize_object(stream, Data1));
		CHECK_SERIALIZE(serialize_object(stream, Data2));
		CHECK_SERIALIZE(serialize_object(stream, Data3));
		CHECK_SERIALIZE(serialize_bytes(stream, Data4, 8 * sizeof(u8)));
		return true;
	}

	inline friend bool operator==(const GuidComp& a, const GuidComp& b)
	{
		return memcmp(&a, &b, sizeof(GuidComp)) == 0;
	}
};

struct GuidHash
{
	std::size_t operator()(GuidComp const& s) const noexcept
	{
		size_t seed = 0;

		seed ^= std::hash<unsigned long>{}(s.Data1) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<unsigned short>{}(s.Data2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<unsigned short>{}(s.Data3) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		for (int i = 0; i < 8; ++i)
			seed ^= std::hash<u8>{}(s.Data4[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};

class Camera;

struct SpatialMapSurfaceRecord;
using namespace winrt::Windows::Foundation;

enum class ShowMeshMode
{
	Hidden,
	PlaneOnly,
	Full
};

class HoloSpatialMap : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(HoloSpatialMap, CoreModifiable, Renderer)
	DECLARE_CONSTRUCTOR(HoloSpatialMap);
	virtual ~HoloSpatialMap();

	WRAP_METHODS(StartListening, StopListening);

	void StartListening();
	void StopListening();

	void SetObservedBoundingSphere(v3f pos, float radius);
	void SetObservedBoundingFrustum(Camera* camera);

	void SetShowMeshMode(ShowMeshMode mode);
	ShowMeshMode GetShowMeshMode() const { return mShowMeshMode; }

	//void Export();

	bool IsRecording() { return mRecordEnabled; }
	void StartRecording();
	void StopRecording();

	winrt::Windows::Foundation::IAsyncAction ExportTimedScan(std::string filename = "");
	winrt::Windows::Foundation::IAsyncAction ResetTimedScan();

	void TransformAllNodes(const mat3x4& mat);

	void ImportTimedScan(const std::string& filename);
protected:
	void InitModifiable() override;

	winrt::Windows::Foundation::IAsyncAction ReplayRecordedSpatialMap();

	virtual void Update(const Timer& timer, void* addParam);

	void CreateMesh(winrt::Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh const& inMesh, SpatialMeshInfo& outInfo, SpatialMapSurfaceRecord* record_surface);
	
	CollisionManager* mCollisionManager = nullptr;
	
	maReference	mAttachNode = BASE_ATTRIBUTE(AttachNode, "Node3D:AttachMap");

	ShowMeshMode mShowMeshMode = ShowMeshMode::Hidden;

	winrt::Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver mSurfaceObserver = nullptr;

	winrt::event_token mEventToken;
	bool mIsListening = false;
	bool mAllowed = false;

	std::atomic_bool gInUpdate = false;

	maDouble mPrecision = BASE_ATTRIBUTE(Precision, 32.0);

	std::unordered_map<GuidComp, SpatialMeshInfo, GuidHash> mMeshList;
	std::mutex mListMtx;
	std::atomic_int mRecordUses{ 0 };
	std::atomic_bool mRecordEnabled{ false };


	moodycamel::BlockingConcurrentQueue<std::function<winrt::Windows::Foundation::IAsyncAction()>, moodycamel::ConcurrentQueueDefaultTraits > mToProcess;
	std::atomic_bool mContinueProcess;
	std::thread mProcessThread;
};


class HoloSpatialMapShader : public API3DGenericMeshShader
{
public:
	DECLARE_CLASS_INFO(HoloSpatialMapShader, API3DGenericMeshShader, Holo);
	DECLARE_INLINE_CONSTRUCTOR(HoloSpatialMapShader) {}
private:

#ifdef USE_D3D
	std::string GetFragmentShader() override
	{
		return ParentClassType::GetFragmentShader(); // TODO
	}
#else
	std::string GetFragmentShader() override
	{
		std::string result = ParentClassType::GetDefaultVaryings();
		result += ParentClassType::GetDefaultFragmentShaderUniforms();

		result += R"====(
void main()
{
	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
#ifdef HOLOGRAPHIC 
	float index = vRenderTargetArrayIndex;
#endif
}
)====";
		return result;
	}
#endif
};