#ifndef _MAC_CORE_H_
#define _MAC_CORE_H_

// MAC_FILE struct to "emulate" fopen fread ... on mainbundle ressources
#include <stdio.h>

class FileHandle;

template<typename smartPointOn>
class SmartPointer;

struct  MAC_FILE {
	void*			myData; // store NSString path
	unsigned int	myCurrentPos;
	unsigned int	mySize;
	FILE* myFile;
};

struct UITouch;
/****** Touch ******/
struct structCoord
{
	float mouseCoordinate[2];
	unsigned int phase;
	int tapCount;
	bool active;
	UITouch* Touch;
};

extern structCoord listCoordinate[5];
/*******************/


extern bool IPhonefopen(FileHandle* handle, const char* mode);
extern SmartPointer<FileHandle> IPhonefopen(const char* name, const char * mode);

extern long int IPhonefread(void * ptr, long size, long count, FileHandle* handle);
extern long int IPhonefwrite(const void * ptr, long size, long count, FileHandle* handle);
extern long int IPhoneftell(FileHandle* handle);
extern int IPhonefseek(FileHandle* handle, long int offset, int origin);
extern int IPhonefflush(FileHandle* handle);
extern int IPhonefclose(FileHandle* handle);

extern SmartPointer<FileHandle> IPhoneFindFullName(const kstl::string&	filename);
extern void IPhoneCheckState(FileHandle* handle);
extern void IPhoneCreateFolderTree(FileHandle* hndl);

#endif //_MAC_CORE_H_