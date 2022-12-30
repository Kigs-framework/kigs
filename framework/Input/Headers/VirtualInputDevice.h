#pragma once

#include "CoreModifiable.h"

namespace Kigs
{
    namespace Input
    {

        class VirtualInputDevice : public CoreModifiable
        {
        public:

            DECLARE_ABSTRACT_CLASS_INFO(VirtualInputDevice, CoreModifiable, Input)

                VirtualInputDevice(const std::string& name);

            void	update(double time);

        protected:
            virtual ~VirtualInputDevice();
        };

    }
}
