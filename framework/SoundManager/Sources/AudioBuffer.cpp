//#include "PrecompiledHeaders.h"
#include "AudioBuffer.h"
#include "FilePathManager.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(AudioBuffer)

AudioBuffer::AudioBuffer(const kstl::string& name,CLASS_NAME_TREE_ARG)
: Drawable(name,PASS_CLASS_NAME_TREE_ARG)
, myFileName(*this,true,LABEL_AND_ID(FileName))
{
	
}     

AudioBuffer::~AudioBuffer() {}

void AudioBuffer::InitModifiable()
{
	Drawable::InitModifiable();
	if(_isInit)
	{
		FilePathManager* pathManager= (FilePathManager*)KigsCore::GetSingleton(_S_2_ID("FilePathManager"));
		
		kstl::string fullfilename;
		SmartPointer<FileHandle> fullfilenamehandle;
		fullfilenamehandle = pathManager->FindFullName(myFileName);

		if (fullfilenamehandle)
		{
			if (!LoadSound(fullfilenamehandle->myFullFileName))
			{
				UninitModifiable();
			}
		}
	}
}
