#pragma once

#include "DisplayDeviceCaps.h"

namespace Kigs
{
    namespace Gui
    {
        // ****************************************
        // * DisplayDeviceCapsAndroid class
        // * --------------------------------------
        /**
        * \file	DisplayDeviceCapsAndroid.h
        * \class	DisplayDeviceCapsAndroid
        * \ingroup GUIModule
        * \brief Specific DisplayDeviceCaps for Android platform.
        *
        */
        // ****************************************
        class DisplayDeviceCapsAndroid : public DisplayDeviceCaps
        {
        public:
            DECLARE_CLASS_INFO(DisplayDeviceCapsAndroid, DisplayDeviceCaps, GUI)

                //! constructor
                DisplayDeviceCapsAndroid(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

                //! destructor
                virtual ~DisplayDeviceCapsAndroid();
        protected:

        };

    }
}
