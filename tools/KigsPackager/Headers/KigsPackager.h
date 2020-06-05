#ifndef _KIGS_PACKAGER_H_
#define _KIGS_PACKAGER_H_

#include "CoreBaseApplication.h"


class KigsPackager : public CoreBaseApplication
{
public:

	DECLARE_CLASS_INFO(KigsPackager, CoreBaseApplication, Core)

	KigsPackager(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual		~KigsPackager();

protected:

	void	RetreiveShortNameAndExt(const kstl::string& filename,kstl::string& shortname,kstl::string& fileext);

	void	InitExternClasses();


	//Virtual methods
	virtual void									ProtectedInit();
	virtual void									ProtectedUpdate();
	virtual void									ProtectedClose();
	virtual void									ProtectedExternAskExit();
	float											myStartTime;

};


#endif //_KIGS_PACKAGER_H_