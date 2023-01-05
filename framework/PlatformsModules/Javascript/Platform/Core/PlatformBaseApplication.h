#pragma once

//! This class must not be virtual because there's a double inheritance
// nothing to do on javascript ?

namespace Kigs
{
	namespace Core
	{
		static const char* platformName = "JS";

		class PlatformBaseApplication
		{
		public:
			//! constructor
			PlatformBaseApplication(){;}

			//! destructor
				//! no virtual here
			~PlatformBaseApplication(){;}
			
			bool	IsHolographic() {return false;} // no holographic mode
			
			
			void	Init(){;}
			void	Update(){;}
			void	Close(){;}
			void	Sleep(){;}
			void	Resume(){;}
			void	Message(int /* mtype */,int /* Params */){;}

			static const char* getPlatformName()
			{
				return platformName;
			}
			
			// check esc key state
			bool	CheckBackKeyPressed();
			
			// get number of core / processor
			unsigned int getProcessorCount();
		};

	}
}