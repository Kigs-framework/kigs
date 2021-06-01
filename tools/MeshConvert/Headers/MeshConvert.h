#ifndef _MESHCONVERT_H_
#define _MESHCONVERT_H_

#include "CoreBaseApplication.h"
// for MeshConvertParams
#include "ModernMesh.h"
#include "Base3DImporter.h"

class	ModuleSceneGraph;
class	ModuleRenderer;
class	ModuleGUI;


class MeshConvert : public CoreBaseApplication
{
public:

	DECLARE_CLASS_INFO(MeshConvert, CoreBaseApplication, Core)

	MeshConvert(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual		~MeshConvert();

protected:

	void	RetreiveShortNameAndExt(const kstl::string& filename,kstl::string& shortname,kstl::string& fileext);

	void	InitExternClasses();


	//Virtual methods
	virtual void									ProtectedInit();
	virtual void									ProtectedUpdate();
	virtual void									ProtectedClose();
	virtual bool									ProtectedExternAskExit();
	float											myStartTime;

	SP<ModuleSceneGraph>		theSceneGraph;
	SP<ModuleRenderer>			theRenderer;
	SP<ModuleGUI>				theGUI;

	MeshConvertParams		myParams;

	kstl::vector<CMSP>	myObjectsToDestroy;
};


#endif //_MESHCONVERT_H_