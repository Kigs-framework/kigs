#include "KigsPackager.h"

//Modules
#include "CoreIncludes.h"

#include "CorePackage.h"

#include "TimerIncludes.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"

#include <algorithm>

void	usage()
{
	printf("Usage : \n");
	printf("KigsPackager -i inputFolder -o outputPackage [-r packageRootName] \n\n");
	printf("-i inputFolder : full path of the folder to package \n");
	printf("-o outputPackage : name of ouput package file \n");
	printf("-r packageRootName : name of the root folder in package");
}

void	KigsPackager::InitExternClasses()
{


}
#pragma optimize("", off)
IMPLEMENT_CLASS_INFO(KigsPackager)

//-------------------------------------------------------------------------
//Constructor

KigsPackager::KigsPackager(const kstl::string& name, CLASS_NAME_TREE_ARG):
CoreBaseApplication(name, PASS_CLASS_NAME_TREE_ARG)
{

}

KigsPackager::~KigsPackager()
{
	
}

void	KigsPackager::RetreiveShortNameAndExt(const kstl::string& filename,kstl::string& shortname,kstl::string& fileext)
{
	int pos=static_cast<int>(filename.rfind("/"))+1;
	int pos1=static_cast<int>(filename.rfind("\\"))+1;

	if(pos1 > pos)
	{
		pos=pos1;
	}

	bool	filenameIsShortfilename=false;

	if(pos==0)
	{
		shortname=filename;
		filenameIsShortfilename=true;
	}
	else
	{
		shortname=filename.substr(static_cast<unsigned int>(pos),filename.length()-pos);
	}

	fileext="";
	pos=static_cast<int>(shortname.rfind("."))+1;
	if(pos)
	{
		fileext.append(shortname,static_cast<unsigned int>(pos),shortname.length()-pos);
		shortname=shortname.substr(0,pos-1);
	}

	// lower case fileext
	std::transform(fileext.begin(), fileext.end(), fileext.begin(), ::tolower);
}


//-------------------------------------------------------------------------
//ProtectedInit

void	KigsPackager::ProtectedInit()
{
	

	InitExternClasses();

	// retreive args
	kstl::vector<kstl::string>::iterator itArgs=mArgs.begin();
	// skip app name
	itArgs++;

	int totalArgsCount=mArgs.size()-1;

	if((totalArgsCount<3) && totalArgsCount != 1)
	{
		usage();
		exit(-1);
	}

	kstl::string fileNameIn="";
	kstl::string fileNameOut="";
	kstl::string packageRootName = "";

	for(;itArgs!=mArgs.end();itArgs++)
	{
		kstl::string& current=(*itArgs);

		if(current.at(0) == '-')
		{
			char argtype=current.at(1);
			switch(argtype)
			{
			case 'i':
				{
					// in already there
					if(fileNameIn != "")
					{

						usage();
						exit(-1);
					}
					itArgs++;
					fileNameIn=(*itArgs);
				}
				break;
			case 'o':
				{
					// out already there
					if(fileNameOut != "")
					{
						usage();
						exit(-1);
					}
					itArgs++;
					fileNameOut=(*itArgs);
				}
				break;

			case 'r':
			{
				// out already there
				if (packageRootName != "")
				{
					usage();
					exit(-1);
				}
				itArgs++;
				packageRootName = (*itArgs);
			}
			break;

			break;
			default:
				break;
			}
		}
	}

	if (totalArgsCount == 1)
	{
		fileNameIn = mArgs[1];
		fileNameOut = "Assets.kpkg";
	}

	// create a package
	{

		CorePackage* testpackage = CorePackage::CreateNewEmptyPackage();

		testpackage->AddFolder(fileNameIn, packageRootName);

		testpackage->Export(fileNameOut);

		delete testpackage;

	}
	
	ProtectedExternAskExit();
}

//-------------------------------------------------------------------------
//ProtectedUpdate

void	KigsPackager::ProtectedUpdate()
{
}

//-------------------------------------------------------------------------
//ProtectedClose

void	KigsPackager::ProtectedClose()
{

}

void	KigsPackager::ProtectedExternAskExit()
{
	mNeedExit = true;
}