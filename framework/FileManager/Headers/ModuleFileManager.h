#pragma once

#include "ModuleBase.h"
#include "FilePathManager.h"
#include "CoreRawBuffer.h"

namespace Kigs
{
	namespace File
	{
		/*! \defgroup FileManager FileManager module
		 *  manage pathes for resource files and file read / write ... methods
		*/


		// ****************************************
		// * ModuleFileManager class
		// * --------------------------------------
		/**
		 * \file	ModuleFileManager.h
		 * \class	ModuleFileManager
		 * \ingroup FileManager
		 * \ingroup Module
		 * \brief	Manage classes related to pathes and file reading / writing.
		 */
		 // ****************************************


		class ModuleFileManager : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(ModuleFileManager, ModuleBase, FileManager)
				DECLARE_INLINE_CONSTRUCTOR(ModuleFileManager) {}

			//! module init
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;

			//! module close
			void Close() override;

			//! module update
			void Update(const Timer& timer, void* addParam) override;

			/**
			* load a binary file
			* \param pFilename name of the file
			* \param filelength length of the file
			* \param startOffset offset from the start of the file
			* \return a pointer on the loaded file in memory
			* \note returned pointer must be deleted with delete[] after use
			*/
			static	SP<CoreRawBuffer>	LoadFile(const char* pFilename, u64& filelength, u64 startOffset = 0, unsigned int trailing_zero = 0);
			static	SP<CoreRawBuffer>	LoadFile(FileHandle* pFilename, u64& filelength, u64 startOffset = 0, unsigned int trailing_zero = 0);
			static	SmartPointer<CoreRawBuffer> LoadFileToBuffer(const std::string& pFilename);

			/**
			* load a text file in a string, add a 0 character at the end of the char buffer
			* \param pFilename name of the file
			* \param filelength length of the file
			* \param startOffset offset from the start of the file
			* \return a pointer on the loaded file in memory
			* \note returned pointer must be deleted with delete[] after use
			*/
			static  SP<CoreRawBuffer> LoadFileAsCharString(const char* pFilename, u64& filelength, u8 charSize, u64 startOffset = 0);
			static  SP<CoreRawBuffer> LoadFileAsCharString(FileHandle* pFilename, u64& filelength, u8 charSize, u64 startOffset = 0);

			/**
			* save a binary file
			* \param filename name of the file to create (or overwrite)
			* \param data binary data to save
			* \param length length of the data
			* \return true if data was written into specified file, false otherwise
			*/
			static	bool	SaveFile(const char* filename, u8* data, u64 length);

			/**
			* remove given file
			* \param filename name of the file to delete (remove)
			* \return true if the file was successfully removed
			*/
			static	bool	RemoveFile(const char* filename);


			/**
			* copy a binary file
			* \param sourceFilename name of the file to copy
			* \param destFileName name of the file to create or overwrite
			* \return true if file was copied
			*/
			static	bool	CoreCopyFile(const char* sourceFilename, const char* destFileName, int buffLen = 2048);

			/**
			* copy a binary file
			* \param source handle of source file
			* \param dest handle of dest file
			* \return true if file was copied
			*/
			static	bool	CoreCopyFile(SP<FileHandle> source, SP<FileHandle> dest, int buffLen = 2048);

			/**
			* copy a part of a file
			* \param source handle of source file
			* \param dest handle of dest file
			* \return true if file was copied
			*/
			static	bool	CoreCopyPartOfFile(SP<FileHandle> lsource, u64 lstart, u64 lsize, SP<FileHandle> ldest, int lbuffLen = 2048);

			// utility method, retreive a short from a const char* stream (pointer is not increased) 
			inline static void GetShort(const char* read, s16& result)
			{
				char* write = (char*)&result;
				write[0] = read[0];
				write[1] = read[1];
			}

			inline static void GetUShort(const char* read, u16& result)
			{
				char* write = (char*)&result;
				write[0] = read[0];
				write[1] = read[1];
			}

			// utility method, retreive a int from a const char* stream (pointer is not increased)    
			inline static void GetInt(const char* read, s32& result)
			{
				char* write = (char*)&result;
				write[0] = read[0];
				write[1] = read[1];
				write[2] = read[2];
				write[3] = read[3];
			}

			inline static void GetUInt(const char* read, u32& result)
			{
				char* write = (char*)&result;
				write[0] = read[0];
				write[1] = read[1];
				write[2] = read[2];
				write[3] = read[3];
			}

			// utility method, retreive a float from a const char* stream (pointer is not increased)    
			inline static void GetFloat(const char* read, float& result)
			{
				char* write = (char*)&result;
				write[0] = read[0];
				write[1] = read[1];
				write[2] = read[2];
				write[3] = read[3];
			}

		protected:

		};

	}
}
