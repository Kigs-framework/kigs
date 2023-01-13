

#include <windows.h>

namespace Kigs
{
	namespace Core
	{
		std::string		LocalizationManager::getCurrentUserLanguage()
		{
			// current language in 2 letter format
			std::string ret = "en";


			LANGID LanguageId = /*GetUserDefaultUILanguage();//*/ LANG_ENGLISH;
			LanguageId &= 0xff;
			switch (LanguageId)
			{
			case LANG_ITALIAN:
			{
				ret = "it";
				break;
			}
			case LANG_FRENCH:
			{
				ret = "fr";
				break;
			}
			case LANG_ENGLISH:
			{
				ret = "en";
				break;
			}
			case LANG_GERMAN:
			{
				ret = "de";
				break;
			}
			case LANG_SPANISH:
			{
				ret = "sp";
				break;
			}
			case LANG_RUSSIAN:
			{
				ret = "ru";
				break;
			}
			case LANG_KOREAN:
			{
				ret = "kr";
				break;
			}
			case LANG_JAPANESE:
			{
				ret = "jp";
				break;
			}
			case LANG_CHINESE:
			{
				ret = "ch";
				break;
			}
			}

			return ret;
		}
	}
}