#pragma once

#include "ModuleBase.h"


namespace Kigs
{
    namespace Gui
    {
        using namespace Kigs::Core;
        class ModuleGUIAndroid;

        ModuleBase* PlatformGUIModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);


        // ****************************************
        // * ModuleGUIAndroid class
        // * --------------------------------------
        /**
        * \file	ModuleGUIAndroid.h
        * \class	ModuleGUIAndroid
        * \ingroup GUIModule
        * \ingroup Module
        * \brief Specific module for GUI classes ( Windows management )
        *
        */
        // ****************************************
        class ModuleGUIAndroid : public ModuleBase
        {
        public:

            DECLARE_CLASS_INFO(ModuleGUIAndroid, ModuleBase, GUI)

                ModuleGUIAndroid(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

            void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
            void Close() override;
            void Update(const Time::Timer& timer, void* /*addParam*/) override;
            virtual ~ModuleGUIAndroid();

        protected:

        };

    }
}