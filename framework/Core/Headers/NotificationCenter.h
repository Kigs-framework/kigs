#pragma once

#include "CoreModifiable.h"
#include "CoreItemOperator.h"

#include <mutex>

namespace Kigs
{
	namespace Core
	{
		// ****************************************
		// * NotificationCenter class
		// * --------------------------------------
		/**
		 * \file	NotificationCenter.h
		 * \class	NotificationCenter
		 * \ingroup Core
		 * \brief	Manage messages between CoreModifiable
		 *
		 * This class should be used as a singleton.
		 */
		 // ****************************************
		class NotificationCenter
		{
		public:
			NotificationCenter();
			~NotificationCenter();

			void addObserver(CoreModifiable* observer, const std::string& selector, const std::string& notificationName, CoreModifiable* sender = 0);

			void removeObserver(CoreModifiable* observer, const std::string& notificationName = "", CoreModifiable* sender = 0, bool fromDestructor = false);

			void postNotificationName(const KigsID& notificationName, std::vector<CoreModifiableAttribute*>& params, CoreModifiable* sender = 0, void* data = 0);
			void postNotificationName(const KigsID& notificationName, CoreModifiable* sender = 0, void* data = 0)
			{
				std::vector<CoreModifiableAttribute*> params;
				postNotificationName(notificationName, params, sender, data);
			}

			void Update();

		protected:
			int mPostLevel;

			struct ObserverStruct
			{

				bool            mIsRemoved;
				bool			mIsStringItem;
				CoreItemSP		mCurrentItem = CoreItemSP(nullptr);
				unsigned int    mSelectorID;
				CoreModifiable* mSender;
			};

			struct ObserverStructVector
			{
				CoreModifiable* mObserver;
				std::vector<ObserverStruct> mVector;
			};

			struct NotifVectorStruct
			{
				std::vector<std::string>		mVector;
			};

			// map associating observer to notifications...
			std::unordered_map<CoreModifiable*, NotifVectorStruct> mObserverMap;

			// map associating notification name to observers map
			std::unordered_map<unsigned int, std::vector<ObserverStructVector> > mNotificationMap;

			void protectedAddObserver(CoreModifiable* observer, const ObserverStruct& newobstruct, const std::string& selector, const std::string& notificationName);

			void protectedRemoveObserver(CoreModifiable* observer, const std::string& notificationName, CoreModifiable* sender, bool wasDestroyed);
			void protectedRemoveObserver(CoreModifiable* observer, bool wasDestroyed);
			void protectedSetRemoveState(CoreModifiable* observer, const std::string& notificationName, CoreModifiable* sender);
			void protectedSetRemoveState(CoreModifiable* observer);

			void manageRemoveObserverList();

			struct removeObserverStruct
			{
				std::string		mNotificationName;
				CoreModifiable* mSender;
				bool			mWasDestroyed;
			};

			std::unordered_map<CoreModifiable*, std::vector<removeObserverStruct> > mRemoveObserverMap;

			CoreItemEvaluationContext	mContext;

			std::recursive_mutex mMutex;
		};

	}
}
