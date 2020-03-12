
#include "PlatformBaseApplication.h"
#include "CoreIncludes.h"
/*#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <arpa/inet.h>
#import <ifaddrs.h>
#import <netdb.h>
#import <sys/socket.h>*/
#include <unistd.h>


/******* messages *******/
 struct structMessage 
 {
	 int types;
	 int param;
 };
structMessage mesMessage;
std::list<structMessage> listMessage;
 /************************/


void PlatformBaseApplication::Message(int mtype, int params) 
{
	//rempli list
	mesMessage.types = mtype;
	mesMessage.param = params;
	listMessage.push_back(mesMessage);
}

// get number of core / processor
unsigned int PlatformBaseApplication::getProcessorCount()
{
	return sysconf( _SC_NPROCESSORS_ONLN );
}


void	PlatformBaseApplication::OpenLink(const char* a_link)
{
/*	NSString* L_Url = [[NSString alloc] initWithUTF8String:a_link];
	L_Url = [L_Url stringByAddingPercentEscapesUsingEncoding : NSUTF8StringEncoding];
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString : L_Url]];*/
    
}

void	PlatformBaseApplication::OpenLink(const unsigned short* a_link, unsigned int a_length)
{
/*	NSString* L_Url = [[NSString alloc] initWithBytes:a_link length : a_length*sizeof(unsigned short) encoding : NSUTF16LittleEndianStringEncoding];
	L_Url = [[NSString alloc] initWithUTF8String:[L_Url UTF8String]];
	L_Url = [L_Url stringByAddingPercentEscapesUsingEncoding : NSUTF8StringEncoding];
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString : L_Url]];*/
}

bool    PlatformBaseApplication::CheckConnexion()
{
/*	struct sockaddr_in zeroAddress;
	bzero(&zeroAddress, sizeof(zeroAddress));
	zeroAddress.sin_len = sizeof(zeroAddress);
	zeroAddress.sin_family = AF_INET;

	SCNetworkReachabilityRef L_reachability = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (const struct sockaddr *)&zeroAddress);
	bool L_alwaysReturnLocalWiFiStatus = false;

	if (L_reachability != NULL)
	{
		NetworkStatus L_NetworkStatus = NotReachable;
		SCNetworkReachabilityFlags flags;

		if (SCNetworkReachabilityGetFlags(L_reachability, &flags))
		{
			L_NetworkStatus = (L_alwaysReturnLocalWiFiStatus) ? localWiFiStatusForFlags(flags) : networkStatusForFlags(flags);
			if (L_NetworkStatus != NotReachable)
				return true;
		}
	}

    */
    return false;
}
/*
PlatformBaseApplication::NetworkStatus PlatformBaseApplication::localWiFiStatusForFlags(SCNetworkReachabilityFlags flags)
{
    PlatformBaseApplication::NetworkStatus returnValue = NotReachable;

	if ((flags & kSCNetworkReachabilityFlagsReachable) && (flags & kSCNetworkReachabilityFlagsIsDirect))
	{
		returnValue = ReachableViaWiFi;
	}

	return returnValue;
}

PlatformBaseApplication::NetworkStatus PlatformBaseApplication::networkStatusForFlags(SCNetworkReachabilityFlags flags)
{
	if ((flags & kSCNetworkReachabilityFlagsReachable) == 0)
	{
		// The target host is not reachable.
		return NotReachable;
	}

    PlatformBaseApplication::NetworkStatus returnValue = NotReachable;

	if ((flags & kSCNetworkReachabilityFlagsConnectionRequired) == 0)
	{
		
		returnValue = ReachableViaWiFi;
	}

	if ((((flags & kSCNetworkReachabilityFlagsConnectionOnDemand) != 0) ||
		(flags & kSCNetworkReachabilityFlagsConnectionOnTraffic) != 0))
	{
		

		if ((flags & kSCNetworkReachabilityFlagsInterventionRequired) == 0)
		{
			
			returnValue = ReachableViaWiFi;
		}
	}

	if ((flags & kSCNetworkReachabilityFlagsIsWWAN) == kSCNetworkReachabilityFlagsIsWWAN)
	{
		
		returnValue = ReachableViaWWAN;
	}

	return returnValue;
}*/