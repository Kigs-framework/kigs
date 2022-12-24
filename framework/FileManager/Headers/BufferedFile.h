#pragma once

//#include "CoreModifiable.h"
#include "Platform/Core/PlatformCore.h"
#include "FilePathManager.h"

namespace Kigs
{
	namespace File
	{
		// ****************************************
		// * BufferedFile class
		// * --------------------------------------
		/**
		 * \file	BufferedFile.h
		 * \class	BufferedFile
		 * \ingroup FileManager
		 * \brief	buffered file class
		 */
		 // ****************************************
		class BufferedFile
		{
		private:
			/**
			 * \brief	constructor
			 * \fn 		BufferedFile()
			 */
			BufferedFile() {}

			/**
			 * \brief	constructor - do nothing
			 * \fn 		BufferedFile(const BufferedFile&)
			 * \param	BufferedFile : not used
			 */
			BufferedFile(const BufferedFile&) {}

		public:
			/**
				* \brief	open a file and create a bufferedFile
				* \fn 		static BufferedFile *Open(const char *pFilehdl)
				* \param	pFilehdl : filehandle of the file
				*/
			static BufferedFile* Open(FileHandle* pFilehdl)
			{
				if (!pFilehdl->mFile)
					return NULL;

				Platform_fopen(pFilehdl, "rb");

				BufferedFile* pRes = new BufferedFile;
				Platform_fseek(pFilehdl, 0, SEEK_END);
				pRes->mFileSize = Platform_ftell(pFilehdl);
				Platform_fseek(pFilehdl, 0, SEEK_SET);

				pRes->mFileContent = new char[pRes->mFileSize];
				Platform_fread(pRes->mFileContent, 1, pRes->mFileSize, pFilehdl);
				pRes->mFileCurrentOffset = 0;
				Platform_fclose(pFilehdl);
				return pRes;
			}

			/**
			 * \brief	open a file and create a bufferedFile
			 * \fn 		static BufferedFile *Open(const char *pFilename)
			 * \param	pFilename : file name to open
			 */
			static BufferedFile* Open(const char* pFilename)
			{
				SmartPointer<FileHandle> pFile = Platform_fopen(pFilename, "rb");
				if (!pFile->mFile)
					return NULL;
				BufferedFile* pRes = new BufferedFile;
				Platform_fseek(pFile.get(), 0, SEEK_END);
				pRes->mFileSize = Platform_ftell(pFile.get());
				Platform_fseek(pFile.get(), 0, SEEK_SET);

				pRes->mFileContent = new char[pRes->mFileSize];
				Platform_fread(pRes->mFileContent, 1, pRes->mFileSize, pFile.get());
				pRes->mFileCurrentOffset = 0;
				Platform_fclose(pFile.get());
				return pRes;
			}

			/**
			 * \brief	destructor
			 * \fn 		~BufferedFile()
			 */
			~BufferedFile() { delete[] mFileContent; }

			/**
			 * \brief	read from the bufferedFile
			 * \fn 		int	MemRead(void *pDest, size_t ItemSize, size_t ItemCount)
			 * \param	pDest : item which receive the readed byte
			 * \param	ItemSize : size of the item
			 * \param	ItemCount : number of item to read
			 * \return	number of byte read
			 */
			int	MemRead(void* pDest, size_t ItemSize, size_t ItemCount)
			{
				char* pcDest = (char*)pDest;
				unsigned int ReadCount = 0;
				while (ReadCount < ItemCount)
				{
					if ((mFileCurrentOffset + ItemSize) > mFileSize)
						break;
					for (unsigned int i = 0; i < ItemSize; i++)
						*pcDest++ = mFileContent[mFileCurrentOffset++];
					ReadCount++;
				}
				return ReadCount;
			}

			bool	searchNextWord(const std::string& toSearch);
			bool	getNextFloat(float& flt);

		private:
			//! size of the file
			unsigned int mFileSize;
			//! data in the file
			char* mFileContent;
			//! read position
			unsigned int mFileCurrentOffset;
		};

		//! redefinition of fread
		inline int fread(void* pDest, size_t ItemSize, size_t ItemCount, BufferedFile* pFile)
		{
			return pFile->MemRead(pDest, ItemSize, ItemCount);
		}
	}
}