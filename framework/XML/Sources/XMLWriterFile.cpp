#include "PrecompiledHeaders.h"
#include "XMLWriterFile.h"
#include "XML.h"
#include "XMLNode.h"
#include "XMLAttribute.h"
#include "Core.h"
#include "CoreRawBuffer.h"

#include "FilePathManager.h"
#include "AttributePacking.h"


void XMLWriterFile::appendString( std::string &sAppend, bool isElementText )
{
	mData.emplace_back();
	auto& s = mData.back();

    if ( !isElementText )
    {
        s = sAppend;
    }
    else
    {
		s.reserve(sAppend.size() + 1024);
		for ( int i = 0; i < (int) sAppend.size( ); ++i )
        {
            switch ( sAppend.at( i ) )
            {
				case '&':
                s += "&amp;";
                break;
				
				case '<':
                s += "&lt;";
                break;
				
				case '>':
                s += "&gt;";
                break;
				
				case '"':
                s += "&quot;";
                break;
				
				case '\'':
                s += "&apos;";
                break;
				
				default:
                s += sAppend.at(i);
                break;
            }
        }
    }
}

void XMLWriterFile::writeHeader( XML &xml )
{
    std::string s;
    s += "<?xml";
	
    if ( xml.getVersion( ) != "" )
    {
        s += " version=\"";
        s += xml.getVersion( );
        s += "\"";
    }
	
    if ( xml.getEncoding( ) != "" )
    {
        s += " encoding=\"";
        s += xml.getEncoding( );
        s += "\"";
    }
	
    s += "?>";
    appendString(s);
}

void XMLWriterFile::writeNode( XMLNode *node, int tab )
{
    if ( !node )
    {
        return;
    }
	
    int i;
    std::string s;
	
    switch ( node->getType( ) )
    {
		case XML_NODE_ELEMENT:
        s += '\n';
        
        s += std::string(tab, '\t');
        
		
        s += "<";
        s += node->XMLNodeBase::getName( );
		
        for ( i = 0; i < node->getAttributeCount( ); ++i )
        {
            s += " ";
            s += node->getAttribute( i )->getName( );
            s += "=\"";
            s += node->getAttribute( i )->getString( );
            s += "\"";
        }
		
        if ( node->getChildCount( ) == 0 )
        {
            s += "/>";
        }
        else
        {
            s += ">";
        }
		
        appendString( s );
        break;
		
		case XML_NODE_TEXT:
		
		s += node->XMLNodeBase::getString();
		
        appendString( s, true );
        break;
		
		case XML_NODE_TEXT_NO_CHECK:
		s += '\n';
		s += "<![CDATA[";
        s += node->XMLNodeBase::getString();
		s += "]]>";
        appendString( s );
        break;
		
		case XML_NODE_COMMENT:
        s += '\n';
		s += std::string(tab, '\t');
		
        s += "<-- ";
        s += node->XMLNodeBase::getString( );
        s += " -->";
		
        appendString( s );
        break;
		default:
        break;
    }
	
    for ( i = 0; i < node->getChildCount( ); ++i )
    {
        writeNode((XMLNode*) node->getChildElement( i ), tab + 1 );
    }
	
    if ( ( node->getType( ) == XML_NODE_ELEMENT ) && ( node->getChildCount( ) > 0 ) )
    {
        s = "";
        if ( ( node->getChildElement( ) ) && ( node->getChildElement( )->getType( ) != XML_NODE_TEXT ) )
        {
            s += '\n';
			s += std::string(tab, '\t');
        }
		
        s += "</";
        s += node->XMLNodeBase::getName( );
        s += ">";
        appendString( s );
    }
}

bool XMLWriterFile::WriteFile(const std::string& name,XML& xml)
{
    // get file extension to choose XML or BXML ?
    std::string	fileext;
    size_t extpos=name.rfind(".")+1;
    fileext=name.substr(extpos,name.length()-extpos);
	
    if(fileext=="xml" || fileext == "kxml" || fileext == "vcxproj")
    {
        XMLWriterFile  writer;
		
        XMLNode *node = (XMLNode*)xml.getRoot( );
		
        writer.writeHeader( xml );
        writer.writeNode( node, 0 );
		
		SmartPointer<FileHandle> file = Platform_fopen(name.c_str(), "wb");
		
		if (file->mFile)
        {
			if (fileext == "kxml")
			{
				auto kxml_manager = KigsCore::GetSingleton("KXMLManager");
				if (!kxml_manager)
				{
					KIGS_ERROR("Need to include KXMLManager in the project", 3);
				}
				std::vector<u8> result;
				kxml_manager->SimpleCall("CompressKXML", writer.mData, result);
				Platform_fwrite(result.data(), 1, (long)result.size(), file.get());
			}
			else
			{
				for (auto& str : writer.mData)
					Platform_fwrite(str.data(), 1, (long)str.size(), file.get());
			}
            Platform_fflush(file.get());
            Platform_fclose(file.get());
            return true;
        }
    }
   
    return false;
}

void XMLWriterFile::WriteString(XML& xml, std::string &result, bool header, bool compress)
{
	XMLWriterFile  writer;
	
    XMLNode *node =(XMLNode*)xml.getRoot( );
	
	if(header) writer.writeHeader( xml );
    writer.writeNode( node, 0 );
	
	if (compress)
	{
		auto kxml_manager = KigsCore::GetSingleton("KXMLManager");
		if (!kxml_manager)
		{
			KIGS_ERROR("Need to include KXMLManager in the project", 3);
		}
		std::vector<u8> result_compressed;
		kxml_manager->SimpleCall("CompressKXML", writer.mData, result_compressed);
		result.assign((char*)result_compressed.data(), result_compressed.size());
		return;
	}
	
	u64 total_size = 0;
	for (auto& str : writer.mData)
		total_size += str.size();

	result.resize(total_size);
	
	auto write_ptr = result.data();
	for (auto& str : writer.mData)
	{
		memcpy((void*)write_ptr, str.data(), str.size());
		write_ptr += str.size();
	}
}

SP<CoreRawBuffer> XMLWriterFile::WriteBuffer(XML& xml, bool header, bool compress)
{
	XMLWriterFile  writer;

	XMLNode* node = (XMLNode*)xml.getRoot();
	auto result = MakeRefCounted<CoreRawBuffer>();

	if (header) writer.writeHeader(xml);
	writer.writeNode(node, 0);

	if (compress)
	{
		auto kxml_manager = KigsCore::GetSingleton("KXMLManager");
		if (!kxml_manager)
		{
			KIGS_ERROR("Need to include KXMLManager in the project", 3);
		}
		std::vector<u8> result_compressed;
		kxml_manager->SimpleCall("CompressKXML", writer.mData, result_compressed);
		result->resize(result_compressed.size());
		memcpy(result->data(), result_compressed.data(), result_compressed.size());
		return result;
	}

	u64 total_size = 0;
	for (auto& str : writer.mData)
		total_size += str.size();

	result->resize(total_size);

	auto write_ptr = result->data();
	for (auto& str : writer.mData)
	{
		memcpy((void*)write_ptr, str.data(), str.size());
		write_ptr += str.size();
	}

	return result;
}