
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

	myVertexBufferManager->FlushUnusedVertexAttrib();

	glDrawArrays(mode, first, count);

	myVertexBufferManager->MarkVertexAttrib();
}

void	RendererOpenGL::DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices)
{
	CHECK_GLERROR;

	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);

	myVertexBufferManager->FlushUnusedVertexAttrib();

	glDrawElements(mode, count, type, indices);
	
	myVertexBufferManager->MarkVertexAttrib();
}

void RendererOpenGL::DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount)
{
	CHECK_GLERROR;
	myVertexBufferManager->FlushBindBuffer();
	FlushState();
	FlushMatrix(state);

	myVertexBufferManager->FlushUnusedVertexAttrib();

	glDrawElementsInstanced(mode, count, type, indices, primcount);
	
	myVertexBufferManager->MarkVertexAttrib();
}