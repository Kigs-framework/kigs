#include "BundleList.h"
#include "ModuleThread.h"
#include "Thread.h"
#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <fstream>

void	BundleList::usage()
{
	std::cout << "Usage :" << std::endl;
	std::cout << "BundleList [-i inputPath] [ -o outputfile ]" << std::endl;
	std::cout << "-i inputPath : path of the directory to create bundle list. Default is \'.\'" << std::endl;
	std::cout << "-o outputFile : name of the bundlelist file to create. Default is \"files.bundle\"" << std::endl;
}


IMPLEMENT_CLASS_INFO(BundleList);

IMPLEMENT_CONSTRUCTOR(BundleList)
{

}

void	BundleList::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	CoreCreateModule(ModuleThread, nullptr);


	// retreive args
	kstl::vector<kstl::string>::iterator itArgs = mArgs.begin();
	// skip app name
	itArgs++;

	for (; itArgs != mArgs.end(); itArgs++)
	{
		kstl::string& current = (*itArgs);

		if (current.at(0) == '-')
		{
			char argtype = current.at(1);
			switch (argtype)
			{
			case 'i':
			{
				// in already there
				if (mStartPath != "./")
				{
					usage();
					continue; // exit
				}
				itArgs++;
				mStartPath = (*itArgs);
			}
			break;
			case 'o':
			{
				// out already there
				if (mFilename != "files.bundle")
				{
					usage();
					continue; // exit
				}
				itArgs++;
				mFilename = (*itArgs);
			}
			break;

			}
		}
	}

	// check for valid path
	char pathend = mStartPath[mStartPath.length() - 1];
	if((pathend != '\\') && ((pathend != '/')))
	{
		mStartPath += "/";
	}

	if (mFilename == "")
	{
		usage();
	}
	else
	{
		SP<Thread> bundlethread = KigsCore::GetInstanceOf("bundleThread", "Thread");
		mThread = bundlethread;
		bundlethread->setMethod(this, "CreateBundle");
		bundlethread->Init();
	}
}

void	BundleList::ProtectedUpdate()
{
	std::cout << "Processed file count : " << mBundle.size() << std::endl;
	if (mBundleIsDone)
	{
		mNeedExit = true;
		mThread = nullptr;
	}
}

void	BundleList::ProtectedClose()
{
	WriteBundleList();

	CoreDestroyModule(ModuleThread);

}

void	BundleList::CreateBundle()
{
	mBundle.clear();
	RecursiveInitFileList(mStartPath, mStartPath.length());
	mBundleIsDone = true;
}


void	BundleList::RecursiveInitFileList(std::string	startDirectory, int cropFilePath)
{

	HANDLE			hFind;

	// Iterate through dirs

	std::vector<std::string>	subDirectoryList;
	subDirectoryList.clear();


	WIN32_FIND_DATAA wfd;

	std::string search = startDirectory + "*";

	hFind = ::FindFirstFileA(search.c_str(), &wfd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{

			std::string asciifilename = wfd.cFileName;

			// FIRST check if its a dir
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{

				// only valid directory
				if (!(asciifilename[0] == '.'))
				{
					subDirectoryList.push_back(startDirectory + asciifilename + "/");
				}
			}
			else
			{
				if (!(asciifilename[0] == '.'))
				{
					std::string croppedPath = startDirectory.substr(cropFilePath, startDirectory.length() - cropFilePath);

					if (mBundle.find(asciifilename) != mBundle.end())
					{
						mBundle[asciifilename].push_back(croppedPath);
					}
					else
					{
						std::vector<std::string>	toAdd;
						toAdd.push_back(croppedPath);
						mBundle[asciifilename] = toAdd;
					}
				}

			}

		} while (::FindNextFileA(hFind, &wfd));

		::FindClose(hFind);
	}

	// recurse now
	unsigned int i;
	for (i = 0; i < subDirectoryList.size(); i++)
	{
		RecursiveInitFileList(subDirectoryList[i], cropFilePath);
	}

}

void BundleList::WriteBundleList()
{
	std::ofstream fichier(mFilename, std::ios::out | std::ios::trunc);

	if (fichier)
	{
		std::map<std::string, std::vector<std::string> >::const_iterator IT = mBundle.begin();
		while (IT != mBundle.end())
		{
			fichier << IT->first;

			const std::vector<std::string>& currentVector = IT->second;
			std::vector<std::string>::const_iterator ITVector = currentVector.begin();
			while (ITVector != currentVector.end())
			{
				fichier << ";" << *ITVector;
				++ITVector;
			}
			++IT;

			fichier << std::endl;
		}

		//Parse de la liste
		fichier.close();
	}
	else
	{
		std::cerr << "Can't open file " << mFilename << std::endl;
	}
}