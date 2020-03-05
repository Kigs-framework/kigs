#include "Platform/Core/Core.h"
#include <stdio.h>
//#include "FilePathManager.h"
#include "../../../../Core/Headers/Core.h"
#include "../../../../FileManager/Headers/FilePathManager.h"
#include "Platform/Core/Core.h"
#include "Platform/Core/PlatformBaseApplication.h"
//#include <shlobj.h>

#include "../../../../Core/Headers/ModuleBase.h"
#include "../../../../Core/Headers/CoreBaseApplication.h"

#include "../../../../HTTPRequest/Headers/HTTPConnect.h"

#import <Foundation/Foundation.h>

extern const char* gRessourcePath;

SmartPointer<FileHandle> IPhoneFindFullName(const kstl::string&	filename)
{
	SmartPointer<FileHandle> result;
	const char* filename_cstr = filename.c_str();

	kstl::string fullFileName = "";

	if (filename_cstr[0] != '#')
	{
		NSString* mallocFilename= [[NSString alloc] initWithUTF8String:filename_cstr];
		NSString* fileext=0;
		NSString* fileshortname=0;
		NSString* convertFilename = mallocFilename;
		
		NSRange range;
		range=[convertFilename rangeOfString:@"." options:NSBackwardsSearch];
		
		if(range.length)
		{
			fileext= [convertFilename substringFromIndex:range.location+1];
			// cut extension
			convertFilename=[convertFilename substringToIndex:range.location];
		}
		
		range=[convertFilename rangeOfString:@"/" options:NSBackwardsSearch];
		
		if(range.length)
		{
			fileshortname= [convertFilename substringFromIndex:range.location+1];	
			// keep only path
			convertFilename=[convertFilename substringToIndex:range.location];
		}
		else 
		{
			fileshortname=convertFilename;	
			// nothing else
			convertFilename=0;
		}
		
		NSString* path=[[NSBundle mainBundle] pathForResource:fileshortname ofType:fileext inDirectory:convertFilename];
		
        if(path)
		{
            fullFileName = [path cStringUsingEncoding:NSASCIIStringEncoding];
			FileHandle* newhandle = new FileHandle();
			newhandle->myFileName = filename;
			newhandle->myFullFileName = fullFileName;
			result = newhandle;
        }
		[mallocFilename release];

        return result;
	}
	else
	{
		fullFileName = "";

		kstl::string nameWithDevicePath="";
		
		FilePathManager::DeviceID	id=(FilePathManager::DeviceID)(filename[1]);
		
		switch(id)
		{
			case FilePathManager::CARD_STORAGE:
				// No CARD on ios
				return result;
				break;
			case FilePathManager::DEVICE_STORAGE:
				// No DEVICE on ios
				return result;
				break;
			case FilePathManager::APPLICATION_STORAGE:
			{
				nameWithDevicePath+=(const char*)(&filename[3]);
				NSString* mallocFilename= [[NSString alloc] initWithUTF8String:nameWithDevicePath.c_str()];
				NSString* convertFilename = mallocFilename;
				NSString* fileshortname=0;
				
				NSRange range = [convertFilename rangeOfString:@"/" options:NSBackwardsSearch];
				
				if(range.length)
					fileshortname= [convertFilename substringFromIndex:range.location+1];
				else 
					fileshortname=convertFilename;	
				
				fileshortname =	[NSString stringWithFormat:@"Documents/%@", fileshortname];

				NSString* fullpath = [NSHomeDirectory() stringByAppendingPathComponent:fileshortname];
				
                [mallocFilename release];
                
                if(fullpath)
                {
                    fullFileName = [fullpath cStringUsingEncoding:NSASCIIStringEncoding];
					FileHandle* newhandle = new FileHandle();
					newhandle->myFileName = filename;
					newhandle->myFullFileName = fullFileName;
					result = newhandle;
                    return result;
                }
                else
                    return result;
				
				break;
			}
            default:
                break;
		}
	}
	return result;
}

bool IPhonefopen (FileHandle *handle, const char* mode)
{
	handle->myFile->myFile = fopen(handle->myFullFileName.c_str(), mode);

	if (handle->myFile)
	{
		handle->myStatus |= FileHandle::Open;
		return true;
	}
	return false;
}

SmartPointer<FileHandle> IPhonefopen(const char * filename, const char * mode)
{
	SmartPointer<FileHandle> fullfilenamehandle;
	
	// check for specific device
	if (filename[0] == '#')
	{
		kstl::string nameWithDevicePath = "";
		
		fullfilenamehandle = IPhoneFindFullName(filename);
		if (fullfilenamehandle != 0)
		{
			nameWithDevicePath = fullfilenamehandle->myFullFileName;
            FILE* tmpfile = fopen(nameWithDevicePath.c_str(), mode);
            if(tmpfile)
            {
                MAC_FILE* L_NewFile = new MAC_FILE();
                L_NewFile->myFile = tmpfile;
                L_NewFile->mySize = 0;
                
                fullfilenamehandle->myFile = L_NewFile;
            }
		}
	}
	else
	{
        MAC_FILE* newFile = NULL;
        
        NSString* mallocFileName = [[NSString alloc] initWithUTF8String:filename];
        NSString* fileext = 0;
        NSString* fileshortname = 0;
        NSString* convertFilename = mallocFileName;
        
        NSRange range;
        range = [convertFilename rangeOfString:@"." options:NSBackwardsSearch];
        
        if(range.length)
        {
            fileext = [convertFilename substringFromIndex:range.location+1];
            //cut extension
            convertFilename = [convertFilename substringToIndex:range.location];
        }
        range = [convertFilename rangeOfString:@"/" options:NSBackwardsSearch];
        
        if(range.length)
        {
            fileshortname = [convertFilename substringFromIndex:range.location+1];
            //Keep only path
            convertFilename = [convertFilename substringToIndex:range.location];
        }
        else
        {
            fileshortname = convertFilename;
            //nothing else
            convertFilename = 0;
        }
        
        NSString* path = [[NSBundle mainBundle] pathForResource:fileshortname ofType: fileext inDirectory:convertFilename];
        
        
        FileHandle* newhandle = new FileHandle();
        newhandle->myFileName = filename;
        
        if(path)
        {
            newFile = new MAC_FILE();
            NSData* theData = [NSData dataWithContentsOfFile:path];
            [theData retain];
            newFile->myData = theData;
            newFile->myCurrentPos = 0;
            newFile->myFile = 0;
            NSFileManager* fm = [[NSFileManager alloc] init];
            
            NSNumber* filesize = [[fm attributesOfItemAtPath:path error:nil] objectForKey:NSFileSize];
            newFile->mySize = [filesize unsignedIntValue];
            [fm release];
            
            kstl::string fullFileName = [path cStringUsingEncoding:NSASCIIStringEncoding];
            
            newhandle->myFullFileName = fullFileName;
            newhandle->myFile = newFile;
            
            /*if (fullfilenamehandle->myFile)
                fullfilenamehandle->myFile->myFile = fopen(filename, mode);*/

        }
        fullfilenamehandle = newhandle;
        [mallocFileName release];
        
		
	}

	if (fullfilenamehandle && fullfilenamehandle->myFile && fullfilenamehandle->myFile->myFile)
	{
		fullfilenamehandle->myStatus |= FileHandle::Open;
	}

	return fullfilenamehandle;
}


long int IPhonefread(void * ptr, long size, long count, FileHandle* handle)
{
    if(handle && handle->myFile)
    {
        if(handle->myFile->myFile)
            return fread(ptr, size, count, handle->myFile->myFile);
        NSData* currentdata = (__bridge NSData*) handle->myFile->myData;
        
        int range = count*size;
        
        if( (range + handle->myFile->myCurrentPos) > handle->myFile->mySize)
            range = handle->myFile->mySize - handle->myFile->myCurrentPos;
        
        NSRange theRange = NSMakeRange(handle->myFile->myCurrentPos, range);
        [currentdata getBytes:ptr range:theRange];
        
        handle->myFile->myCurrentPos += range;
        return range;
    }
    return 0;
}

long int IPhonefwrite(const void * ptr, long size, long count, FileHandle* handle)
{
    if(handle && handle->myFile && handle->myFile->myFile)
        return fwrite(ptr, size, count, handle->myFile->myFile);
    return 0;
}

long int IPhoneftell(FileHandle* handle)
{
    long int L_retValue = 0;
    if(handle && handle->myFile)
    {
        if(handle->myFile->myFile)
            return ftell(handle->myFile->myFile);
        L_retValue = handle->myFile->myCurrentPos;
    }
    return L_retValue;
}

int IPhonefseek(FileHandle* handle, long int offset, int origin)
{
        if(!handle ||!handle->myFile)
            return 1;
        if(handle->myFile->myFile)
            return fseek(handle->myFile->myFile,offset,origin);
        switch(origin)
        {
            case SEEK_SET :
                if(offset<handle->myFile->mySize)
                {
                    handle->myFile->myCurrentPos=offset;
                    return 0;
                }
                break;
            case SEEK_CUR :
                if((offset+handle->myFile->myCurrentPos)<handle->myFile->mySize)
                {
                    handle->myFile->myCurrentPos+=offset;
                    return 0;
                }
                break;
            case SEEK_END :
                if((handle->myFile->mySize-offset)>=0)
                {
                    handle->myFile->myCurrentPos=(handle->myFile->mySize-offset);
                    return 0;
                }
                break;
            default : return 1;
        }
        
        return 3;
}

int IPhonefflush(FileHandle* handle)
{
    if(handle && handle->myFile && handle->myFile->myFile)
        return fflush(handle->myFile->myFile);
    return 0;
}

int IPhonefclose(FileHandle* handle)
{
    int result=0;
    if(handle && handle->myFile)
    {
        if(handle->myFile->myFile)
        {
            result = fclose(handle->myFile->myFile);
        }
        NSData* currentdata = (__bridge NSData*)handle->myFile->myData;
        if(currentdata)
           [currentdata release];
        
        handle->myFile->myData = 0;
        delete handle->myFile;
        handle->myStatus=0; // reset
    }
    return result;
}

/*

int		IPhonefseek(MAC_FILE * file, long int offset, int origin)
{
	
	MAC_FILE* currentFile=(MAC_FILE*)file;
	if(!currentFile)
	{
		return 1;
	}
	if (currentFile->myFile != 0) {
		return fseek(currentFile->myFile, offset, origin);
	}
	switch(origin)
	{
		case SEEK_SET : 
			if(offset<currentFile->mySize)
			{
				currentFile->myCurrentPos=offset;
				return 0;
			}
			break;
		case SEEK_CUR : 
			if((offset+currentFile->myCurrentPos)<currentFile->mySize)
			{
				currentFile->myCurrentPos+=offset;
				return 0;
			}
			break;
		case SEEK_END :
			if((currentFile->mySize-offset)>=0)
			{
				currentFile->myCurrentPos=(currentFile->mySize-offset);
				return 0;
			}
			break;
		default : return 1;
	}
	
	return 3;
}

long int		IPhoneftell(MAC_FILE * file)
{
	
	MAC_FILE* currentFile=(MAC_FILE*)file;
	if(currentFile)
	{
		if (currentFile->myFile != 0) {
			return ftell(currentFile->myFile);
		}
		return currentFile->myCurrentPos;
	}
	return 0;
}

size_t		IPhonefread(void * dst, size_t elemsize, size_t count, MAC_FILE * file)
{
	MAC_FILE* currentFile=(MAC_FILE*)file;
	if(currentFile)
	{
		if (currentFile->myFile != 0) 
			return fread(dst, elemsize, count, currentFile->myFile);
		
		NSData* currentdata=(__bridge NSData*)currentFile->myData;
		
		int rangeL=count*elemsize;
		
		if((rangeL+currentFile->myCurrentPos) > currentFile->mySize )
		{
			rangeL=currentFile->mySize-currentFile->myCurrentPos;
		}
		
		
		NSRange range=NSMakeRange(currentFile->myCurrentPos,rangeL);
		[ currentdata getBytes:dst range:range];
		
		currentFile->myCurrentPos+=rangeL;
		
		return rangeL;
	}

	return 0;
}

int		IPhoneferror(MAC_FILE * file)
{
	
	MAC_FILE* currentFile=(MAC_FILE*)file;
	if(currentFile)
	{
		if (currentFile->myFile != 0) 
			return ferror(currentFile->myFile);
	}
	return 0;
}

int		IPhonefeof(MAC_FILE * file)
{
	MAC_FILE* currentFile=(MAC_FILE*)file;
	if(currentFile)
	{
		if (currentFile->myFile != 0) 
			return feof(currentFile->myFile);
		
		return (currentFile->myCurrentPos>=currentFile->mySize)?1:0;
	}
	return 0;
}

int		IPhonefclose(MAC_FILE * file)
{
	MAC_FILE* currentFile=(MAC_FILE*)file;
	int result = 0;
	if(currentFile)
	{
		if (currentFile->myFile != 0) 
			result = fclose(currentFile->myFile);
		
		NSData* currentdata=(__bridge NSData*)currentFile->myData;
		if (currentdata) {
			[currentdata release];
		}
		
		currentFile->myData=0;
		
		delete currentFile;
	}
	return result;
}

size_t IPhonefwrite ( const void * ptr, size_t size, size_t count, MAC_FILE * stream ) {
	MAC_FILE* currentFile=(MAC_FILE*)stream;
	if(currentFile) {
		if (currentFile->myFile != 0) 
			return fwrite(ptr, size, count, currentFile->myFile);
	}
	return 0;
	
}

int IPhonefflush ( MAC_FILE * stream ) {
	MAC_FILE* currentFile=(MAC_FILE*)stream;
	if(currentFile) {
		if (currentFile->myFile != 0) 
			return fflush(currentFile->myFile);
	}
	return 0;
}




*/