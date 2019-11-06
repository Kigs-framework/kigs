#ifndef __BUFFEREDFILE_H__
#define __BUFFEREDFILE_H__

//#include "CoreModifiable.h"
#include "Platform/Core/PlatformCore.h"
#include "FilePathManager.h"

// ****************************************
// * BufferedFile class
// * --------------------------------------
/**
 * \file	BufferedFile.h
 * \class	BufferedFile
 * \ingroup Renderer
 * \brief	buffered file class
 * \author	ukn
 * \version ukn
 * \date	ukn
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
	static BufferedFile *Open(FileHandle * pFilehdl)
	{
		if (!pFilehdl->myFile)
			return NULL;
		
		Platform_fopen(pFilehdl, "rb");

		BufferedFile *pRes = new BufferedFile;
		Platform_fseek(pFilehdl, 0, SEEK_END);
		pRes->myFileSize = Platform_ftell(pFilehdl);
		Platform_fseek(pFilehdl, 0, SEEK_SET);

		pRes->myFileContent = new char[pRes->myFileSize];
		Platform_fread(pRes->myFileContent, 1, pRes->myFileSize, pFilehdl);
		pRes->myFileCurrentOffset = 0;
		Platform_fclose(pFilehdl);
		return pRes;
	}

	/**
	 * \brief	open a file and create a bufferedFile
	 * \fn 		static BufferedFile *Open(const char *pFilename)
	 * \param	pFilename : file name to open
	 */
	static BufferedFile *Open(const char *pFilename)
	{
		SmartPointer<FileHandle> pFile = Platform_fopen(pFilename, "rb");
		if (!pFile->myFile)
			return NULL;
		BufferedFile *pRes = new BufferedFile;
		Platform_fseek(pFile.get(), 0, SEEK_END);
		pRes->myFileSize = Platform_ftell(pFile.get());
		Platform_fseek(pFile.get(), 0, SEEK_SET);
		
		pRes->myFileContent = new char[pRes->myFileSize];
		Platform_fread(pRes->myFileContent, 1, pRes->myFileSize, pFile.get());
		pRes->myFileCurrentOffset = 0;
		Platform_fclose(pFile.get());
		return pRes;
	}
	
	/**
	 * \brief	destructor
	 * \fn 		~BufferedFile()
	 */
	~BufferedFile() {delete[] myFileContent;}

	/**
	 * \brief	read from the bufferedFile
	 * \fn 		int	MemRead(void *pDest, size_t ItemSize, size_t ItemCount)
	 * \param	pDest : item which receive the readed byte
	 * \param	ItemSize : size of the item
	 * \param	ItemCount : number of item to read
	 * \return	number of byte read
	 */
	int	MemRead(void *pDest, size_t ItemSize, size_t ItemCount)
	{
		char *pcDest = (char*)pDest;
		unsigned int ReadCount=0;
		while (ReadCount<ItemCount)
		{
			if ((myFileCurrentOffset+ItemSize)>myFileSize)
				break;
			for (unsigned int i=0; i<ItemSize; i++)
				*pcDest++ = myFileContent[myFileCurrentOffset++];
			ReadCount++;
		}
		return ReadCount;
	}

	bool	searchNextWord(const kstl::string& toSearch);
	bool	getNextFloat(kfloat& flt);

private:
	//! size of the file
	unsigned int myFileSize;
	//! data in the file
	char *myFileContent;
	//! read position
	unsigned int myFileCurrentOffset;
};

//! redefinition of fread
inline int fread(void *pDest, size_t ItemSize, size_t ItemCount, BufferedFile *pFile)
{
	return pFile->MemRead(pDest, ItemSize, ItemCount);
}
#endif
