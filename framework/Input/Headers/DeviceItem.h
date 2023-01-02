#pragma once

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"

namespace Kigs
{
	namespace Input
	{
#define GetTypedValue(a)   getValueMethod((a*)0)
#define DECL_VP_TypeValue(a)	virtual a getValueMethod(a*) const =0; \
								virtual void SetValue(a)     =0;
#define DECL_TypeValue(a)	virtual a getValueMethod(a*) const {assert(0);return *(a*)0;} \
							virtual void SetValue(a)    {assert(0);return;}

		// ****************************************
		// * DeviceItemBaseState class
		// * --------------------------------------
		/**
		 * \file	DeviceItem.h
		 * \class	DeviceItemBaseState
		 * \ingroup Input
		 * \brief	abstract base class for device item state
		 */
		 // ****************************************
		class DeviceItemBaseState
		{
		public:
			friend class DeviceItem;

			/**
			 * \brief	coppy this DeviceItemBaseState
			 * \fn 		virtual DeviceItemBaseState*	MakeCopy()const =0 ;
			 * \return	the copy of this DeviceItemBaseState
			 */
			virtual DeviceItemBaseState* MakeCopy()const = 0;

			DECL_VP_TypeValue(float);
			DECL_VP_TypeValue(int);
			DECL_VP_TypeValue(v3f);

			/**
			 * \brief	destructor
			 * \fn 		~DeviceItemBaseState();
			 */
			virtual ~DeviceItemBaseState() {};

		protected:
			/**
			 * \brief	constructor
			 * \fn 		DeviceItemBaseState();
			 */
			DeviceItemBaseState() {};
		};

		// ****************************************
		// * DeviceItemState class
		// * --------------------------------------
		/**
		 * \class	DeviceItemState
		 * \file	DeviceItem.h
		 * \ingroup Input
		 * \brief	base class for typed device item state
		 */
		 // ****************************************
		template<typename valueType>
		class DeviceItemState : public DeviceItemBaseState
		{
		public:
			friend class DeviceItem;

			/**
			 * \brief	constructor
			 * \fn 		DeviceItemState(valueType value)
			 * \param	value : state of the device
			 */
			DeviceItemState(valueType value) : mValue(value)
			{
			}

			/**
			 * \brief	destructor
			 * \fn 		~DeviceItemState();
			 */
			virtual ~DeviceItemState()
			{
			}

			/**
			 * \brief	copy a DeviceItemBaseState
			 * \fn 		DeviceItemBaseState*	MakeCopy() const
			 * \return	a copy of the DeviceItemBaseState
			 */
			DeviceItemBaseState* MakeCopy() const
			{
				DeviceItemState<valueType>* localstate = new DeviceItemState<valueType>(mValue);
				return localstate;
			}

			DECL_TypeValue(float);
			DECL_TypeValue(int);
			DECL_TypeValue(v3f);

		protected:
			//! state of the device
			valueType	mValue;
		};

		template<>
		inline float DeviceItemState<float>::getValueMethod(float*) const { return mValue; }
		template<>
		inline int   DeviceItemState<float>::getValueMethod(int*)   const { return (int)mValue; }
		template<>
		inline void  DeviceItemState<float>::SetValue(float v) { mValue = v; }
		template<>
		inline void  DeviceItemState<float>::SetValue(int v) { mValue = (float)v; }

		template<>
		inline float DeviceItemState<int>::getValueMethod(float*)  const { return (float)mValue; }
		template<>
		inline int   DeviceItemState<int>::getValueMethod(int*)    const { return mValue; }
		template<>
		inline void  DeviceItemState<int>::SetValue(float v) { mValue = (int)v; }
		template<>
		inline void  DeviceItemState<int>::SetValue(int v) { mValue = v; }

		template<>
		inline v3f DeviceItemState<v3f>::getValueMethod(v3f*)  const { return mValue; }
		template<>
		inline void  DeviceItemState<v3f>::SetValue(v3f v) { mValue = v; }

		// ****************************************
		// * DeviceItem class
		// * --------------------------------------
		/**
		 * \class	DeviceItem
		 * \file	DeviceItem.h
		 * \ingroup Input
		 * \brief	manage device item
		 */
		 // ****************************************
		class DeviceItem
		{
			friend class InputDevice;
		public:
			/**
			 * \brief	constructor
			 * \fn 		DeviceItem(const DeviceItemBaseState& state);
			 * \param	state : states of the device
			 */
			DeviceItem(const DeviceItemBaseState& state);

			/**
			 * \brief	constructor by copy
			 * \fn 		DeviceItem(const DeviceItem& item)
			 * \param	item : DeviceItem to copy
			 */
			DeviceItem(const DeviceItem& item)
			{
				mStates = item.mStates->MakeCopy();
			}

			/**
			 * \brief	coppy this deviceItem
			 * \fn 		DeviceItem*	MakeCopy() const
			 * \return	the copy of this deviceItem
			 */
			DeviceItem* MakeCopy() const
			{
				DeviceItem* localitem = new DeviceItem(*mStates);
				return localitem;
			}

			/**
			 * \brief	destructor
			 * \fn 		~DeviceItem();
			 */
			virtual ~DeviceItem();

			/**
			 * \brief	get the states of the device
			 * \fn 		inline DeviceItemBaseState*	getState()
			 * \return	a lilst of DeviceItemBaseState
			 */
			inline DeviceItemBaseState* getState() { return mStates; }

		protected:
			//! list of state
			DeviceItemBaseState* mStates;
		};


		/**
		 * \brief	constructor by copy
		 * \fn 		inline DeviceItem::DeviceItem(const DeviceItemBaseState& state)
		 * \param	item : DeviceItem to copy
		 */
		inline DeviceItem::DeviceItem(const DeviceItemBaseState& state)
		{
			mStates = state.MakeCopy();
		}

		/**
		 * \brief	destructor
		 * \fn 		inline DeviceItem::~DeviceItem()
		 */
		inline DeviceItem::~DeviceItem()
		{
			delete mStates;
		}

	}
}
