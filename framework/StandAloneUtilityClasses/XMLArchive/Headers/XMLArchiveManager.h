#pragma once
#include <vector>
#include <string>
#include "XML.h"
#include "CoreRawBuffer.h"

// base class for file or folder in the archive
class XMLArchiveHierarchy
{
protected:
	std::string				mFileName;
	bool					mIsFolder=false;
	XMLArchiveHierarchy*	mParent = nullptr;
public:
	XMLArchiveHierarchy(const std::string_view& n, XMLArchiveHierarchy* p) : mFileName(n), mParent(p){};
	virtual~XMLArchiveHierarchy() {};

	// get file name with or without fullpath
	std::string getName(bool fullpath=false)
	{
		std::string result = mFileName;
		if (fullpath)
		{
			XMLArchiveHierarchy* p = mParent;
			while (p)
			{
				if (p->getName() != "")
				{
					result = p->getName() + "/" + result;
				}
				p = p->mParent;
			}
		}
		return result;
	}

	// get file path only
	std::string getPath()
	{
		std::string result = "";
		XMLArchiveHierarchy* p = mParent;
		while (p)
		{
			if (p->getName() != "")
			{
				std::string newresult = p->getName();
				if (result != "")
				{
					newresult+= "/" + result;
				}
				result = newresult;
			}
			p = p->mParent;
		}
		
		return result;
	}

	// get file extension (without '.')
	std::string getExtension()
	{
		size_t foundext = mFileName.rfind(".");
		if (foundext != std::string::npos)
		{
			return mFileName.substr(foundext + 1);
		}
		return "";
	}

	// return true for folders
	bool	isFolder()
	{
		return mIsFolder;
	}
};

// class managing folder in the archive
class XMLArchiveFolder : public XMLArchiveHierarchy
{
protected:
	std::vector< XMLArchiveHierarchy* > mSons; 
public:
	XMLArchiveFolder(const std::string_view& n, XMLArchiveFolder*p) : XMLArchiveHierarchy(n,p) {
		mIsFolder = true;
		if(p)
			p->addSon(this);
	}
	virtual ~XMLArchiveFolder()
	{
		clear();
	}

	// return son folder (not recursive)
	XMLArchiveFolder* findFolder(const std::string& n)
	{
		for (auto& f : mSons)
		{
			if ((f->getName() == n) && f->isFolder())
			{
				return static_cast<XMLArchiveFolder * >(f);
			}
		}
		return nullptr;
	}

	// add son (file or folder) to this folder
	void	addSon(XMLArchiveHierarchy* s)
	{
		// check if not already there
		for (auto& f : mSons)
		{
			if ((f->getName() == s->getName()) && (f->isFolder() == s->isFolder()))
			{
				if (s != f)
				{
					KIGS_ERROR("XMLArchiveFolder trying to add file or folder with same name ", 2);
				}
				return;
			}
		}
		mSons.push_back(s);
	}

	// clear all sons
	void clear()
	{
		for (auto& f : mSons)
		{
			delete f;
		}
		mSons.clear();
	}

	// get file given filename with relative path
	XMLArchiveHierarchy* getFile(const std::string& n);

	// return son file & folder count 
	u32 getFileCount()
	{
		return mSons.size();
	}

	// get file or folder at given index
	XMLArchiveHierarchy* getFile(u32 n)
	{
		if(n< mSons.size())
			return mSons[n];
		return nullptr;
	}

};

// class managing a file in the archive
class XMLArchiveFile : public XMLArchiveHierarchy
{
protected:
	
	// only one of this pointer should be available
	// as the file is an XML file or another data file
	XMLBase*			mXMLData = nullptr;
	SP<CoreRawBuffer>		mRawData = nullptr;

public:
	XMLArchiveFile(const std::string& n, XMLArchiveFolder* p) : XMLArchiveHierarchy(n, p) {
		p->addSon(this);
	};
	XMLArchiveFile(const std::string& n, SP<CoreRawBuffer> buf, XMLArchiveFolder* p) : XMLArchiveHierarchy(n,p), mRawData(buf){
		p->addSon(this);
	};
	// init file with an XML string
	XMLArchiveFile(const std::string& n, const std::string& xmlstring, XMLArchiveFolder* p) : XMLArchiveHierarchy(n, p)
	{
		mRawData = MakeRefCounted<CoreRawBuffer>();
		mRawData->SetBuffer(nullptr, xmlstring.length(), true);
		memcpy(mRawData->buffer(), xmlstring.c_str(), xmlstring.length());
		p->addSon(this);
		interpretAsXML();
	}

	virtual ~XMLArchiveFile()
	{
		if (mXMLData)
		{
			delete mXMLData;
			mXMLData = nullptr;
		}
	}

	// if mRawData is available, try to interpret content as XML and set mXMLData
	bool	interpretAsXML();

	// return mXMLData if available
	XMLBase* getXML()
	{
		return mXMLData;
	}

	// return mRawData if available
	CoreRawBuffer* getRawBuffer()
	{
		return mRawData.get();
	}

};

// manage an xml archive ( Office Open XML, BCF ... )
class XMLArchiveManager
{
protected:
	// root folder
	XMLArchiveFolder	mRoot = { "",nullptr };

	// update folder hierarchy and return folder where the file should be
	XMLArchiveFolder*	updateFolderHierarchy(const std::string& path);

	// return filename for a given path+filename
	std::string getFilename(const std::string& fullpath);


public:

	// iterator on file hierarchy
	class XMLArchiveIterator
	{
	protected:
		std::vector<std::pair<XMLArchiveFolder*, u32>>	mCurrentFolder;
		s32								mPos = 0;
	public:
		XMLArchiveIterator()
		{

		}
		XMLArchiveIterator(const XMLArchiveIterator& other)
		{
			mCurrentFolder = other.mCurrentFolder;
			mPos = other.mPos;
		}

		bool	operator ==(const XMLArchiveIterator& other)
		{
			if (mPos != other.mPos)
			{
				return false;
			}
			if (mCurrentFolder.size() != other.mCurrentFolder.size())
			{
				return false;
			}
			else
			{
				for (size_t i = 0; i < mCurrentFolder.size(); i++)
				{
					if (mCurrentFolder[i] != other.mCurrentFolder[i])
					{
						return false;
					}
				}
			}
			return true;
		}

		bool	operator !=(const XMLArchiveIterator& other)
		{
			return !(*this == other);
		}

		XMLArchiveHierarchy* operator *()
		{
			if (mPos >= 0)
			{
				if (mCurrentFolder.size())
				{
					if (mCurrentFolder.back().first->getFileCount() > mPos)
					{
						return mCurrentFolder.back().first->getFile(mPos);
					}
				}
			}
			return nullptr;
		}

		XMLArchiveIterator& operator++()
		{
			if (mCurrentFolder.size())
			{
				if (mCurrentFolder.back().first->getFile(mPos)->isFolder()) // folder, jump in
				{
					mCurrentFolder.push_back({ static_cast<XMLArchiveFolder*>(mCurrentFolder.back().first->getFile(mPos)),mPos });
					mPos = 0;
				}
				else
				{
					mPos++;
					while (mCurrentFolder.back().first->getFileCount() <= mPos) // jump out
					{
						mPos = mCurrentFolder.back().second;
						mPos++;
						if (mCurrentFolder.size() > 1)
						{
							mCurrentFolder.pop_back();
						}
						else
						{
							mPos = -1; // everything done
							break;
						}
					}
				}
			}
			return *this;
		}

		XMLArchiveIterator operator++(int)
		{
			XMLArchiveIterator	tmp(*this);
			operator++();
			return tmp;
		}

		friend class XMLArchiveManager;
	};


	XMLArchiveManager();
	virtual ~XMLArchiveManager();

	// return iterator on first file in hierarchy
	XMLArchiveIterator	begin()
	{
		XMLArchiveIterator result;
		result.mCurrentFolder.push_back({ &mRoot,0 });
		if (mRoot.getFileCount())
		{
			result.mPos = 0;
		}
		else
		{
			result.mPos = -1;
		}
		return result;
	}
	// end iterator
	XMLArchiveIterator	end()
	{
		XMLArchiveIterator result;
		result.mCurrentFolder.push_back({ &mRoot,0 });
		result.mPos = -1;
		return result;
	}

	// open xml archive and init 
	virtual bool open(const std::string& filename);
	virtual bool open(const SP<CoreRawBuffer>& buffer);

	// create a binary buffer from current XMLArchiveManager
	// so it can be saved directly as a XML Archive file
	virtual SP<CoreRawBuffer>	save();

	// clear everything
	void	clear();
};