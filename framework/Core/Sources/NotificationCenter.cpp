#include "PrecompiledHeaders.h"
#include "NotificationCenter.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include "CoreItemOperator.h"

//! constructor, init all parameters
NotificationCenter::NotificationCenter()
{
	myObserverMap.clear();
	myNotificationMap.clear();
	myRemoveObserverMap.clear();

	myPostLevel=0;

	// use context to pass variable to coreItemOperator
	myContext.myVariableList[LABEL_TO_ID(sender).toUInt()] = 0;
	myContext.myVariableList[LABEL_TO_ID(data).toUInt()] = 0;
}

NotificationCenter::~NotificationCenter()
{
	myContext.myVariableList[LABEL_TO_ID(sender).toUInt()] = 0;
	myContext.myVariableList[LABEL_TO_ID(data).toUInt()] = 0;

	myObserverMap.clear();
	myNotificationMap.clear();
}

void NotificationCenter::addObserver(CoreModifiable* observer, const kstl::string& selector, const kstl::string&  notificationName, CoreModifiable* sender)
{
	// first map : observers
	ObserverStruct newobstruct;
	newobstruct.myCurrentItem = nullptr;
	newobstruct.myIsStringItem = false;
	unsigned int selectorID = CharToID::GetID(selector);
	newobstruct.mySelectorID = selectorID;
	newobstruct.mySender = sender;
	newobstruct.myIsRemoved = false;

	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	if(selector.substr(0, 4) == "eval")
	{
		CoreItemEvaluationContext::SetContext(&myContext);

		// check if eval float or string
		if (selector.substr(4, 3) == "Str")
		{
			kstl::string toeval = selector.substr(7, selector.length() - 7);
			CoreItemSP toAdd = CoreItemOperator<kstl::string>::Construct(toeval, observer, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			newobstruct.myCurrentItem = toAdd;
			newobstruct.myIsStringItem = true;
		}
		else
		{
			kstl::string toeval = selector.substr(4, selector.length() - 4);

			CoreItemSP toAdd = CoreItemOperator<kfloat>::Construct(toeval, observer, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			newobstruct.myCurrentItem = toAdd;
		}
		CoreItemEvaluationContext::ReleaseContext();
	}

	protectedAddObserver(observer, newobstruct, selector, notificationName);
}

void NotificationCenter::protectedAddObserver(CoreModifiable* observer, const ObserverStruct& newobstruct, const kstl::string& selector, const kstl::string&  notificationName)
{

	unsigned int notificationID=CharToID::GetID(notificationName);

	// first check if observer already exist
	if(myObserverMap.find(observer)!=myObserverMap.end())
	{
		NotifVectorStruct&	currentvector=myObserverMap[observer];

		bool alreadythere=false;
		// check in current list if observer is not already there 
		for(unsigned int i=0;i<currentvector.myVector.size();i++)
		{
			if(currentvector.myVector[i] == notificationName)
			{
				alreadythere=true;
				break;
			}
		}
		if(!alreadythere)
		{
			currentvector.myVector.push_back(notificationName);
		}
	}
	else // first use of this observer
	{
		NotifVectorStruct	toadd;
		toadd.myVector.push_back(notificationName);
		myObserverMap[observer]=toadd;
		observer->flagAsNotificationCenterRegistered();
	}

	// second map : messages

	if(myNotificationMap.find(notificationID) != myNotificationMap.end())
	{
		kstl::vector<ObserverStructVector >& currentmap=myNotificationMap[notificationID];
		kstl::vector<ObserverStructVector >::iterator	itobs;

		bool	found=false;
		for(itobs = currentmap.begin();itobs != currentmap.end();itobs++)
		{
			if((*itobs).myObserver == observer)
			{
				bool alreadythere=false;
				int alreadythereindex=0;
				for(unsigned int i=0;i<(*itobs).myVector.size();i++)
				{
					if(((*itobs).myVector[i].mySelectorID == newobstruct.mySelectorID)&&(((*itobs).myVector[i].mySender == newobstruct.mySender)|| (newobstruct.mySender == 0)))
					{
						(*itobs).myVector[i].myIsRemoved=false;
						alreadythere=true;
						alreadythereindex=i;
						break;
						
					}
				}
				if(!alreadythere)
				{
					(*itobs).myVector.push_back(newobstruct);
				}
				else // already there, check if sender is different
				{

					if(newobstruct.mySender == 0)
					{
						(*itobs).myVector[alreadythereindex].mySender=0;
					}
					else if(newobstruct.mySender != (*itobs).myVector[alreadythereindex].mySender)
					{
						// what can I do ?
					}
				}
				found=true;
				break;
			}
		}

		if(!found)
		{
			ObserverStructVector	newvectortoadd;
			newvectortoadd.myVector.clear();
			newvectortoadd.myObserver=observer;
			newvectortoadd.myVector.push_back(newobstruct);
			currentmap.push_back(newvectortoadd);
		}
	}
	else
	{

		ObserverStructVector	newvectortoadd;
		newvectortoadd.myVector.clear();
		newvectortoadd.myObserver=observer;
		newvectortoadd.myVector.push_back(newobstruct);

		kstl::vector<ObserverStructVector > newmaptoadd;

		newmaptoadd.push_back(newvectortoadd);
		
		myNotificationMap[notificationID]=newmaptoadd;
	}
}

void NotificationCenter::removeObserver(CoreModifiable* observer,const kstl::string& notificationName,CoreModifiable* sender, bool fromDestructor)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	CheckUniqueObject L_tmp = observer;
	protectedSetRemoveState(L_tmp,notificationName,sender);

	if(myRemoveObserverMap.find(observer)!=myRemoveObserverMap.end())
	{
		kstl::vector<removeObserverStruct>&	currentvector=myRemoveObserverMap[observer];
		removeObserverStruct	toAdd;
		toAdd.notificationName=notificationName;
		toAdd.sender=sender;
		toAdd.myWasDestroyed = fromDestructor;
		currentvector.push_back(toAdd);		
	}
	else
	{
		kstl::vector<removeObserverStruct>	currentvector;
		
		removeObserverStruct	toAdd;
		toAdd.notificationName=notificationName;
		toAdd.sender=sender;
		toAdd.myWasDestroyed = fromDestructor;
		currentvector.push_back(toAdd);	
		myRemoveObserverMap[observer]=currentvector;
		
	}
}

void NotificationCenter::manageRemoveObserverList()
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	kstl::map<CheckUniqueObject,kstl::vector<removeObserverStruct> >::const_iterator it;
	for(it=myRemoveObserverMap.begin();it!=myRemoveObserverMap.end();++it)
	{
		CheckUniqueObject obs=(*it).first;
		const kstl::vector<removeObserverStruct>&	currentvector=(*it).second;
		kstl::vector<removeObserverStruct>::const_iterator	itv;

		bool wasDestroyed = false;
		// check if was destroyed
		for (itv = currentvector.begin(); itv != currentvector.end(); ++itv)
		{
			wasDestroyed |= (*itv).myWasDestroyed;
		}

		for(itv=currentvector.begin();itv!=currentvector.end();++itv)
		{
			protectedRemoveObserver(obs,(*itv).notificationName,(*itv).sender, wasDestroyed);
		}
	}

	myRemoveObserverMap.clear();

}

void NotificationCenter::protectedSetRemoveState(CheckUniqueObject& observer,const kstl::string&  notificationName,CoreModifiable* sender)
{
	if((sender==0)&&(notificationName==""))
	{
		protectedSetRemoveState(observer);
		return;
	}
	unsigned int notificationID=CharToID::GetID(notificationName);
				
	//bool canErase=false;
	kstl::map<CheckUniqueObject,NotifVectorStruct >::iterator	it=myObserverMap.find(observer);
	if(it!=myObserverMap.end())
	{
		NotifVectorStruct&	currentvector=myObserverMap[observer];
		kstl::vector<kstl::string>::iterator	itvector;

		for(itvector=currentvector.myVector.begin();itvector!=currentvector.myVector.end();itvector++)
		{
			if(notificationName == (*itvector))
			{
				if(myNotificationMap.find(notificationID) != myNotificationMap.end())
				{
					kstl::vector<ObserverStructVector>&		currentprotocolmap=myNotificationMap[notificationID];
					kstl::vector<ObserverStructVector>::iterator  vectorToRemoveit;

					for(vectorToRemoveit=currentprotocolmap.begin();vectorToRemoveit!=currentprotocolmap.end();vectorToRemoveit++)
					{
						if((*vectorToRemoveit).myObserver == observer)
						{	
							kstl::vector<ObserverStruct>::iterator removestructit;
							for(removestructit = (*vectorToRemoveit).myVector.begin();removestructit != (*vectorToRemoveit).myVector.end();removestructit++)
							{
								if( ((*removestructit).mySender == sender) || (sender == 0))
								{
									(*removestructit).myIsRemoved=true;
									
									(*removestructit).myCurrentItem=CoreItemSP(nullptr);
									
									break;
								}
							}
							break;
						}
					}
				}
				break;
			}
		}
	}
}

void NotificationCenter::protectedSetRemoveState(CheckUniqueObject& observer)
{
	// search all notifications for this observer
	kstl::map<CheckUniqueObject,NotifVectorStruct >::iterator	it=myObserverMap.find(observer);
	if(it!=myObserverMap.end())
	{
		NotifVectorStruct&	currentvector=myObserverMap[observer];

		// search each notification name and set remove state

		unsigned int i;
		for(i=0;i<currentvector.myVector.size();i++)
		{
			kstl::string notifname=currentvector.myVector[i];
			unsigned int notificationID=CharToID::GetID(notifname);

			if(myNotificationMap.find(notificationID) != myNotificationMap.end())
			{
				kstl::vector<ObserverStructVector>&		currentprotocolmap=myNotificationMap[notificationID];
				kstl::vector<ObserverStructVector>::iterator  vectorToRemoveit;

				for(vectorToRemoveit=currentprotocolmap.begin();vectorToRemoveit!=currentprotocolmap.end();vectorToRemoveit++)
				{
					if((*vectorToRemoveit).myObserver == observer)
					{	
						kstl::vector<ObserverStruct>::iterator	itFlag;
						for(itFlag=(*vectorToRemoveit).myVector.begin();itFlag!=(*vectorToRemoveit).myVector.end();itFlag++)
						{
							(*itFlag).myIsRemoved=true;
							(*itFlag).myCurrentItem = CoreItemSP(nullptr);
						}
						break;
					}
				}
			}
		}
	}
}

void NotificationCenter::protectedRemoveObserver(CheckUniqueObject& observer, bool wasDestroyed)
{
	// search all notifications for this observer

	kstl::map<CheckUniqueObject,NotifVectorStruct >::iterator	it=myObserverMap.find(observer);
	bool canErase=true;
	if(it!=myObserverMap.end())
	{
		NotifVectorStruct&	currentvector=myObserverMap[observer];
		// search each notification name and remove observer form the list

		unsigned int i;
		for(i=0;i<currentvector.myVector.size();i++)
		{
			kstl::string notifname=currentvector.myVector[i];

			unsigned int notificationID=CharToID::GetID(notifname);

			kstl::map<unsigned int,kstl::vector<ObserverStructVector> >::iterator	itfound=myNotificationMap.find(notificationID);
			if(itfound != myNotificationMap.end())
			{
				kstl::vector<ObserverStructVector>&		currentprotocolmap=myNotificationMap[notificationID];
				kstl::vector<ObserverStructVector>::iterator  vectorToRemoveit;

				for(vectorToRemoveit=currentprotocolmap.begin();vectorToRemoveit!=currentprotocolmap.end();vectorToRemoveit++)
				{
					if((*vectorToRemoveit).myObserver == observer)
					{	
						kstl::vector<ObserverStruct>::iterator removestructit;

						kstl::vector<ObserverStruct>	reconstructList;
						reconstructList.clear();
						
						for(removestructit = (*vectorToRemoveit).myVector.begin();removestructit != (*vectorToRemoveit).myVector.end();removestructit++)
						{
							if((*removestructit).myIsRemoved)
							{	
								
								
							}
							else
							{
								reconstructList.push_back((*removestructit));
								canErase=false;
							}
						}

						if(reconstructList.size())
						{
							(*vectorToRemoveit).myVector=reconstructList;
						}
						else
						{
							(*vectorToRemoveit).myVector.clear();
							currentprotocolmap.erase(vectorToRemoveit);
						}

						break;
					}

					
				}

				if(currentprotocolmap.size()==0)
				{
					myNotificationMap.erase(itfound);				
				}
				
			}

		}
		if(canErase)
		{
			currentvector.myVector.clear();
			myObserverMap.erase(it);
			if (!wasDestroyed)
			{
				((CoreModifiable*)(RefCountedClass*)observer)->unflagAsNotificationCenterRegistered();
			}
		}
	
	}

}

void NotificationCenter::protectedRemoveObserver(CheckUniqueObject& observer,const kstl::string& notificationName,CoreModifiable* sender, bool wasDestroyed)
{
	if((sender==0)&&(notificationName==""))
	{
		protectedRemoveObserver(observer, wasDestroyed);
		return;
	}
	unsigned int notificationID=CharToID::GetID(notificationName);

	bool canErase=false;
	kstl::map<CheckUniqueObject,NotifVectorStruct >::iterator	it=myObserverMap.find(observer);
	if(it!=myObserverMap.end())
	{
		NotifVectorStruct&	currentvector=myObserverMap[observer];
		kstl::vector<kstl::string>::iterator	itvector;

		for(itvector=currentvector.myVector.begin();itvector!=currentvector.myVector.end();itvector++)
		{
			if(notificationName == (*itvector))
			{
				kstl::map<unsigned int,kstl::vector<ObserverStructVector> >::iterator	itfound=myNotificationMap.find(notificationID);

				if(itfound != myNotificationMap.end())
				{
					kstl::vector<ObserverStructVector>&		currentprotocolmap=myNotificationMap[notificationID];
					kstl::vector<ObserverStructVector>::iterator  vectorToRemoveit;

					for(vectorToRemoveit=currentprotocolmap.begin();vectorToRemoveit!=currentprotocolmap.end();vectorToRemoveit++)
					{
						if((*vectorToRemoveit).myObserver == observer)
						{	
							kstl::vector<ObserverStruct>::iterator removestructit;
							for(removestructit = (*vectorToRemoveit).myVector.begin();removestructit != (*vectorToRemoveit).myVector.end();removestructit++)
							{
								if((*removestructit).myIsRemoved)
								{	
									if( ((*removestructit).mySender == sender) || (sender == 0))
									{
										(*vectorToRemoveit).myVector.erase(removestructit);
										if((*vectorToRemoveit).myVector.size() == 0)
										{
											currentprotocolmap.erase(vectorToRemoveit);
										}
										canErase=true;
										break;
									}
								}
							}
							break;
						}
					}
					if(currentprotocolmap.size() == 0)
					{
						myNotificationMap.erase(itfound);
					}
				}

				if(canErase)
				{
					currentvector.myVector.erase(itvector);
					if(currentvector.myVector.size() == 0)
					{
						myObserverMap.erase(it);
						if (!wasDestroyed)
						{
							((CoreModifiable*)(RefCountedClass*)observer)->unflagAsNotificationCenterRegistered();
						}
					}
				}
				break;
			}
		}
	}
}

void NotificationCenter::postNotificationName(const KigsID& notificationID,kstl::vector<CoreModifiableAttribute*>& params,CoreModifiable* sender,void* data)
{
	std::unique_lock<std::recursive_mutex> lk{ mMutex };
	if(myPostLevel == 0)
	{
		manageRemoveObserverList();
	}
	myPostLevel++;
	// search if some observers exists for this msg

	if(myNotificationMap.find(notificationID.toUInt()) != myNotificationMap.end())
	{
		// add notificationID to params
		CoreModifiableAttribute* notificationIDAttr = 0;
		

		unsigned int j;
		bool endj=false;
		j=0;

		bool swallow=false;

		while(!endj)
		{
			NotificationCenter::ObserverStructVector& currentObsStructV=myNotificationMap[notificationID.toUInt()][j];

			CoreModifiable* currentobserver=(CoreModifiable*)(RefCountedBaseClass*)currentObsStructV.myObserver;

			if (notificationIDAttr == 0)
			{
				notificationIDAttr = new maUInt(*currentobserver, false, LABEL_AND_ID(NotificationID), notificationID.toUInt());
				params.push_back(notificationIDAttr);
			}

			bool endi=false;
			unsigned int i=0;
			while(!endi)
			{
				NotificationCenter::ObserverStruct&	currentobsStruct=currentObsStructV.myVector[i];

				if(!currentobsStruct.myIsRemoved)
				{		
					if((currentobsStruct.mySender == 0) || (currentobsStruct.mySender == sender))
					{
						if (!currentobsStruct.myCurrentItem.isNil())
						{
							CoreItemEvaluationContext::SetContext(&myContext);
							myContext.myVariableList[LABEL_TO_ID(sender).toUInt()] = sender;
							// Warning faked cast
							myContext.myVariableList[LABEL_TO_ID(data).toUInt()] = (RefCountedBaseClass*)data;


							// push params
							kstl::vector<CoreModifiableAttribute*>::iterator	paramscurrent=params.begin();
							kstl::vector<CoreModifiableAttribute*>::iterator	paramsend = params.end();

							while (paramscurrent != paramsend)
							{

								CoreModifiable::ATTRIBUTE_TYPE type = (*paramscurrent)->getType();

								switch (type)
								{
									case CoreModifiable::ATTRIBUTE_TYPE::BOOL:
									case CoreModifiable::ATTRIBUTE_TYPE::CHAR:
									case CoreModifiable::ATTRIBUTE_TYPE::SHORT:
									case CoreModifiable::ATTRIBUTE_TYPE::INT:
									case CoreModifiable::ATTRIBUTE_TYPE::LONG:
									case CoreModifiable::ATTRIBUTE_TYPE::UCHAR:
									case CoreModifiable::ATTRIBUTE_TYPE::USHORT:
									case CoreModifiable::ATTRIBUTE_TYPE::UINT:
									case CoreModifiable::ATTRIBUTE_TYPE::ULONG:
									case CoreModifiable::ATTRIBUTE_TYPE::FLOAT:
									case CoreModifiable::ATTRIBUTE_TYPE::DOUBLE:
									{
										myContext.myVariableList[(*paramscurrent)->getLabelID().toUInt()] = new CoreModifiableAttributeOperator<kfloat>((*paramscurrent));
									}
									break;
									case CoreModifiable::ATTRIBUTE_TYPE::STRING:
									case CoreModifiable::ATTRIBUTE_TYPE::USSTRING:
									{
										myContext.myVariableList[(*paramscurrent)->getLabelID().toUInt()] = new CoreModifiableAttributeOperator<kstl::string>((*paramscurrent));
									}
									break;
									default:
										break;
								}
								
								++paramscurrent;
							}
							
							CoreItemEvaluationContext::ReleaseContext();
							
							// pop params
							paramscurrent = params.begin();
							paramsend = params.begin();

							while (paramscurrent != paramsend)
							{
								myContext.myVariableList[(*paramscurrent)->getLabelID().toUInt()]->Destroy();
								myContext.myVariableList[(*paramscurrent)->getLabelID().toUInt()] = 0;
								++paramscurrent;
							}

							swallow=true;
							break;
						}
						else if(currentobserver->CallMethod(currentobsStruct.mySelectorID,params,data,sender))
						{
							swallow=true;
							break;
						}
					}
				}

				i++;
				if(i>=currentObsStructV.myVector.size())
				{
					endi=true;
				}
			}

			if(swallow)
			{
				break;
			}

			j++;
			if(j>=myNotificationMap[notificationID.toUInt()].size())
			{
				endj=true;
			}
		}

		// remove notificationIDAttr from params
		if (notificationIDAttr != 0)
		{
			kstl::vector<CoreModifiableAttribute*>::iterator	paramscurrent = params.begin();
			kstl::vector<CoreModifiableAttribute*>::iterator	paramsend = params.end();

			while (paramscurrent != paramsend)
			{
				if ((*paramscurrent)->getLabelID() == LABEL_TO_ID(NotificationID))
				{
					params.erase(paramscurrent);
					
					break;
				}
				++paramscurrent;
			}
			delete notificationIDAttr;
			notificationIDAttr = 0;
		}
	}
	myPostLevel--;
}

void	NotificationCenter::Update()
{
	manageRemoveObserverList();
}