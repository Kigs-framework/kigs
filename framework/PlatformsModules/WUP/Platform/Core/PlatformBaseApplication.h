#ifndef _WUP_PLATFORMBASEAPPLICATION_H_
#define _WUP_PLATFORMBASEAPPLICATION_H_

class KeyboardDevice;

//! This class must not be virtual because there's a double inheritance

class PlatformBaseApplication
{
public:
	//! constructor
	PlatformBaseApplication(){ myKeyboard=0; }

	//! destructor
	//! no virtual here
	~PlatformBaseApplication(){ ; }


	void	Init(){ ; }
	void	Update(){ ; }
	void	Close(){ ; }
	void	Sleep(){ ; }
	void	Resume(){ ; }
	void	Message(int /* mtype */, int /* Params */){ ; }
	void	OpenLink(const char* a_link);
	void	OpenLink(const unsigned short* a_link, unsigned int a_length);
	bool	CheckConnexion();

	// check esc key state
	bool	CheckBackKeyPressed();

	// check holographic mode
	bool	IsHolographic();
	
	// get number of core / processor
	static unsigned int getProcessorCount();

	static int		getCpuId();

	static void		setCurrentThreadAffinityMask(int mask);
	static void		setThreadAffinityMask(void*,int mask);

	// for back key check
	KeyboardDevice* myKeyboard;
	
	static const char* getPlatformName();

protected:
	bool mIsHolographic;
};

#endif //_WINDOWS_PLATFORMBASEAPPLICATION_H_