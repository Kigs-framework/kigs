#include "PrecompiledHeaders.h"
#include "XML.h"
#include "XMLNode.h"
#include "XMLReaderFile.h"
#include "XMLWriterFile.h"
#include "CoreRawBuffer.h"


void	XMLBase::WriteFile(const std::string& filename)
{
	if(useStringRef())
	{
		KIGS_ERROR("trying to write string_view XML",2);
	}
	else
	{
		XMLWriterFile::WriteFile(filename, *(XML*)this);
	}
}

XMLBase::XMLBase(CoreRawBuffer* buffer) : mReadedRawBuffer(buffer)
{
	mReadedRawBuffer->GetRef();
	mVersion = "1.0";
	mEncoding = "";
	mStandalone = "";
}

XMLBase::~XMLBase()
{
	if (mRoot)
	{
		delete mRoot;
		mRoot = nullptr;
	}
	if (mReadedRawBuffer)
	{
		mReadedRawBuffer->Destroy();
		mReadedRawBuffer = nullptr;
	}

}


XMLBase* XMLBase::ReadFile(const std::string& filename, const char* force_as_format)
{
	return XMLReaderFile::ReadFile(filename, force_as_format);
}

bool XMLBase::ReadFile(const std::string& filename, CoreModifiable* delegateObject, const char* force_as_format)
{
	return XMLReaderFile::ReadFile(filename, delegateObject, force_as_format);
}


