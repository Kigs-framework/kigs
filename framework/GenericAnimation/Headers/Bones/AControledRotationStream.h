// **********************************************************************
// * FILE  : AControledRotationStream.h
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : 
// * COMMENT : 
// *---------------------------------------------------------------------
// * DATES     :      
// **********************************************************************

#pragma once

#include "astream.h"
#include "ABonesDefines.h"
#include "AnimationResourceInfo.h"
#include "APRSStream.h"

namespace Kigs
{
    namespace Anim
    {
        // ****************************************
        // * AControledRotationStream  class
        // * --------------------------------------
        /*!
            stream doing a rotation along an axis
            \ingroup BoneAnimation
        */
        // ****************************************

        class AControledRotationStream : public APRSStream
        {
        public:

            DECLARE_CLASS_INFO(AControledRotationStream, APRSStream, Animation);
            DECLARE_CONSTRUCTOR(AControledRotationStream);

            // ******************************
            // * LERPData
            // *-----------------------------
            /*!
            */
            // ******************************

            void	LERPData(LocalToGlobalBaseType* data, Float t) override;



            // ******************************
            // * InitFromResource
            // *-----------------------------
            /*! init the stream with the given PRSControllerKey ( in the AnimationResourceInfo )
            */
            // ******************************

            void InitFromResource(AnimationResourceInfo* info, IntU32 streamindex) override;

            static AnimationResourceInfo* CreateAnimationResourceInfo(IntU32 group_id, Float*& angular_speed, Float*& max_angle, Float*& min_angle, Float*& angle, Float*& axis, bool*& use_min_max);
            static AnimationResourceInfo* CreateAnimationResourceInfo(IntU32 group_id_count, IntU32* group_id_list, Float*& angular_speed, Float*& max_angle, Float*& min_angle, Float*& angle, Float*& axis, bool*& use_min_max);
            virtual ~AControledRotationStream();

        protected:

            // ******************************
             // * UpdateData
             // *-----------------------------
             /*!
             */
             // ******************************

            void    UpdateData(LocalToGlobalBaseType* standdata) override;

            class RotationStruct
            {
            public:
                Float		m_AngularSpeed;
                Float		m_Angle;
                Float		m_MaxAngle;
                Float		m_MinAngle;
                Float		m_Axis[3];
                bool		m_UseMinMax;
                ATimeValue	m_LastTime;
            };

            // +---------
            // | protected members
            // +---------

            RotationStruct* m_RotationStruct;

        };

    }
}

