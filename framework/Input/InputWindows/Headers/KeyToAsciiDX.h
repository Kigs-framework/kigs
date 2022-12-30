#pragma once

#include "KeyToAscii.h"

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * KeyToAsciiDX class
		// * --------------------------------------
		/**
		* \file	KeyToAsciiDX.h
		* \class	KeyToAsciiDX
		* \ingroup Input
		* \brief Specific DirectX KeyToAscii utility class
		*
		*/
		// ****************************************
		class KeyToAsciiDX : public KeyToAscii
		{
		public:
			DECLARE_CLASS_INFO(KeyToAsciiDX, KeyToAscii, Input)
				KeyToAsciiDX(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual int Convert(unsigned Code);
		};

	}
}
