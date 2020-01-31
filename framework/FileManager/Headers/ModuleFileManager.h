#ifndef _MODULEFILEMANAGER_H_
#define _MODULEFILEMANAGER_H_

#include "ModuleBase.h"
#include "FilePathManager.h"
#include "CoreRawBuffer.h"

/*! \defgroup FileManager FileManager module
 *  manage pathes for resource files
*/

// ****************************************
// * ModuleFileManager class
// * --------------------------------------
/*!  \class ModuleFileManager
	 this class is the module manager class. It registers the FilePathManager class
	 \ingroup FileManager
*/
// ****************************************

class ModuleFileManager : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleFileManager,ModuleBase,FileManager)
	DECLARE_INLINE_CONSTRUCTOR(ModuleFileManager){}

	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

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
	static	CoreRawBuffer*	LoadFile(const char *pFilename, u64& filelength, u64 startOffset = 0, bool AsCharString = false);
	static	CoreRawBuffer*	LoadFile(FileHandle *pFilename,u64& filelength, u64 startOffset=0,bool AsCharString=false);
	static	SmartPointer<CoreRawBuffer> LoadFileToBuffer(const std::string& pFilename);

	/**
	* load a text file in a string, add a 0 character at the end of the char buffer 
	* \param pFilename name of the file
	* \param filelength length of the file
	* \param startOffset offset from the start of the file 
	* \return a pointer on the loaded file in memory
	* \note returned pointer must be deleted with delete[] after use
	*/
	static  CoreRawBuffer*	LoadFileAsCharString(const char *pFilename, u64& filelength, u64 startOffset = 0);
	static  CoreRawBuffer*	LoadFileAsCharString(FileHandle *pFilename,u64& filelength,u64 startOffset=0);

	/**
	* save a binary file 
	* \param filename name of the file to create (or overwrite)
	* \param data binary data to save
	* \param length length of the data
	* \return true if data was written into specified file, false otherwise
	*/
	static	bool	SaveFile(const char *filename, u8* data, u64 length);
	
	/**
	* copy a binary file
	* \param sourceFilename name of the file to copy 
	* \param destFileName name of the file to create or overwrite 
	* \return true if file was copied
	*/
	static	bool	CoreCopyFile(const char *sourceFilename, const char *destFileName, int buffLen=2048);


	// utility method, retreive a short from a const char* stream (pointer is not increased) 
	inline static void GetShort(const char* read, s16& result)
	{
		char*	write=(char*)&result;
		write[0]=read[0];
		write[1]=read[1];
	}
	
	inline static void GetUShort(const char* read, u16& result)
	{
		char* write=(char*)&result;
		write[0]=read[0];
		write[1]=read[1];
	}
	
	// utility method, retreive a int from a const char* stream (pointer is not increased)    
	inline static void GetInt(const char* read, s32& result)
	{
		char*	write=(char*)&result;
		write[0]=read[0];
		write[1]=read[1];
		write[2]=read[2];
		write[3]=read[3];
	}
	
	inline static void GetUInt(const char* read, u32& result)
	{
		char*	write=(char*)&result;
		write[0]=read[0];
		write[1]=read[1];
		write[2]=read[2];
		write[3]=read[3];
	}
	
	// utility method, retreive a float from a const char* stream (pointer is not increased)    
	inline static void GetFloat(const char* read, float& result)
	{
		char*	write=(char*)&result;
		write[0]=read[0];
		write[1]=read[1];
		write[2]=read[2];
		write[3]=read[3];
	}
		   
protected:
	
}; 

#endif //_MODULEFILEMANAGER_H_
