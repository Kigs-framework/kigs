#ifndef _ANDROID_PLATFORMBASEAPPLICATION_H_
#define _ANDROID_PLATFORMBASEAPPLICATION_H_

//! This class must not be virtual because there's a double inheritance
class PlatformBaseApplication
{
public:
	//! constructor
	PlatformBaseApplication(){;}

	//! destructor
	//! no virtual here
	~PlatformBaseApplication(){;}
	
	bool	IsHolographic() {return false;} // no holographic mode
	
	void	Init(){;}
	void	Update(){;}
	void	Close(){;}
	void	Sleep();
	void	Resume();
	void	Message(int /* mtype */,int /* Params */);
	void	OpenLink(const char* a_link);
	void	OpenLink(const unsigned short* a_link, const unsigned int a_length);
	
	bool	CheckConnexion();
	bool	CheckBackKeyPressed();
		
	// get number of core / processor
	static unsigned int getProcessorCount();

	static int		getCpuId();

	static void		setCurrentThreadAffinityMask(int mask);
	static void		setThreadAffinityMask(void*,int mask);
	
	static const char* getPlatformName();
};

#endif //_ANDROID_PLATFORMBASEAPPLICATION_H_