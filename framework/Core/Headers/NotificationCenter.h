#ifndef _NOTIFICATIONCENTER_H_
#define _NOTIFICATIONCENTER_H_

#include "CoreModifiable.h"
#include "CoreItemOperator.h"

#include <mutex>

// ****************************************
// * NotificationCenter class should be used as a singleton
// * --------------------------------------
/*!  \class NotificationCenter
\ingroup NotificationCenter
*/
// ****************************************


class NotificationCenter
{
public:
	NotificationCenter();
	~NotificationCenter();

	void addObserver(CoreModifiable* observer,const kstl::string& selector,const kstl::string& notificationName,CoreModifiable* sender=0);

	void removeObserver(CoreModifiable* observer,const kstl::string& notificationName="",CoreModifiable* sender=0,bool fromDestructor=false);

	void postNotificationName(const kstl::string& notificationName,kstl::vector<CoreModifiableAttribute*>& params,CoreModifiable* sender=0,void* data=0)
	{
		unsigned int notificationID=CharToID::GetID(notificationName);
		postNotificationName(notificationID,params,sender,data);
	}
	void postNotificationName(const kstl::string& notificationName,CoreModifiable* sender=0,void* data=0)
	{
		unsigned int notificationID=CharToID::GetID(notificationName);
		postNotificationName(notificationID,sender,data);
	}

	void postNotificationName(unsigned int notificationID,kstl::vector<CoreModifiableAttribute*>& params,CoreModifiable* sender=0,void* data=0);
	void postNotificationName(unsigned int notificationID,CoreModifiable* sender=0,void* data=0)
	{
		kstl::vector<CoreModifiableAttribute*> params;
		postNotificationName(notificationID,params,sender,data);
	}

	void Update();

protected:
	int myPostLevel;

	struct ObserverStruct
	{

		bool            myIsRemoved;
		bool			myIsStringItem;
		CoreItemSP		myCurrentItem = CoreItemSP(nullptr);
		unsigned int    mySelectorID;
		CoreModifiable* mySender;
	};

	struct ObserverStructVector
	{
		CheckUniqueObject		     myObserver;
		kstl::vector<ObserverStruct> myVector;
	};

	struct NotifVectorStruct
	{
		kstl::vector<kstl::string>		myVector;
	};

	// map associating observer to notifications...
	kstl::map<CheckUniqueObject, NotifVectorStruct> myObserverMap;

	// map associating notification name to observers map
	kstl::map<unsigned int,kstl::vector<ObserverStructVector> > myNotificationMap;

	void protectedAddObserver(CoreModifiable* observer, const ObserverStruct& newobstruct, const kstl::string& selector, const kstl::string&  notificationName);

	void protectedRemoveObserver(CheckUniqueObject& observer,const kstl::string& notificationName,CoreModifiable* sender,bool wasDestroyed);
	void protectedRemoveObserver(CheckUniqueObject& observer, bool wasDestroyed);
	void protectedSetRemoveState(CheckUniqueObject& observer,const kstl::string& notificationName,CoreModifiable* sender);
	void protectedSetRemoveState(CheckUniqueObject& observer);

	void manageRemoveObserverList();

	struct removeObserverStruct
	{
		kstl::string	notificationName;
		CoreModifiable* sender;
		bool			myWasDestroyed;
	};

	kstl::map<CheckUniqueObject,kstl::vector<removeObserverStruct> > myRemoveObserverMap;

	CoreItemEvaluationContext	myContext;

	std::recursive_mutex mMutex;
};

#endif //_NOTIFICATIONCENTER_H_
