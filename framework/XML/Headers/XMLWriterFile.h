#pragma once

#include "XML.h"
#include "kstlstring.h"

namespace Kigs
{

	namespace Xml
	{
		// ****************************************
		// * XMLWriterFile class
		// * --------------------------------------
		/**
		 * \file	XMLWriterFile.h
		 * \class	XMLWriterFile
		 * \ingroup ModuleXML
		 * \brief	Write an XML file from an XML structure.
		 */
		 // ****************************************
		class XMLWriterFile
		{
		public:
			//! write the given xml hierarchy in a file
			static bool WriteFile(const std::string&, XML& xml);
			//! write the given xml hierarchy in a memory file (string)
			static void WriteString(XML& xml, std::string& Result, bool header = true, bool compress = false);
			static SP<CoreRawBuffer> WriteBuffer(XML& xml, bool header = true, bool compress = false);

		protected:
			//! constructor
			XMLWriterFile() = default;

			//! utility method
			void appendString(std::string& sAppend, bool isElementText = false);

			//! utility method to write the xml file header
			void writeHeader(XML& xml);

			//! utility to write the given node (recursive)
			void writeNode(XMLNode* node, int tab);


			std::vector<std::string> mData;
		};
	}
}
