
namespace Kigs
{
	namespace Draw
	{
		void RendererOpenGL::PlatformInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
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

			mVertexBufferManager->FlushBindBuffer();
			FlushState();
			FlushMatrix(state);

			mVertexBufferManager->FlushUnusedVertexAttrib();

			glDrawArrays(mode, first, count);

			mVertexBufferManager->MarkVertexAttrib();

#ifdef KIGS_TOOLS
			gRendererStats.DrawCalls += 1;
			gRendererStats.DrawCallsTriangleCount += count / 3;
#endif
		}

		void	RendererOpenGL::DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices)
		{
			CHECK_GLERROR;

			mVertexBufferManager->FlushBindBuffer();
			FlushState();
			FlushMatrix(state);

			mVertexBufferManager->FlushUnusedVertexAttrib();

			glDrawElements(mode, count, type, indices);

			mVertexBufferManager->MarkVertexAttrib();
#ifdef KIGS_TOOLS
			gRendererStats.DrawCalls += 1;
			gRendererStats.DrawCallsTriangleCount += count / 3;
#endif
		}

		void RendererOpenGL::DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount)
		{
			CHECK_GLERROR;
			mVertexBufferManager->FlushBindBuffer();
			FlushState();
			FlushMatrix(state);

			mVertexBufferManager->FlushUnusedVertexAttrib();

			glDrawElementsInstanced(mode, count, type, indices, primcount);

			mVertexBufferManager->MarkVertexAttrib();
#ifdef KIGS_TOOLS
			gRendererStats.DrawCalls += 1;
			gRendererStats.DrawCallsTriangleCount += primcount * count / 3;
#endif
		}

	}
}