#pragma once

#include <cstdint>
#include "CoreTypes.h"

namespace Kigs
{
	namespace Core
	{
		class StructLinkedListBase;

		struct LazyContentLinkedListItemStruct
		{
		protected:

			// hack use first two unused bits of address (aligned on 4 bytes at least)
			// to give a type
			enum class ItemType
			{
				UpgradorType = 0,
				ForwardSmartPtrType = 1,
				// available for future usage = 2,
				// available for future usage = 3
			};

			LazyContentLinkedListItemStruct(uintptr_t init) :mAddressAndType(init)
			{

			}

			static LazyContentLinkedListItemStruct	FromAddressAndType(StructLinkedListBase* realaddress, const LazyContentLinkedListItemStruct::ItemType settype)
			{
				LazyContentLinkedListItemStruct	result((uintptr_t)realaddress);
				result.mAddressAndType |= (uintptr_t)settype;
				return result;
			}


			friend class CoreModifiable;
			friend class StructLinkedListBase;
			friend struct LazyContent;
			uintptr_t	mAddressAndType;

		public:
			operator StructLinkedListBase* () const
			{
				StructLinkedListBase* realaddress = (StructLinkedListBase*)(mAddressAndType & (((uintptr_t)-1) ^ (uintptr_t)3));
				return realaddress;
			}
			operator uintptr_t () const
			{
				return mAddressAndType;
			}

			LazyContentLinkedListItemStruct::ItemType	getType() const
			{
				return (LazyContentLinkedListItemStruct::ItemType)(mAddressAndType & 3);
			}
		};

		class StructLinkedListBase
		{
		protected:

			LazyContentLinkedListItemStruct mNextItem = 0; // store address + type 
		public:
			StructLinkedListBase* getNext(const LazyContentLinkedListItemStruct::ItemType searchtype) const
			{
				LazyContentLinkedListItemStruct next = mNextItem;
				while ((uintptr_t)next)
				{
					StructLinkedListBase* realaddress = (StructLinkedListBase*)next;
					if ((next & 3) == (u32)searchtype)
					{
						return realaddress;
					}
					next = realaddress->mNextItem;
				}
				return nullptr;
			}

			LazyContentLinkedListItemStruct getNext() const
			{
				return mNextItem;
			}

		};

	}
}
