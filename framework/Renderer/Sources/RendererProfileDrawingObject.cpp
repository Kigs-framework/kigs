#include "PrecompiledHeaders.h"
#include "RendererProfileDrawingObject.h"
#include "Core.h"
#include "ModuleRenderer.h"
#include "UIVerticesInfo.h"
#include "NotificationCenter.h"


using namespace Kigs::Draw;
using namespace Kigs::Draw2D;

#define MAX_FPS	(60.0f)

IMPLEMENT_CLASS_INFO(RendererProfileDrawingObject)

RendererProfileDrawingObject::RendererProfileDrawingObject(const std::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
, mFps(*this, false, "Fps", "30.0")
{

	// global timer is pure Red, don't use this color here

	// GREEN
	mColors[0].Set(0.0f,1.0f,0.0f);
	// BLUE
	mColors[1].Set(0.0f,0.0f,1.0f);
	// 
	mColors[2].Set(1.0f,1.0f,0.0f);
	//
	mColors[3].Set(0.0f,1.0f,1.0f);
	//
	mColors[4].Set(1.0f,0.0f,1.0f);
	//
	mColors[5].Set(0.5f,1.0f,0.5f);
	//
	mColors[6].Set(1.0f,0.5f,0.5f);
	//
	mColors[7].Set(0.5f,0.5f,1.0f);
	//
	mColors[8].Set(1.0f,1.0f,0.5f);
	mColors[9].Set(0.5f,1.0f,1.0f);

	// more than 10 timers ?
	int i;
	for(i=10;i<MAX_PROFILER_COUNT;i++)
	{
		mColors[i].Set((float)Platform_rand()/((float)RAND_MAX),(float)Platform_rand()/((float)RAND_MAX),(float)Platform_rand()/((float)RAND_MAX));
	}

	mFirstDraw=true;
	mProfilerCount=0;

	for (i = 0; i < 16; i++)
	{
		mMaxGlobalFrameCount[i] = 0;
	}
	mCurrentGlobalFrameCountIndex = 0;
	
}

bool	RendererProfileDrawingObject::DrawProfiles(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>&,void* privateParams)
{
	
	// retreive ModuleRenderer

	ModuleSpecificRenderer* renderer = (ModuleRenderer::mTheGlobalRenderer);
	//renderer->PushState();
	renderer->SetCullMode(RENDERER_CULL_NONE);
	renderer->SetLightMode(RENDERER_LIGHT_OFF);
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
	renderer->SetBlendMode(RENDERER_BLEND_ON);
	renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA,RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
	renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_OFF);
	renderer->SetDepthTestMode(false);

	// init ortho matrix
	renderer->PushMatrix(MATRIX_MODE_PROJECTION);
	renderer->Ortho(MATRIX_MODE_PROJECTION, 0, 1, 1, 0, 0, 1);

	renderer->PushMatrix(MATRIX_MODE_MODEL);
	renderer->LoadIdentity(MATRIX_MODE_MODEL);
	renderer->PushMatrix(MATRIX_MODE_VIEW);
	renderer->LoadIdentity(MATRIX_MODE_VIEW);

	renderer->FlushState();

	//double max=0.0;
	//double min=10000.0;
	//double sum=0.0;

	TimeProfiler* GLOBAL=0;

	int i;
	int index=1;
	int countprofilers=0;
	if(mFirstDraw)
	{
		printf("________________________________________________\n");
		printf("Profilers Colors settings : \n");
		printf("GLOBAL is always first one and Red \n");
	}

	// search global
	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		SP<TimeProfiler>	current=KigsCore::GetProfileManager()->mProfilers[i];
		if(current)
		{
			if(	current->getName() == "GLOBAL")
			{
				GLOBAL=current.get();
			}
		}
	}

	int frameCount=1;
	float globalftime= 1.0f/ MAX_FPS;

	// count how many frames for GLOBAL
	if(GLOBAL)
	{
		globalftime = GLOBAL->GetTime();

		if (globalftime <=0.0f)
		{
			globalftime = 0.00001f;
		}
		// compute fps
		float instantfps = 1.0f / globalftime;

		static float fps = MAX_FPS;
		fps = fps*0.8f + instantfps*0.2f;	// smooth

		frameCount = ceilf (MAX_FPS / instantfps);
		if (frameCount < 1)
		{
			frameCount = 1;
		}

		setGlobalFrameCount(frameCount);
		frameCount = getMaxFrameCount();

		char	fpsbuffer[32];
		sprintf(fpsbuffer, "%d fps", (int)fps);

		mFps = fpsbuffer;
		std::vector<CoreModifiableAttribute*> mySendParams;
		mySendParams.push_back(&mFps);
		KigsCore::GetNotificationCenter()->postNotificationName("SetFPS", mySendParams, this);
	}

	float oneOnFrameCount=1.0f/(float)(frameCount);

	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		SP<TimeProfiler>	current=KigsCore::GetProfileManager()->mProfilers[i];
		if(current)
		{
			if(	current->getName() != "GLOBAL")
			{
				if(current->WasUpdate())
				{
					double currenttime=current->GetTime();

					// compute y of the current profiler
					float h=((float)index*0.05f);
					// draw full length for 1/60 second
				
					float instantfps = 1.0f / currenttime;
					int lframeCount = ceilf(MAX_FPS / instantfps);
					if (lframeCount < 1)
					{
						lframeCount = 1;
					}

					int j;

					float startL=0.0f;

					//renderer->SetColor(mColors[index - 1].x, mColors[index - 1].y, mColors[index - 1].z, 0.5f);

					VInfo2D vi;
					UIVerticesInfo mVI = UIVerticesInfo(&vi);
					VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());

					for(j=1;j<lframeCount;j++)
					{
						buf[0].setVertex(startL + 0.01f, h);
						buf[1].setVertex(startL + 0.01f, h + 0.03f);
						buf[2].setVertex(startL + oneOnFrameCount, h + 0.03f);
						buf[3].setVertex(startL + oneOnFrameCount, h);


						buf[0].setColor(mColors[index - 1].x*255.0f, mColors[index - 1].y*255.0f, mColors[index - 1].z*255.0f, 128);
						buf[1].setColor(mColors[index - 1].x*255.0f, mColors[index - 1].y*255.0f, mColors[index - 1].z*255.0f, 128);
						buf[2].setColor(mColors[index - 1].x*255.0f, mColors[index - 1].y*255.0f, mColors[index - 1].z*255.0f, 128);
						buf[3].setColor(mColors[index - 1].x*255.0f, mColors[index - 1].y*255.0f, mColors[index - 1].z*255.0f, 128);

						mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Color);
//						renderer->DrawUIQuad(&mVI);
						startL+=oneOnFrameCount;
					}

					float remaining = (currenttime*MAX_FPS - (float)(lframeCount - 1))*oneOnFrameCount;

					if (remaining < 0.0f)
					{
						remaining = 0.0f;
					}


					buf[0].setVertex(startL + 0.01f, h);
					buf[1].setVertex(startL + 0.01f, h + 0.03f);
					buf[2].setVertex(startL + remaining + 0.01f, h + 0.03f);
					buf[3].setVertex(startL + remaining + 0.01f, h);

					buf[0].setColor(mColors[index - 1].x*255.0f, mColors[index - 1].y*255.0f, mColors[index - 1].z*255.0f, 128);
					buf[1].setColor(mColors[index - 1].x*255.0f, mColors[index - 1].y*255.0f, mColors[index - 1].z*255.0f, 128);
					buf[2].setColor(mColors[index - 1].x*255.0f, mColors[index - 1].y*255.0f, mColors[index - 1].z*255.0f, 128);
					buf[3].setColor(mColors[index - 1].x*255.0f, mColors[index - 1].y*255.0f, mColors[index - 1].z*255.0f, 128);

					mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Color);
//					renderer->DrawUIQuad(&mVI);
					current->ClearUpdate();
				}
				if(mFirstDraw)
				{
					printf("Profilers %d %s is R(%d),G(%d),B(%d) \n",i,current->getName().c_str(),(int)(mColors[index-1].x*256.0f),(int)(mColors[index-1].y*256.0f),(int)(mColors[index-1].z*256.0f));
				}
				index++;
				countprofilers++;
			}
			
			
		}
	}

	if(0!=GLOBAL)
	{
		if(GLOBAL->WasUpdate())
		{	
			
			// global is drawn first, in red
			//glColor4f(1.0f,0.0f,0.0f,0.5f);
			//renderer->SetColor(1.0f,0.0f,0.0f,0.5f);
			renderer->FlushState();
			// draw frameCount-1 full "blocks" + final block

			float globalftime = GLOBAL->GetTime();
			if (globalftime <= 0.0f)
			{
				globalftime = 0.00001f;
			}
			// compute fps

			float instantfps = 1.0f / globalftime;
			int lframeCount = ceilf(MAX_FPS / instantfps);
			if (lframeCount < 1)
			{
				lframeCount = 1;
			}

			VInfo2D vi;
			UIVerticesInfo mVI = UIVerticesInfo(&vi);
			VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());

			float startL=0.0f;
			for(i=1;i<lframeCount;i++)
			{
				buf[0].setVertex(startL + 0.01f, 0.01f);
				buf[1].setVertex(startL + 0.01f, 0.04f);
				buf[2].setVertex(startL + oneOnFrameCount, 0.04f);
				buf[3].setVertex(startL + oneOnFrameCount, 0.01f);


				buf[0].setColor(255, 0, 0, 128);
				buf[1].setColor(255, 0, 0, 128);
				buf[2].setColor(255, 0, 0, 128);
				buf[3].setColor(255, 0, 0, 128);

				mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Color);
//				renderer->DrawUIQuad(&mVI);

				startL+=oneOnFrameCount;
			}
			float remaining= (globalftime*MAX_FPS - (float)(lframeCount -1))*oneOnFrameCount;

			if (remaining < 0.0f)
			{
				remaining = 0.0f;
			}

			buf[0].setVertex(startL + 0.01f, 0.01f);
			buf[1].setVertex(startL + 0.01f, 0.04f);
			buf[2].setVertex(startL + remaining+0.01f, 0.04f);
			buf[3].setVertex(startL + remaining+0.01f, 0.01f);
			
			buf[0].setColor(255, 0, 0, 128);
			buf[1].setColor(255, 0, 0, 128);
			buf[2].setColor(255, 0, 0, 128);
			buf[3].setColor(255, 0, 0, 128);

			mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Color);
//			renderer->DrawUIQuad(&mVI);

			
			GLOBAL->ClearUpdate();

		}
	}

	//renderer->PopState();

	renderer->PopMatrix(MATRIX_MODE_PROJECTION);
	renderer->PopMatrix(MATRIX_MODE_VIEW);
	renderer->PopMatrix(MATRIX_MODE_MODEL);
	
	if(mFirstDraw)
	{
		printf("________________________________________________\n");

		mFirstDraw=false;
	}
	if(!mFirstDraw)
	{
		if(mProfilerCount!=countprofilers)
		{
			mProfilerCount=countprofilers;
			mFirstDraw=true;
		}
	}
	else
	{
		mProfilerCount=countprofilers;
	}	

	return true;
}