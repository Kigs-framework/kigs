
void RendererOpenGL::PlatformInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
}

void RendererOpenGL::PlatformUpdate(const Timer&, void*)
{
}

void RendererOpenGL::PlatformClose()
{
}

// # Draw functions
void	RendererOpenGL::DrawArrays(TravState* state,unsigned int mode, int first, int count)
{
	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);
	myVertexBufferManager->FlushUnusedVertexAttrib();
	glDrawArrays(mode, first, count);

	myVertexBufferManager->MarkVertexAttrib();
}

void	RendererOpenGL::DrawElements(TravState* state,unsigned int mode, int count, unsigned int type, void* indices)
{
	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);
	myVertexBufferManager->FlushUnusedVertexAttrib();
	glDrawElements(mode, count, type, indices);
	
	myVertexBufferManager->MarkVertexAttrib();
}

void RendererOpenGL::DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount)
{
	KIGS_ASSERT(false);
	/*
	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);
	CHECK_GLERROR;

	glDrawElementsInstanced(mode, count, type, indices, primcount);
	CHECK_GLERROR;

	if (clear_manager)
		myVertexBufferManager->Clear();
	*/
}