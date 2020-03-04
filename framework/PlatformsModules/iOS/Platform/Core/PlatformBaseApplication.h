#ifndef _IPHONE_PLATFORMBASEAPPLICATION_H_
#define _IPHONE_PLATFORMBASEAPPLICATION_H_

//! This class must not be virtual because there's a double inheritance
class PlatformBaseApplication
{
public:
	enum NetworkStatus
	{
		NotReachable = 0,
		ReachableViaWiFi,
		ReachableViaWWAN
	};

	//! constructor
	PlatformBaseApplication(){;}

	//! destructor
	//! no virtual here
	~PlatformBaseApplication(){;}
	
	
	void	Init(){;}
	void	Update(){;}
	void	Close(){;}
	void	Sleep(){;}
	void	Resume(){;}
	void	Message(int /* mtype */,int /* Params */);
	void	OpenLink(const char* a_link);
	void	OpenLink(const unsigned short* a_link, unsigned int a_length);
    bool    CheckConnexion();
    
    // ?
    bool	CheckBackKeyPressed()
    {
        return false;
    }
	
	// get number of core / processor
	unsigned int getProcessorCount();
    
protected:
/*    NetworkStatus localWiFiStatusForFlags(SCNetworkReachabilityFlags flags);
    NetworkStatus networkStatusForFlags(SCNetworkReachabilityFlags flags);
	*/
};

#endif //_IPHONE_PLATFORMBASEAPPLICATION_H_