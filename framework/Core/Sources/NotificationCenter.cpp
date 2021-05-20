#include "PrecompiledHeaders.h"
#include "NotificationCenter.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include "CoreItemOperator.h"

//! constructor, init all parameters
NotificationCenter::NotificationCenter()
{
	mObserverMap.clear();
	mNotificationMap.clear();
	mRemoveObserverMap.clear();

	mPostLevel=0;

	// use context to pass variable to coreItemOperator
	mContext.mVariableList[LABEL_TO_ID(sender).toUInt()];
	mContext.mVariableList[LABEL_TO_ID(data).toUInt()];
}

NotificationCenter::~NotificationCenter()
{
	mContext.mVariableList[LABEL_TO_ID(sender).toUInt()];
	mContext.mVariableList[LABEL_TO_ID(data).toUInt()];

	mObserverMap.clear();
	mNotificationMap.clear();
}

void NotificationCenter::addObserver(CoreModifiable* observer, const std::string& selector, const std::string&  notificationName, CoreModifiable* sender)
{
	// first map : observers
	ObserverStruct newobstruct;
	newobstruct.mCurrentItem = nullptr;
	newobstruct.mIsStringItem = false;
	unsigned int selectorID = CharToID::GetID(selector);
	newobstruct.mSelectorID = selectorID;
	newobstruct.mSender = sender;
	newobstruct.mIsRemoved = false;

	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	if(selector.substr(0, 4) == "eval")
	{
		CoreItemEvaluationContext::SetContext(&mContext);

		// check if eval float or string
		if (selector.substr(4, 3) == "Str")
		{
			std::string toeval = selector.substr(7, selector.length() - 7);
			CoreItemSP toAdd = CoreItemOperator<std::string>::Construct(toeval, observer, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			newobstruct.mCurrentItem = toAdd;
			newobstruct.mIsStringItem = true;
		}
		else
		{
			std::string toeval = selector.substr(4, selector.length() - 4);

			CoreItemSP toAdd = CoreItemOperator<kfloat>::Construct(toeval, observer, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			newobstruct.mCurrentItem = toAdd;
		}
		CoreItemEvaluationContext::ReleaseContext();
	}

	protectedAddObserver(observer, newobstruct, selector, notificationName);
}

void NotificationCenter::protectedAddObserver(CoreModifiable* observer, const ObserverStruct& newobstruct, const std::string& selector, const std::string&  notificationName)
{

	unsigned int notificationID=CharToID::GetID(notificationName);

	// first check if observer already exist
	if(mObserverMap.find(observer)!=mObserverMap.end())
	{
		NotifVectorStruct&	currentvector=mObserverMap[observer];

		bool alreadythere=false;
		// check in current list if observer is not already there 
		for(unsigned int i=0;i<currentvector.mVector.size();i++)
		{
			if(currentvector.mVector[i] == notificationName)
			{
				alreadythere=true;
				break;
			}
		}
		if(!alreadythere)
		{
			currentvector.mVector.push_back(notificationName);
		}
	}
	else // first use of this observer
	{
		NotifVectorStruct	toadd;
		toadd.mVector.push_back(notificationName);
		mObserverMap[observer]=toadd;
		observer->flagAsNotificationCenterRegistered();
	}

	// second map : messages

	if(mNotificationMap.find(notificationID) != mNotificationMap.end())
	{
		std::vector<ObserverStructVector >& currentmap=mNotificationMap[notificationID];
		std::vector<ObserverStructVector >::iterator	itobs;

		bool	found=false;
		for(itobs = currentmap.begin();itobs != currentmap.end();itobs++)
		{
			if((*itobs).mObserver == observer)
			{
				bool alreadythere=false;
				int alreadythereindex=0;
				for(unsigned int i=0;i<(*itobs).mVector.size();i++)
				{
					if(((*itobs).mVector[i].mSelectorID == newobstruct.mSelectorID)&&(((*itobs).mVector[i].mSender == newobstruct.mSender)|| (newobstruct.mSender == 0)))
					{
						(*itobs).mVector[i].mIsRemoved=false;
						alreadythere=true;
						alreadythereindex=i;
						break;
						
					}
				}
				if(!alreadythere)
				{
					(*itobs).mVector.push_back(newobstruct);
				}
				else // already there, check if mSender is different
				{

					if(newobstruct.mSender == 0)
					{
						(*itobs).mVector[alreadythereindex].mSender=0;
					}
					else if(newobstruct.mSender != (*itobs).mVector[alreadythereindex].mSender)
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
			newvectortoadd.mVector.clear();
			newvectortoadd.mObserver=observer;
			newvectortoadd.mVector.push_back(newobstruct);
			currentmap.push_back(newvectortoadd);
		}
	}
	else
	{

		ObserverStructVector	newvectortoadd;
		newvectortoadd.mVector.clear();
		newvectortoadd.mObserver=observer;
		newvectortoadd.mVector.push_back(newobstruct);

		std::vector<ObserverStructVector > newmaptoadd;

		newmaptoadd.push_back(newvectortoadd);
		
		mNotificationMap[notificationID]=newmaptoadd;
	}
}

void NotificationCenter::removeObserver(CoreModifiable* observer,const std::string& notificationName,CoreModifiable* sender, bool fromDestructor)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	CoreModifiable* L_tmp = observer;
	protectedSetRemoveState(L_tmp,notificationName,sender);

	if(mRemoveObserverMap.find(observer)!=mRemoveObserverMap.end())
	{
		std::vector<removeObserverStruct>&	currentvector=mRemoveObserverMap[observer];
		removeObserverStruct	toAdd;
		toAdd.mNotificationName=notificationName;
		toAdd.mSender=sender;
		toAdd.mWasDestroyed = fromDestructor;
		currentvector.push_back(toAdd);		
	}
	else
	{
		std::vector<removeObserverStruct>	currentvector;
		
		removeObserverStruct	toAdd;
		toAdd.mNotificationName=notificationName;
		toAdd.mSender=sender;
		toAdd.mWasDestroyed = fromDestructor;
		currentvector.push_back(toAdd);	
		mRemoveObserverMap[observer]=currentvector;
		
	}
}

void NotificationCenter::manageRemoveObserverList()
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	for(auto it = mRemoveObserverMap.begin(); it != mRemoveObserverMap.end(); ++it)
	{
		CoreModifiable* obs=(*it).first;
		const std::vector<removeObserverStruct>&	currentvector=(*it).second;
		std::vector<removeObserverStruct>::const_iterator	itv;

		bool wasDestroyed = false;
		// check if was destroyed
		for (itv = currentvector.begin(); itv != currentvector.end(); ++itv)
		{
			wasDestroyed |= (*itv).mWasDestroyed;
		}

		for(itv=currentvector.begin();itv!=currentvector.end();++itv)
		{
			protectedRemoveObserver(obs,(*itv).mNotificationName,(*itv).mSender, wasDestroyed);
		}
	}

	mRemoveObserverMap.clear();

}

void NotificationCenter::protectedSetRemoveState(CoreModifiable* observer,const std::string&  notificationName,CoreModifiable* sender)
{
	if((sender==0)&&(notificationName==""))
	{
		protectedSetRemoveState(observer);
		return;
	}
	unsigned int notificationID=CharToID::GetID(notificationName);
				
	//bool canErase=false;
	auto it = mObserverMap.find(observer);
	if(it != mObserverMap.end())
	{
		NotifVectorStruct&	currentvector=mObserverMap[observer];
		for(auto itvector=currentvector.mVector.begin(); itvector != currentvector.mVector.end(); itvector++)
		{
			if(notificationName == (*itvector))
			{
				if(mNotificationMap.find(notificationID) != mNotificationMap.end())
				{
					std::vector<ObserverStructVector>&		currentprotocolmap=mNotificationMap[notificationID];
					std::vector<ObserverStructVector>::iterator  vectorToRemoveit;

					for(vectorToRemoveit=currentprotocolmap.begin();vectorToRemoveit!=currentprotocolmap.end();vectorToRemoveit++)
					{
						if((*vectorToRemoveit).mObserver == observer)
						{	
							std::vector<ObserverStruct>::iterator removestructit;
							for(removestructit = (*vectorToRemoveit).mVector.begin();removestructit != (*vectorToRemoveit).mVector.end();removestructit++)
							{
								if( ((*removestructit).mSender == sender) || (sender == 0))
								{
									(*removestructit).mIsRemoved=true;
									
									(*removestructit).mCurrentItem=CoreItemSP(nullptr);
									
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

void NotificationCenter::protectedSetRemoveState(CoreModifiable* observer)
{
	// search all notifications for this observer
	auto it = mObserverMap.find(observer);
	if(it != mObserverMap.end())
	{
		NotifVectorStruct&	currentvector=mObserverMap[observer];

		// search each notification name and set remove state

		unsigned int i;
		for(i=0;i<currentvector.mVector.size();i++)
		{
			std::string notifname=currentvector.mVector[i];
			unsigned int notificationID=CharToID::GetID(notifname);

			if(mNotificationMap.find(notificationID) != mNotificationMap.end())
			{
				std::vector<ObserverStructVector>&		currentprotocolmap=mNotificationMap[notificationID];
				std::vector<ObserverStructVector>::iterator  vectorToRemoveit;

				for(vectorToRemoveit=currentprotocolmap.begin();vectorToRemoveit!=currentprotocolmap.end();vectorToRemoveit++)
				{
					if((*vectorToRemoveit).mObserver == observer)
					{	
						std::vector<ObserverStruct>::iterator	itFlag;
						for(itFlag=(*vectorToRemoveit).mVector.begin();itFlag!=(*vectorToRemoveit).mVector.end();itFlag++)
						{
							(*itFlag).mIsRemoved=true;
							(*itFlag).mCurrentItem = CoreItemSP(nullptr);
						}
						break;
					}
				}
			}
		}
	}
}

void NotificationCenter::protectedRemoveObserver(CoreModifiable* observer, bool wasDestroyed)
{
	// search all notifications for this observer

	auto it = mObserverMap.find(observer);
	bool canErase=true;
	if(it != mObserverMap.end())
	{
		NotifVectorStruct&	currentvector=mObserverMap[observer];
		// search each notification name and remove observer form the list

		unsigned int i;
		for(i=0;i<currentvector.mVector.size();i++)
		{
			std::string notifname=currentvector.mVector[i];

			unsigned int notificationID=CharToID::GetID(notifname);

			auto itfound = mNotificationMap.find(notificationID);
			if(itfound != mNotificationMap.end())
			{
				std::vector<ObserverStructVector>&		currentprotocolmap=mNotificationMap[notificationID];
				std::vector<ObserverStructVector>::iterator  vectorToRemoveit;

				for(vectorToRemoveit=currentprotocolmap.begin();vectorToRemoveit!=currentprotocolmap.end();vectorToRemoveit++)
				{
					if((*vectorToRemoveit).mObserver == observer)
					{	
						std::vector<ObserverStruct>::iterator removestructit;

						std::vector<ObserverStruct>	reconstructList;
						reconstructList.clear();
						
						for(removestructit = (*vectorToRemoveit).mVector.begin();removestructit != (*vectorToRemoveit).mVector.end();removestructit++)
						{
							if((*removestructit).mIsRemoved)
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
							(*vectorToRemoveit).mVector=reconstructList;
						}
						else
						{
							(*vectorToRemoveit).mVector.clear();
							currentprotocolmap.erase(vectorToRemoveit);
						}

						break;
					}

					
				}

				if(currentprotocolmap.size()==0)
				{
					mNotificationMap.erase(itfound);				
				}
				
			}

		}
		if(canErase)
		{
			currentvector.mVector.clear();
			mObserverMap.erase(it);
			if (!wasDestroyed)
			{
				observer->unflagAsNotificationCenterRegistered();
			}
		}
	
	}

}

void NotificationCenter::protectedRemoveObserver(CoreModifiable* observer,const std::string& notificationName,CoreModifiable* sender, bool wasDestroyed)
{
	if((sender==0)&&(notificationName==""))
	{
		protectedRemoveObserver(observer, wasDestroyed);
		return;
	}
	unsigned int notificationID=CharToID::GetID(notificationName);

	bool canErase=false;
	auto it = mObserverMap.find(observer);
	if(it != mObserverMap.end())
	{
		NotifVectorStruct&	currentvector=mObserverMap[observer];
		std::vector<std::string>::iterator	itvector;

		for(itvector=currentvector.mVector.begin();itvector!=currentvector.mVector.end();itvector++)
		{
			if(notificationName == (*itvector))
			{
				auto itfound = mNotificationMap.find(notificationID);
				if(itfound != mNotificationMap.end())
				{
					std::vector<ObserverStructVector>&		currentprotocolmap=mNotificationMap[notificationID];
					std::vector<ObserverStructVector>::iterator  vectorToRemoveit;

					for(vectorToRemoveit=currentprotocolmap.begin();vectorToRemoveit!=currentprotocolmap.end();vectorToRemoveit++)
					{
						if((*vectorToRemoveit).mObserver == observer)
						{	
							std::vector<ObserverStruct>::iterator removestructit;
							for(removestructit = (*vectorToRemoveit).mVector.begin();removestructit != (*vectorToRemoveit).mVector.end();removestructit++)
							{
								if((*removestructit).mIsRemoved)
								{	
									if( ((*removestructit).mSender == sender) || (sender == 0))
									{
										(*vectorToRemoveit).mVector.erase(removestructit);
										if((*vectorToRemoveit).mVector.size() == 0)
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
						mNotificationMap.erase(itfound);
					}
				}

				if(canErase)
				{
					currentvector.mVector.erase(itvector);
					if(currentvector.mVector.size() == 0)
					{
						mObserverMap.erase(it);
						if (!wasDestroyed)
						{
							observer->unflagAsNotificationCenterRegistered();
						}
					}
				}
				break;
			}
		}
	}
}

void NotificationCenter::postNotificationName(const KigsID& notificationID,std::vector<CoreModifiableAttribute*>& params,CoreModifiable* sender,void* data)
{
	std::unique_lock<std::recursive_mutex> lk{ mMutex };
	if(mPostLevel == 0)
	{
		manageRemoveObserverList();
	}
	mPostLevel++;
	// search if some observers exists for this msg

	if(mNotificationMap.find(notificationID.toUInt()) != mNotificationMap.end())
	{
		// add notificationID to params
		CoreModifiableAttribute* notificationIDAttr = 0;
		

		unsigned int j;
		bool endj=false;
		j=0;

		bool swallow=false;

		while(!endj)
		{
			NotificationCenter::ObserverStructVector& currentObsStructV=mNotificationMap[notificationID.toUInt()][j];
			CoreModifiable* currentobserver = currentObsStructV.mObserver;

			if (notificationIDAttr == 0)
			{
				notificationIDAttr = new maUInt(*currentobserver, false, LABEL_AND_ID(NotificationID), notificationID.toUInt());
				params.push_back(notificationIDAttr);
			}

			bool endi=false;
			unsigned int i=0;
			while(!endi)
			{
				NotificationCenter::ObserverStruct&	currentobsStruct=currentObsStructV.mVector[i];

				if(!currentobsStruct.mIsRemoved)
				{		
					if((currentobsStruct.mSender == 0) || (currentobsStruct.mSender == sender))
					{
						if (currentobsStruct.mCurrentItem)
						{
							CoreItemEvaluationContext::SetContext(&mContext);
							mContext.mVariableList[LABEL_TO_ID(sender).toUInt()].push_back(sender->shared_from_this());
							// Warning faked cast
							mContext.mVariableList[LABEL_TO_ID(data).toUInt()].push_back(((GenericRefCountedBaseClass*) data)->shared_from_this());


							// push params
							std::vector<CoreModifiableAttribute*>::iterator	paramscurrent=params.begin();
							std::vector<CoreModifiableAttribute*>::iterator	paramsend = params.end();

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
										mContext.mVariableList[(*paramscurrent)->getLabelID().toUInt()].push_back(std::make_shared<CoreModifiableAttributeOperator<kfloat>>(*paramscurrent));
									}
									break;
									case CoreModifiable::ATTRIBUTE_TYPE::STRING:
									case CoreModifiable::ATTRIBUTE_TYPE::USSTRING:
									{
										mContext.mVariableList[(*paramscurrent)->getLabelID().toUInt()].push_back(std::make_shared<CoreModifiableAttributeOperator<std::string>>(*paramscurrent));
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
								mContext.mVariableList[(*paramscurrent)->getLabelID().toUInt()].pop_back();
								++paramscurrent;
							}

							swallow=true;
							break;
						}
						else if(currentobserver->CallMethod(currentobsStruct.mSelectorID,params,data,sender))
						{
							swallow=true;
							break;
						}
					}
				}

				i++;
				if(i>=currentObsStructV.mVector.size())
				{
					endi=true;
				}
			}

			if(swallow)
			{
				break;
			}

			j++;
			if(j>=mNotificationMap[notificationID.toUInt()].size())
			{
				endj=true;
			}
		}

		// remove notificationIDAttr from params
		if (notificationIDAttr != 0)
		{
			std::vector<CoreModifiableAttribute*>::iterator	paramscurrent = params.begin();
			std::vector<CoreModifiableAttribute*>::iterator	paramsend = params.end();

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
	mPostLevel--;
}

void	NotificationCenter::Update()
{
	manageRemoveObserverList();
}