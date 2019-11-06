#ifndef _DISPLAYDEVICECAPS_H_
#define _DISPLAYDEVICECAPS_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"


// ****************************************
// * DisplayDeviceCaps class
// * --------------------------------------
/*!  \class DisplayDeviceCaps
     base class for Display Device Caps enumerator

     \ingroup GUIModule
*/
// ****************************************

class DisplayDeviceCaps : public CoreModifiable
{
public:
    
    DECLARE_ABSTRACT_CLASS_INFO(DisplayDeviceCaps,CoreModifiable,GUI)
    
	//! constructor
    DisplayDeviceCaps(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	//! get the display device count 
	unsigned int	GetDisplayDeviceCount()
	{
		return (unsigned int)myDisplayDeviceList.size();
	}

	//! structure storing one available display device capacity 
	class	DisplayDeviceCapacity
	{
	public:
		int		myWidth;
		int		myHeight;
		int		myBitPerPixel;
		bool	myIsCurrent;
	};

	//! structure storing one display device information
	class	DisplayDevice
	{
	public:
		kstl::string						myName;
		kstl::vector<DisplayDeviceCapacity>	myCapacityList;		
		bool								myMain;
	};

	DisplayDeviceCapacity* Get_CurrentDisplay(const kstl::string& a_deviceName)
	{
		if(myDisplayDeviceList.find(a_deviceName)!=myDisplayDeviceList.end())
		{
			kstl::vector<DisplayDeviceCapacity>::iterator L_It = myDisplayDeviceList[a_deviceName].myCapacityList.begin();
			kstl::vector<DisplayDeviceCapacity>::iterator L_ItEnd =  myDisplayDeviceList[a_deviceName].myCapacityList.end();

			while(L_It != L_ItEnd)
			{
				if((*L_It).myIsCurrent)
					return &(*L_It);

				L_It++;
			}
		}

		return NULL;
	}

	DisplayDeviceCapacity* Get_CurrentDisplay(unsigned int a_index)
	{
		if(a_index < myDisplayDeviceList.size())
		{
			unsigned int i;
			kstl::map<kstl::string,DisplayDevice>::iterator	it=myDisplayDeviceList.begin();
			for(i=0;i<a_index;i++)
				++it;

			if(it!=myDisplayDeviceList.end())
			{
				kstl::vector<DisplayDeviceCapacity>::iterator L_It = (*it).second.myCapacityList.begin();
				kstl::vector<DisplayDeviceCapacity>::iterator L_ItEnd =  (*it).second.myCapacityList.end();

				while(L_It != L_ItEnd)
				{
					if((*L_It).myIsCurrent)
						return &(*L_It);

					L_It++;
				}
			}
		}

		return NULL;
	}

	DisplayDevice* Get_DisplayDevice(unsigned int a_index)
	{
		//if (a_index < myDisplayDeviceList.size())
		{
			unsigned int i;
			kstl::map<kstl::string, DisplayDevice>::iterator it = myDisplayDeviceList.begin();
			for (i = 0; i < a_index; i++)
			{
				//get main if -1
				if (a_index == 0xFFFFFFFF)
				{
					if(it->second.myMain)
						return  &(it->second);
				}
				++it;
			}

			return &(it->second);
		}

		return NULL;
	}
	
	//! retreive all available capacities for given display device
	const kstl::vector<DisplayDeviceCapacity>*	GetDisplayDeviceCapacityList(const kstl::string&	devicename)
	{
		if(myDisplayDeviceList.find(devicename)!=myDisplayDeviceList.end())
		{
			return &myDisplayDeviceList[devicename].myCapacityList;
		}
		return 0;
	}

	//! retreive all available capacities for given display device index
	const kstl::vector<DisplayDeviceCapacity>*	GetDisplayDeviceCapacityList(unsigned int index)
	{
		if(index<myDisplayDeviceList.size())
		{
			unsigned int i;
			kstl::map<kstl::string,DisplayDevice>::iterator	it=myDisplayDeviceList.begin();
			for(i=0;i<index;i++)
			{
				++it;
			}

			if(it!=myDisplayDeviceList.end())
			{
				return &((*it).second.myCapacityList);
			}
		}

		return 0;
	}

	//! retreive all available capacities for main display device
	const kstl::vector<DisplayDeviceCapacity>*	GetMainDisplayDeviceCapacityList()
	{
		if(myDisplayDeviceList.size())
		{
			
			kstl::map<kstl::string,DisplayDevice>::iterator	it=myDisplayDeviceList.begin();
			for(it=myDisplayDeviceList.begin();it!=myDisplayDeviceList.end();it++)
			{
				if((*it).second.myMain)
				{
					return &((*it).second.myCapacityList);
				}
			}
		}

		return 0;
	}

	virtual bool SupportWindowedMode()
	{
		return false; // false by default, will be true on Windows / MacOS / Javascript ?
	}
	
protected:
	//! destructor
    virtual ~DisplayDeviceCaps();

	kstl::map<kstl::string,DisplayDevice>		myDisplayDeviceList;

};    

#endif //_DISPLAYDEVICECAPS_H_
