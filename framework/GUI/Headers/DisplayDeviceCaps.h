#ifndef _DISPLAYDEVICECAPS_H_
#define _DISPLAYDEVICECAPS_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"

#include <map>


// ****************************************
// * DisplayDeviceCaps class
// * --------------------------------------
/**
* \file	DisplayDeviceCaps.h
* \class	DisplayDeviceCaps
* \ingroup GUIModule
* \brief	Base class to determine display device capacities ( resolution, screen count ...).
*/
// ****************************************
class DisplayDeviceCaps : public CoreModifiable
{
public:
    
    DECLARE_ABSTRACT_CLASS_INFO(DisplayDeviceCaps,CoreModifiable,GUI)
    
	//! constructor
    DisplayDeviceCaps(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	//! destructor
	virtual ~DisplayDeviceCaps();

	//! get the display device count 
	unsigned int	GetDisplayDeviceCount()
	{
		return (unsigned int)mDisplayDeviceList.size();
	}

	//! structure storing one available display device capacity 
	class	DisplayDeviceCapacity
	{
	public:
		int		mWidth;
		int		mHeight;
		int		mBitPerPixel;
		bool	mIsCurrent;
	};

	//! structure storing one display device information
	class	DisplayDevice
	{
	public:
		kstl::string						mName;
		kstl::vector<DisplayDeviceCapacity>	mCapacityList;		
		bool								mMain;
		float								mScaling = 1.0f;
	};

	DisplayDeviceCapacity* Get_CurrentDisplay(const kstl::string& a_deviceName)
	{
		if(mDisplayDeviceList.find(a_deviceName)!=mDisplayDeviceList.end())
		{
			kstl::vector<DisplayDeviceCapacity>::iterator L_It = mDisplayDeviceList[a_deviceName].mCapacityList.begin();
			kstl::vector<DisplayDeviceCapacity>::iterator L_ItEnd =  mDisplayDeviceList[a_deviceName].mCapacityList.end();

			while(L_It != L_ItEnd)
			{
				if((*L_It).mIsCurrent)
					return &(*L_It);

				L_It++;
			}
		}

		return NULL;
	}

	DisplayDeviceCapacity* Get_CurrentDisplay(unsigned int a_index)
	{
		if(a_index < mDisplayDeviceList.size())
		{
			unsigned int i;
			kstl::map<kstl::string,DisplayDevice>::iterator	it=mDisplayDeviceList.begin();
			for(i=0;i<a_index;i++)
				++it;

			if(it!=mDisplayDeviceList.end())
			{
				kstl::vector<DisplayDeviceCapacity>::iterator L_It = (*it).second.mCapacityList.begin();
				kstl::vector<DisplayDeviceCapacity>::iterator L_ItEnd =  (*it).second.mCapacityList.end();

				while(L_It != L_ItEnd)
				{
					if((*L_It).mIsCurrent)
						return &(*L_It);

					L_It++;
				}
			}
		}

		return NULL;
	}

	DisplayDevice* Get_DisplayDevice(unsigned int a_index)
	{
		{
			unsigned int i;
			kstl::map<kstl::string, DisplayDevice>::iterator it = mDisplayDeviceList.begin();
			for (i = 0; i < a_index; i++)
			{
				//get main if -1
				if (a_index == 0xFFFFFFFF)
				{
					if(it->second.mMain)
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
		if(mDisplayDeviceList.find(devicename)!=mDisplayDeviceList.end())
		{
			return &mDisplayDeviceList[devicename].mCapacityList;
		}
		return 0;
	}

	//! retreive all available capacities for given display device index
	const kstl::vector<DisplayDeviceCapacity>*	GetDisplayDeviceCapacityList(unsigned int index)
	{
		if(index<mDisplayDeviceList.size())
		{
			unsigned int i;
			kstl::map<kstl::string,DisplayDevice>::iterator	it=mDisplayDeviceList.begin();
			for(i=0;i<index;i++)
			{
				++it;
			}

			if(it!=mDisplayDeviceList.end())
			{
				return &((*it).second.mCapacityList);
			}
		}

		return 0;
	}

	//! retreive all available capacities for main display device
	const kstl::vector<DisplayDeviceCapacity>*	GetMainDisplayDeviceCapacityList()
	{
		if(mDisplayDeviceList.size())
		{
			
			kstl::map<kstl::string,DisplayDevice>::iterator	it=mDisplayDeviceList.begin();
			for(it=mDisplayDeviceList.begin();it!=mDisplayDeviceList.end();it++)
			{
				if((*it).second.mMain)
				{
					return &((*it).second.mCapacityList);
				}
			}
		}

		return 0;
	}

	float GetMainDisplayDeviceScaling() const
	{
		if (mDisplayDeviceList.size())
		{
			for (auto it = mDisplayDeviceList.cbegin(); it != mDisplayDeviceList.cend(); it++)
			{
				if ((*it).second.mMain)
				{
					return (*it).second.mScaling;
				}
			}
		}
		return 1.0f;
	}

	virtual bool SupportWindowedMode()
	{
		return false; // false by default, will be true on Windows / MacOS / Javascript ?
	}
	
protected:
	kstl::map<kstl::string,DisplayDevice>		mDisplayDeviceList;

};    

#endif //_DISPLAYDEVICECAPS_H_
