// **********************************************************************
// * FILE  : ABoneChannel.h
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : animation channel for one bone
// * COMMENT : 
// *---------------------------------------------------------------------
// * DATES     : 24/05/2000
// **********************************************************************

#pragma once

#include "AChannel.h"
#include "ABonesDefines.h"
#include "Bones/APRSStream.h"

namespace Kigs
{
    namespace Anim
    {
        using namespace Core;

        // ----------------------------------------------------------------------------

        // ****************************************
        // * ABoneChannel  class
        // * --------------------------------------
        /*!
            Bone channel class, used by ABoneSystem class
            \ingroup BoneAnimation
        */
        // ****************************************

        class ABoneChannel : public AChannel<PRSKey>
        {
        public:
            DECLARE_CLASS_INFO(ABoneChannel, AChannel<PRSKey>, Animation);
            DECLARE_CONSTRUCTOR(ABoneChannel);

            // ******************************
            // * UpdateTransformParameters
            // *-----------------------------
            /*! after the flux mix is done, the parameters for the transformation must be set
            */
            // ******************************

            void    UpdateTransformParameters() override;

            // ******************************
            // * GetCurrentPRSMatrix
            // *-----------------------------
            /*! return the PRS matrix
            */
            // ******************************

            inline const AMMatrix& GetCurrentPRSMatrix();

            // ******************************
            // * SetStandStreamData
            // *-----------------------------
            /*! set the stand stream data for this channel
            */
            // ******************************

            void            SetStandStreamData() override;
            virtual	~ABoneChannel();

        protected:


            // +---------
            // | protected members
            // +---------

            // Position, rotation and scale of the bone at the current time
            AMMatrix							m_CurrentPRS;


        };

        //  +-------------
        //  | ABoneChannel methods      ------------------------------------------------------------------------------
        //  +-------------


        const AMMatrix& ABoneChannel::GetCurrentPRSMatrix()
        {
            return m_CurrentPRS;
        }

    }
}


