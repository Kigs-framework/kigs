extern "C" void JSGetLanguage(char* _var);

#include <stdlib.h>
#include <stdarg.h>

namespace Kigs
{
	namespace Core
	{

		std::string		LocalizationManager::getCurrentUserLanguage()
		{
			char* language = (char*)malloc(3*sizeof(char));
			
			JSGetLanguage(language);
			language[2]=0;

			
			// current language in 2 letter format
			std::string ret=language;
			

			return ret;
		}
	}
}