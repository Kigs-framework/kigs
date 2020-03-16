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

	void addObserver(CoreModifiable* observer,const std::string& selector,const std::string& notificationName,CoreModifiable* sender=0);

	void removeObserver(CoreModifiable* observer,const std::string& notificationName="",CoreModifiable* sender=0,bool fromDestructor=false);

	void postNotificationName(const KigsID& notificationName, std::vector<CoreModifiableAttribute*>& params, CoreModifiable* sender = 0, void* data = 0);
	void postNotificationName(const KigsID& notificationName, CoreModifiable* sender = 0, void* data = 0)
	{
		std::vector<CoreModifiableAttribute*> params;
		postNotificationName(notificationName, params, sender, data);
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
		CoreModifiable* myObserver;
		std::vector<ObserverStruct> myVector;
	};

	struct NotifVectorStruct
	{
		std::vector<std::string>		myVector;
	};

	// map associating observer to notifications...
	std::unordered_map<CoreModifiable*, NotifVectorStruct> myObserverMap;

	// map associating notification name to observers map
	std::unordered_map<unsigned int,std::vector<ObserverStructVector> > myNotificationMap;

	void protectedAddObserver(CoreModifiable* observer, const ObserverStruct& newobstruct, const std::string& selector, const std::string&  notificationName);

	void protectedRemoveObserver(CoreModifiable* observer,const std::string& notificationName,CoreModifiable* sender,bool wasDestroyed);
	void protectedRemoveObserver(CoreModifiable* observer, bool wasDestroyed);
	void protectedSetRemoveState(CoreModifiable* observer,const std::string& notificationName,CoreModifiable* sender);
	void protectedSetRemoveState(CoreModifiable* observer);

	void manageRemoveObserverList();

	struct removeObserverStruct
	{
		std::string	notificationName;
		CoreModifiable* sender;
		bool			myWasDestroyed;
	};

	std::unordered_map<CoreModifiable* ,std::vector<removeObserverStruct> > myRemoveObserverMap;

	CoreItemEvaluationContext	myContext;

	std::recursive_mutex mMutex;
};

#endif //_NOTIFICATIONCENTER_H_
