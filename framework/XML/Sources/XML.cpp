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

XMLBase::XMLBase(CoreRawBuffer* buffer) : m_ReadedRawBuffer(buffer)
{
	m_ReadedRawBuffer->GetRef();
	m_version = "1.0";
	m_encoding = "";
	m_standalone = "";
}

XMLBase::~XMLBase()
{
	if (m_root)
	{
		delete m_root;
		m_root = nullptr;
	}
	if (m_ReadedRawBuffer)
	{
		m_ReadedRawBuffer->Destroy();
		m_ReadedRawBuffer = nullptr;
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


