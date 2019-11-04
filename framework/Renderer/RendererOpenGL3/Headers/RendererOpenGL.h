#ifndef _MODULE3DENGINEOPENGL_H_
#define _MODULE3DENGINEOPENGL_H_

#include "ModuleBase.h"
#include "ModuleRenderer.h"
#include "GLSLUniform.h"

class RendererOpenGL;
class TravState;

//#define ENABLE_CHECKERROR
#ifdef ENABLE_CHECKERROR
#define CHECK_GLERROR RendererOpenGL::CheckError(__FILE__,__LINE__);
#else 
#define CHECK_GLERROR
#endif


class FreeType_TextDrawer;
class VertexBufferManager;
class API3DShader;


#define PREALLOCATED_VBO_COUNT 64

class OpenGLRenderingState : public RenderingState
{
public:
	OpenGLRenderingState() :RenderingState() {}

	void FlushState(RenderingState* currentState, bool force = false) override;

	void ClearView(RendererClearMode clearMode) override;

	void FlushLightModelfv(RenderingState* currentState) override { assert(0); }
	void FlushLightModeli(RenderingState* currentState)  override { assert(0); }

	void Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
	
	void ProtectedInitHardwareState() override;
};


class VertexBufferManager : public VertexBufferManagerBase
{
public:
	VertexBufferManager();

	void GenBuffer(int count, unsigned int * id) override;
	void DelBuffer(int count, unsigned int * id) override;
	void DelBufferLater(int count, unsigned int * id) override;

	void BufferData(unsigned int bufferName, unsigned int bufftype, int size, void* data, unsigned int usage) override;

	void UnbindBuffer(unsigned int bufferName, int target = 0) override;
	void FlushBindBuffer(int target = 0, bool force = false) override;
	void Clear(bool push = false) override;

	void SetArrayBuffer(unsigned int bufferName, int slot_unused = 0) override;
	void SetElementBuffer(unsigned int bufferName) override;
	void SetVertexAttrib(unsigned int bufferName, unsigned int attribID, int size, unsigned int type, bool normalized, unsigned int stride, void * offset, const Locations* locs = nullptr) override;

	void DoDelayedAction() override;

private:
	void internalBindBuffer(unsigned int bufferName, unsigned int bufftype);

	// store the current buffer bound
	unsigned int mCurrentBoundBuffer[2];
	unsigned int mAskedBoundBuffer[2];

	std::vector<u32> mEnableVertexAttrib;
	std::vector<u32> mToDeleteBuffer;
};


class RendererOpenGL : public ModuleSpecificRenderer
{
public:
	struct HoloMatrix
	{
		u32 mRenderTargetArrayIndices;
		u32 mRenderTargetArrayIndicesCount;
		float			mHoloMatrix[16 * 2];
	};

	DECLARE_CLASS_INFO(RendererOpenGL, ModuleSpecificRenderer, Renderer);
	DECLARE_CONSTRUCTOR(RendererOpenGL);

	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	void PlatformInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

	void Close() override;
	void PlatformClose() override;

	void Update(const Timer& timer, void* addParam) override;
	void PlatformUpdate(const Timer& timer, void* addParam) override;

	static void CheckError(const char* filename,int line);

	void FlushState(bool force = false) override;
	
	// Direct rendering method for UI
	void DrawUIQuad(TravState * state, const UIVerticesInfo * qi) override;
	void DrawUITriangles(TravState * state, const UIVerticesInfo * qi) override;

	static ModuleSpecificRenderer *	theGlobalRenderer;
	static FreeType_TextDrawer*		myDrawer;


	virtual void	setCurrentShaderProgram(ShaderInfo* p) override;

	void DrawPendingInstances(TravState* state) override;

	void ProtectedFlushMatrix(TravState* state) override;

	void DrawArrays(TravState* state, unsigned int mode, int first, int count) override;
	void DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices = 0, bool clear_manager=true) override;
	void DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount, bool clear_manager = true) override;

	void BindArrayBuffer(unsigned int id);
	void BindElementBuffer(unsigned int id);

	void SetVertexAttribDivisor(TravState* state, unsigned int bufferName, int attribute_location, int divisor) override;
	
	void InitTextureInfo() ;
	void CreateTexture(int count, unsigned int * id) override;
	void DeleteTexture(int count, unsigned int * id) override;
	void EnableTexture() override;
	void DisableTexture() override;
	void BindTexture(RendererTextureType type, unsigned int ID) override;
	void UnbindTexture(RendererTextureType type, unsigned int ID) override;

	void TextureParameteri(RendererTextureType type, RendererTexParameter1 name, RendererTexParameter2 param) override;
	
	void SetUniform1i(unsigned int loc, s32 value);
	void ActiveTextureChannel(unsigned int channel) override;

	void SetLightsInfo(kstl::set<CoreModifiable*>*lights)  override;
	void SendLightsInfo(TravState* travstate) override;
	void ClearLightsInfo(TravState* travstate) override;

protected:
	
	// Texture Unit
	int MAX_TEXTURE_UNIT;
	struct TextureUnitInfo
	{
		unsigned int BindedTextureID;
		unsigned int BindedTextureType;
	};
	std::vector<TextureUnitInfo>	myTextureUnit;

	~RendererOpenGL() override;
	RenderingState*	createNewState(RenderingState* toCopy = 0)  override
	{
		OpenGLRenderingState* newstate = new OpenGLRenderingState();

		if (toCopy)
			*newstate = *static_cast<OpenGLRenderingState*>(toCopy);

		return newstate;
	}

	unsigned int			myCurrentOGLMatrixMode;

	HoloMatrix * mHoloMatrix = nullptr;
};


#endif //_MODULE3DENGINEOPENGL_H_
