#ifndef _PUREVIRTUALFILEACCESSDELEGATE_H_
#define _PUREVIRTUALFILEACCESSDELEGATE_H_

class  FileHandle;

class PureVirtualFileAccessDelegate
{
public:
	virtual bool		Platform_fopen(FileHandle* handle, const char * mode) = 0;
	
	virtual long int	Platform_fread(void * ptr, long size, long count, FileHandle* handle) = 0;
	
	virtual long int	Platform_fwrite(const void * ptr, long size, long count, FileHandle* handle) = 0;
	
	virtual long int	Platform_ftell(FileHandle* handle) = 0;
	
	virtual int			Platform_fseek(FileHandle* handle, long int offset, int origin) = 0;
	
	virtual int			Platform_fflush(FileHandle* handle) = 0;
	
	virtual int			Platform_fclose(FileHandle* handle) = 0;

	virtual PureVirtualFileAccessDelegate* MakeCopy() = 0;

	virtual ~PureVirtualFileAccessDelegate() {};
	
protected:
};

#endif //_PUREVIRTUALFILEACCESSDELEGATE_H_