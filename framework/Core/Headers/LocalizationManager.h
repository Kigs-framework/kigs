#ifndef __LOCALIZATIONMANAGER_H__
#define __LOCALIZATIONMANAGER_H__

#include "Core.h"
#include "CoreModifiable.h"

#define  PLATFORM_WCHAR unsigned short

enum GAMELANGUAGE
{
	ENGLISH = 0,
	FRENCH,
	DEUTCH,
	CATALAN,
	CROATE,
};


class LocalizationManager : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(LocalizationManager,CoreModifiable,LocalizationManager);
	
	LocalizationManager(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void	setLocalizationFilePath(const char* path,bool a_erasePreviousLoc=false);

	virtual const PLATFORM_WCHAR*	getLocalizedString(const kstl::string& key) const;

	void			addLocalizationFromFileAtPath(const char* path);
	void			addLocalizationFromBuffer(char* _buffer, unsigned int bufferSize);
	static kstl::string		getCurrentUserLanguage();
	inline kstl::string		getLocalizationFilePath(){ return myLocalizationFilePath; }

	void			InitWithConfigFile(const kstl::string& filename);

	kstl::map<const kstl::string, PLATFORM_WCHAR* > getDebugFullMap()
	{
		return m_LocalizedString;
	}

protected:
	virtual				~LocalizationManager();
	void				EraseMap();

	bool				ParseStringsFile(const char*);
	PLATFORM_WCHAR*		GetNextLocalizedString(char* pBuffer,unsigned long& currentpos,kstl::string& key,unsigned long filelen);
	bool				ExtractQuoted(PLATFORM_WCHAR*& startchar,kstl::string& quoted,unsigned long& currentpos,unsigned long filelen,bool isKey);
	bool				ExtractQuoted(PLATFORM_WCHAR*& startchar,unsigned short* buffer,unsigned long& currentpos,unsigned long filelen,bool isKey);
	void				ExtractComment(PLATFORM_WCHAR*& startchar,unsigned long& currentpos,unsigned long filelen);
	void				GotoNextLine(PLATFORM_WCHAR*& startchar,unsigned long& currentpos,unsigned long filelen);
	void				GotoCommentEnd(PLATFORM_WCHAR*& startchar,unsigned long& currentpos,unsigned long filelen);
	void				ParseBuffer(char* _pBuffer, unsigned long size);
	kstl::string										myLocalizationFilePath;

	kstl::map<const kstl::string,PLATFORM_WCHAR* >		m_LocalizedString;
	unsigned short										m_wbuffer[4096];
};


#endif //__LOCALIZATIONMANAGER_H__
