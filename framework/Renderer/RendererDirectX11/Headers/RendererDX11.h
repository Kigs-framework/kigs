#pragma once

#include "ModuleBase.h"
#include "ModuleRenderer.h"
#include "SceneGraphDefines.h"

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

#ifdef WUP
#include <winrt/Windows.Graphics.Holographic.h>
#include <winrt/Windows.Perception.Spatial.h>
#include "winrt/Windows.UI.Core.h"
#endif

class RendererDX11;

#ifdef _KIGS_ONLY_STATIC_LIB_
#else
#ifdef BUILDING_RENDERER_DLL
#define  __declspec(dllexport)
#else
#define  __declspec(dllimport)
#endif
#endif


namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
}

struct MaterialStruct
{
	v4f MaterialDiffuseColor;
	v4f MaterialSpecularColor;
	v4f MaterialAmbiantColor;
	float MaterialShininess;

	bool operator==(const MaterialStruct& other) const
	{ 
		return MaterialDiffuseColor == other.MaterialDiffuseColor 
			&& MaterialSpecularColor == other.MaterialSpecularColor 
			&& MaterialAmbiantColor == other.MaterialAmbiantColor 
			&& MaterialShininess == other.MaterialShininess;
	}
};


struct DXInstance
{
#ifdef WUP
	winrt::com_ptr<ID3D11Device1> mDevice;
	winrt::com_ptr<ID3D11DeviceContext1> mDeviceContext;
	winrt::com_ptr<IDXGISwapChain1> mSwapChain;
	
	winrt::com_ptr<ID3D11DepthStencilState> mDepthStencilState;
	winrt::com_ptr<ID3D11RasterizerState> mRasterState;
	winrt::com_ptr<ID3D11RenderTargetView> mCurrentRenderTarget;
	winrt::com_ptr<ID3D11DepthStencilView> mCurrentDepthStencilTarget;
#else
	Microsoft::WRL::ComPtr<ID3D11Device1> mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> mDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> mSwapChain;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterState;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mCurrentRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mCurrentDepthStencilTarget;
#endif
	bool mIsFBORenderTarget = false;

	ID3D11Buffer* mMatrixBuffer = nullptr;
	ID3D11Buffer* mLightBuffer = nullptr;
	size_t mCurrentLightBufferSize = 0;
	size_t mCurrentLightBufferHash = 0;
	
	ID3D11Buffer* mFogBuffer = nullptr;

	ID3D11Buffer* mMaterialBuffer = nullptr;
	//size_t m_material_buffer_hash = 0;
	MaterialStruct mCurrentMaterial = {};

#ifdef WUP
	winrt::Windows::Graphics::Holographic::HolographicFrame mCurrentFrame{ nullptr };
	
	//winrt::Windows::Graphics::Holographic::HolographicFramePrediction mPrediction{ nullptr };
	//winrt::Windows::Graphics::Holographic::HolographicCameraRenderingParameters mCurrentRenderingParameters{ nullptr };

	winrt::Windows::UI::Core::CoreWindow mWindow{ nullptr };
	winrt::Windows::Graphics::Holographic::HolographicSpace mHolographicSpace{ nullptr };
	winrt::Windows::Perception::Spatial::SpatialLocator mSpatialLocator{ nullptr };
	winrt::Windows::Perception::Spatial::SpatialStationaryFrameOfReference mStationaryReferenceFrame{ nullptr };
#endif

};

enum DX11_CBUFFER_SLOTS
{
	DX11_MATRIX_SLOT = 0,
	DX11_LIGHT_SLOT = 1,
	DX11_MATERIAL_SLOT = 2,
	DX11_FOG_SLOT = 3,
};

enum DX11_TEXTURE_SLOTS
{
	DX11_COLOR_MAP_SLOT = 0,
	DX11_NORMAL_MAP_SLOT = 1,
};

// ****************************************
// * VertexBufferManager class
// * --------------------------------------
/**
 * \file	RendererDX11.h
 * \class	VertexBufferManager
 * \ingroup Renderer
 * \brief	Manage vertex buffer for DX11 API.
 */
 // ****************************************

class VertexBufferManager : public VertexBufferManagerBase
{
public:
	VertexBufferManager();
	virtual ~VertexBufferManager();

	void GenBuffer(int count, unsigned int * id) override;
	void DelBuffer(int count, unsigned int * id) override;
	void DelBufferLater(int count, unsigned int * id) override;

	void BufferData(unsigned int bufferName, unsigned int bufftype, int size, void* data, unsigned int usage) override;

	void UnbindBuffer(unsigned int bufferName, int target = 0) override;
	void FlushBindBuffer(int target = 0, bool force = false) override;
	void Clear(bool push = false) override;

	void SetArrayBuffer(unsigned int bufferName, int slot=0) override;
	void SetElementBuffer(unsigned int bufferName) override;
	void SetVertexAttrib(unsigned int bufferName, unsigned int attribID, int size, unsigned int type, bool normalized, unsigned int stride, void * offset, const Locations* locs=nullptr) override;
	void SetVertexAttribDivisor(unsigned int bufferName, int attributeID, int divisor) override;

	void DoDelayedAction() override;

	size_t GetAllocatedBufferCount() override;

	size_t GetCurrentLayoutHash();
	D3D11_INPUT_ELEMENT_DESC* CreateLayoutDesc(int& descsize);
	void ClearCurrentLayout();

	std::vector<ID3D11Buffer*> GetBoundBuffersList() const
	{
		std::vector<ID3D11Buffer*> result;
		for (int i = 0; i < mCurrentBoundVertexBuffer.size(); ++i)
		{
			if (mCurrentBoundVertexBuffer[i] == -1) continue;
			result.push_back(mBufferList[mCurrentBoundVertexBuffer[i]].mI3DBuffer);
		}
		return result;
	}
	ID3D11Buffer* GetIBuffer() const
	{
		return mBufferList[mCurrentBoundElementBuffer].mI3DBuffer;
	}

	std::vector<unsigned int> GetBoundBuffersStride()
	{
		std::vector<unsigned int> result;
		for (int i = 0; i < mCurrentBoundVertexBuffer.size(); ++i)
		{
			if (mCurrentBoundVertexBuffer[i] == -1) continue;
			result.push_back(mBufferList[mCurrentBoundVertexBuffer[i]].mBufferStride);
		}
		return result;
	}

	void internalBindBuffer(unsigned int bufferName, unsigned int bufftype, int slot=0);

	// store the current buffer bound

	std::array<unsigned int, 4> mCurrentBoundVertexBuffer;
	//std::array<unsigned int, 4> mCurrentAskedVertexBuffer;
	
	unsigned int mCurrentBoundElementBuffer;
	//unsigned int mCurrentAskedElementBuffer;

	struct InputElementDesc
	{
		unsigned int elemCount;
		unsigned int elemType;
		void* offset;
		int step_rate = 0;
	};

	struct vbufferStruct
	{
		ID3D11Buffer*								mI3DBuffer;
		D3D11_BUFFER_DESC*							mDesc;
		std::map<unsigned int, InputElementDesc>	mLayoutDesc;
		unsigned int								mBufferStride;
		bool										mLayoutDescWasChanged;
	};

	std::vector<vbufferStruct>	mBufferList;
	int							mCountFree = 0;

	//std::vector<u32> mEnableVertexAttrib;
	std::vector<u32> mToDeleteBuffer;
};


struct MatrixBufferType
{
	Matrix4x4 model;
	
	Matrix4x4 uvMatrix;

	union
	{
		struct
		{
			Matrix4x4 viewproj;
		};
		struct
		{
			Matrix4x4 stereo_viewproj[2];
		};
	};
};


class DX11RenderingState : public RenderingState
{
public:
	DX11RenderingState() :RenderingState() {}
	virtual ~DX11RenderingState();

	void FlushState(RenderingState* currentState, bool force = false);

	void ClearView(RendererClearMode clearMode) override;
	void Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;


	void ProtectedInitHardwareState();

	virtual void FlushLightModeli(RenderingState* currentState) {};
	virtual void FlushLightModelfv(RenderingState* currentState) {};

	
	void manageBlend(DX11RenderingState* currentState);
	void manageDepthStencilTest(DX11RenderingState* currentState);
	void manageRasterizerState(DX11RenderingState* currentState);

};

struct OcclusionQuery
{
	ID3D11Query* query = nullptr;
	RendererQueryType type = RENDERER_QUERY_SAMPLES_PASSED;
	int frame_of_execution = -1;
	int frames_to_keep = 1;
	UINT64 result = 0;
	bool result_ok = false;
};


// ****************************************
// * RendererDX11 class
// * --------------------------------------
/**
 * \file	RendererDX11.h
 * \class	RendererDX11
 * \ingroup Renderer
 * \ingroup Module
 * \brief	DX11 implementation of ModuleSpecificRenderer.
 */
 // ****************************************
class RendererDX11 : public ModuleSpecificRenderer
{
public:
	DECLARE_CLASS_INFO(RendererDX11, ModuleSpecificRenderer, Renderer)
	DECLARE_CONSTRUCTOR(RendererDX11);

	void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
	void PlatformInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

	void Close() override;
	void PlatformClose();

	void Update(const Timer& timer, void* addParam) override;
	void PlatformUpdate(const Timer& timer, void* addParam);
	
	void FlushState(bool force = false) override;

	void startFrame(TravState* state) override;
	void endFrame(TravState* state) override;

	// Direct rendering method for UI
	virtual void DrawUIQuad(TravState * state, const UIVerticesInfo * qi) override;
	virtual void DrawUITriangles(TravState * state, const UIVerticesInfo * qi) override;

	virtual void InitLayerDraw(Scene3D * scene) { /* nothing here */ }

	void DrawPendingInstances(TravState * state) override;

	void ProtectedFlushMatrix(TravState* state) override;

	void DrawArrays(TravState* state, unsigned int mode, int first, int count) override;
	void DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices = 0) override;
	void DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount) override;


	//void BindArrayBuffer(unsigned int id);
	//void BindElementBuffer(unsigned int id);

	void SetVertexAttribDivisor(TravState* state, unsigned int bufferName, int attribute_location, int divisor) override;

	void InitTextureInfo() {}
	void CreateTexture(int count, unsigned int * id) override;
	void DeleteTexture(int count, unsigned int * id) override;
	void EnableTexture() override;
	void DisableTexture() override;
	void BindTexture(RendererTextureType type, unsigned int ID) override;
	void UnbindTexture(RendererTextureType type, unsigned int ID) override;

	void TextureParameteri(RendererTextureType type, RendererTexParameter1 name, RendererTexParameter2 param) override;


	void SetUniform1i(unsigned int loc, s32 value) {}
	void ActiveTextureChannel(unsigned int channel) override { mCurrentTextureChannel = channel; }

	u32 GetActiveTextureChannel() const { return mCurrentTextureChannel; }

	ModuleSpecificRenderer::LightCount SetLightsInfo(std::set<CoreModifiable*>*lights) override;
	void SendLightsInfo(TravState* travstate) override;
	void ClearLightsInfo(TravState* travstate) override;

	DXInstance* getDXInstance() { return &mDXInstance; }
	
	std::unordered_map<size_t, ID3D11BlendState*>& BlendStateList() { return mBlendStateList; }
	std::unordered_map<size_t, ID3D11DepthStencilState*>& DepthStateList() { return mDepthStateList; }
	std::unordered_map<size_t, ID3D11RasterizerState*>& RasterizerStateList() { return mRasterizerStateList; }
	std::unordered_map<size_t, ID3D11SamplerState*>& SamplerStateList() { return mSamplerStateList; }

	void SetSampler(bool repeatU, bool repeatV, bool forceNearest);

	v2u GetCurrentViewportSize() const { return mCurrentViewportSize; }
	void SetCurrentViewportSize(v2u s) { mCurrentViewportSize = s; }


	bool    BeginOcclusionQuery(TravState* state, u64& query_id, RendererQueryType type, int frames_to_keep = 1) override;
	void    EndOcclusionQuery(TravState* state, u64 query_id) override;
	bool    GetOcclusionQueryResult(TravState* state, u64 query_id, u64& result, int frames_to_extend_if_not_ready = 0) override;

protected:
	
	bool CreateDevice();

	RenderingState*	createNewState(RenderingState* toCopy = 0) override
	{
		DX11RenderingState* newstate = new DX11RenderingState();
		if (toCopy)
		{
			*newstate = *static_cast<DX11RenderingState*>(toCopy);
		}
		return newstate;
	}

	DXInstance mDXInstance;


	bool					mIsLayoutInitialized = false;
	unsigned int			mCurrentMatrixMode = 0xffffffff;
	static unsigned int		mDirtyShaderMatrix;	// set when shader has changed, so we have to push again matrix

	unsigned int mCurrentTextureChannel = 0;

	std::unordered_map<size_t, ID3D11BlendState*> mBlendStateList;
	std::unordered_map<size_t, ID3D11DepthStencilState*> mDepthStateList;
	std::unordered_map<size_t, ID3D11RasterizerState*> mRasterizerStateList;
	std::unordered_map<size_t, ID3D11SamplerState*> mSamplerStateList;

	std::vector<OcclusionQuery> mOcclusionQueries[RENDERER_QUERY_TYPE_COUNT];
	size_t mFreeQueryCount[RENDERER_QUERY_TYPE_COUNT] = {};
	u64 mOcclusionQueriesForFrame = 0;

	v2u mCurrentViewportSize;
};
