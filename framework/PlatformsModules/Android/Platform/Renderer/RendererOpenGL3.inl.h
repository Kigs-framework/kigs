
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

	glDrawArrays(mode, first, count);

	myVertexBufferManager->Clear();
}

void	RendererOpenGL::DrawElements(TravState* state,unsigned int mode, int count, unsigned int type, void* indices, bool clear_manager)
{
	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);

	glDrawElements(mode, count, type, indices);
	
	if (clear_manager)
		myVertexBufferManager->Clear();
}

void RendererOpenGL::DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount, bool clear_manager)
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