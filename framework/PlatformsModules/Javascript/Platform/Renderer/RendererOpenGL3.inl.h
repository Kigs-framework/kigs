

static unsigned int IndexBuffer = UINT32_MAX;

void RendererOpenGL::PlatformInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	
}

void RendererOpenGL::PlatformUpdate(const Timer& timer, void* addParam)
{
	
}

void RendererOpenGL::PlatformClose()
{
	glDeleteBuffers(1, &IndexBuffer);
}


// # Draw functions
void	RendererOpenGL::DrawArrays(TravState* state, unsigned int mode, int first, int count)
{
	mVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);
	mVertexBufferManager->FlushUnusedVertexAttrib();
	glDrawArrays(mode, first, count);
	
	mVertexBufferManager->MarkVertexAttrib();
}

void	RendererOpenGL::DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices)
{
	mVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);
	mVertexBufferManager->FlushUnusedVertexAttrib();
	glDrawElements(mode, count, type, indices);
	mVertexBufferManager->MarkVertexAttrib();
}

void RendererOpenGL::DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount)
{
	KIGS_ASSERT(false);
	/*
	mVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);
	CHECK_GLERROR;

	glDrawElementsInstanced(mode, count, type, indices, primcount);
	CHECK_GLERROR;

	if (clear_manager)
		mVertexBufferManager->Clear();
	*/
}