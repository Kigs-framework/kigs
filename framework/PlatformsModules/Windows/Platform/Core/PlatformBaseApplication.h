#pragma once

namespace Kigs
{
	namespace Input
	{
		class KeyboardDevice;
	}

	namespace Core
	{
		//! This class must not be virtual because there's a double inheritance

		class PlatformBaseApplication
		{
		public:
			//! constructor
			PlatformBaseApplication() { mKeyboard = 0; }

			//! destructor
			//! no virtual here
			~PlatformBaseApplication() { ; }


			void	Init() { ; }
			void	Update() { ; }
			void	Close() { ; }
			void	Sleep() { ; }
			void	Resume() { ; }
			void	Message(int /* mtype */, int /* Params */) { ; }
			void	OpenLink(const char* a_link);
			void	OpenLink(const unsigned short* a_link, unsigned int a_length);
			bool	CheckConnexion();

			// check esc key state
			bool	CheckBackKeyPressed();

			bool	IsHolographic() { return false; } // no holographic mode

			// get number of core / processor
			static unsigned int getProcessorCount();

			static int		getCpuId();

			static void		setCurrentThreadAffinityMask(int mask);
			static void		setThreadAffinityMask(void*, int mask);

			static const char* getPlatformName();

			// for back key check
			Input::KeyboardDevice* mKeyboard;
		};
	}
}