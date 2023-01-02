// **********************************************************************
// * FILE  : APRSStream.h
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : PRS animation for one bone
// * COMMENT : PC Version
// *---------------------------------------------------------------------
// * DATES     : 21/06/2000
// **********************************************************************

#pragma once


#include "ABonesDefines.h"
#include "ABaseSystem.h"

#include "AStream.h"

namespace Kigs
{
	namespace Anim
	{
		// ****************************************
		// * PRSKey  class
		// * --------------------------------------
		/*!
		a position + a rotation + a scale
		\ingroup BoneAnimation
		*/
		// ****************************************
		class PRSKey : public LocalToGlobalBaseType
		{
		public:
			AMQuat  			m_RotationKey;
			AMPoint3			m_PositionKey;
#ifndef NO_SCALE
			AMPoint3			m_ScaleKey;
#endif

			void	set(const Matrix3x4& m);
			void	get(Matrix3x4& m);

			void	setIdentity()
			{
				m_RotationKey.SetIdentity();
				m_ScaleKey.Set(1.0f);
				m_PositionKey.Set(0.0f);
			}
		}
#ifdef _GNUC_
		__attribute__((aligned(16)))
#endif
			;

		// ****************************************
		// * APRSStream  class
		// * --------------------------------------
		/*!
			base class for PRS streams ( for bone animation )
			\ingroup BoneAnimation
		*/
		// ****************************************

		class APRSStream : public AStream<PRSKey>
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(APRSStream, AStream<PRSKey>, Animation)
				DECLARE_CONSTRUCTOR(APRSStream);

			// ******************************
			// * NewStreamOutputDataInstance
			// *-----------------------------
			/*! return an instance of the stream output data ( here a PRSKey )
				used by the channel to work on output data ( mixing, offset ... )
			*/
			// ******************************

			LocalToGlobalBaseType* NewStreamOutputDataInstance() override;

			// ******************************
			// * DeleteStreamOutputDataInstance
			// *-----------------------------
			/*! delete a PRSKey
			*/
			// ******************************

			void    DeleteStreamOutputDataInstance(LocalToGlobalBaseType* data) override;

			// ******************************
			// * InitData
			// *-----------------------------
			/*! Set data to m_CurrentPRSKey
			*/
			// ******************************

			void		InitData(LocalToGlobalBaseType* data) override;

			// ******************************
			// * LERPData
			// *-----------------------------
			/*! data = (this->m_CurrentPRSKey*t)+(data*(1-t))
			*/
			// ******************************

			void		LERPData(LocalToGlobalBaseType* data, Float t) override;

			// ******************************
			// * IdentityData
			// *-----------------------------
			/*! do : data = identity
				ie: set position to 0, scale to 1 and rotation to Quaternion identity
			*/
			// ******************************

			void IdentityData(LocalToGlobalBaseType* data) override;


			// ******************************
			// * SameData
			// *-----------------------------
			/*! do : data == other_data
			*/
			// ******************************
			bool      SameData(LocalToGlobalBaseType* data, LocalToGlobalBaseType* other_data) override;


			// ******************************
			// * GetSystemType
			// *-----------------------------
			/*!
			*/
			// ******************************

			std::string    GetSystemType() override;


			// ******************************
			// * MulData
			// *-----------------------------
			/*! - used to multiply two PRS keys
				- when this method is used for PRSkey transformations, the arguments have
				  the following meaning :
					destination = MulData(origin, transformation)
				- since transformations are not commutative, its important to respect this
				  order so that Qr1 = Qr2*Qr1 (Qr = Quaternion rotation)
				- the result is put in data1 which is overwritten.
			*/
			// ******************************

			void			MulData(LocalToGlobalBaseType* inout, LocalToGlobalBaseType* input) override;

			// ******************************
			// * MulInvData
			// *-----------------------------
			/*! - data1 = data1*(1/data2)
				- when this method is used for PRSkey transformations, it enables to
				- recover the origin PRSKey given a destination key and a transformation key :
				-   origin = MulInvData(dest,transformation);
				- Calling MulData and MulInvData as following enables to find the origin key again :
					 dest = MulData(origin,transformation);
					 origin = MulInvData(dest,transformation);
				- Do not mix up MulInvData and MulInvData2 which have different definitions.
			*/
			// ******************************

			void			MulInvData(LocalToGlobalBaseType* data1, LocalToGlobalBaseType* data2) override;

			// ******************************
			// * MulInvData2
			// *-----------------------------
			/*! - data1 = data1*(1/data2)
				- when this method is used for PRSkey transformations, it enables to
				- recover the transformation PRSKey given a destination key and an origin key :
				-   transformation = MulInvData2(dest,origin)
				- Calling MulData and MulInvData2 as following enables to find the transformation key again :
					dest = MulData(origin,transformation);
					transformation = MulInvData2(dest,origin);
				- Do not mix up MulInvData and MulInvData2 which have different definitions.
			*/
			// ******************************

			inline void			MulInvData2(LocalToGlobalBaseType* data1, LocalToGlobalBaseType* data2);

			// ******************************
			// * CopyData
			// *-----------------------------
			/*!do : data1 = data2
			*/
			// ******************************

			void			CopyData(LocalToGlobalBaseType* dst, LocalToGlobalBaseType* src) override;

			// ******************************
			// * SetAndModifyData
			// *-----------------------------
			/*! - currentData is the the current PRSKey in the global coordinate space
				- goalData is the global PRSKey where we would like the current PRSKey to move to
				- startAnimData is the PRSKey where the animation started at the end of the last loop
				- the animation moved the root PRSKey from startAnimData to currentData
				- this method computes a new startAnimData so that at the current time, the animation
				  will move the PRSKey precisely to the goal PRSKey.
				- as an example this method allows to rotate around the current root PRSKey instead
				  of rotating around the startAnim Key.
				- at the end the currentData is set to the given goadData
			*/
			// ******************************

			void	SetAndModifyData(LocalToGlobalBaseType* currentData, LocalToGlobalBaseType* goalData, LocalToGlobalBaseType* startAnimData) override;

			// ******************************
			// * GetStreamLength
			// *-----------------------------
			/*! return the length of the stream
			*/
			// ******************************

			ATimeValue	GetStreamLength() override;

			~APRSStream();


		protected:

			// +---------
			  // | protected members
			  // +---------

			PRSKey								m_CurrentPRSKey;
			ATimeValue							m_Length;	//seconds
		};


	}
}

