
void RendererOpenGL::PlatformInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
}

void RendererOpenGL::PlatformUpdate(const Timer& timer, void* addParam)
{
}

void RendererOpenGL::PlatformClose()
{
}


// # Draw functions
void	RendererOpenGL::DrawArrays(TravState* state, unsigned int mode, int first, int count)
{
	CHECK_GLERROR;

	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);

	glDrawArrays(mode, first, count);

	myVertexBufferManager->Clear();
}

void	RendererOpenGL::DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, bool clear_manager)
{
	CHECK_GLERROR;

	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);

	glDrawElements(mode, count, type, indices);
	if(clear_manager)
		myVertexBufferManager->Clear();
}

void RendererOpenGL::DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount, bool clear_manager)
{
	CHECK_GLERROR;
	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);

	glDrawElementsInstanced(mode, count, type, indices, primcount);
	if (clear_manager)
		myVertexBufferManager->Clear(true);
}