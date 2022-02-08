#pragma once
#include <vector>
#include <string>
#include <map>
#include "XLSXSheet.h"
#include "XMLArchiveManager.h"
#include "XLSXElementRef.h"

class XLSXContentType
{
protected:
	std::map<std::string, std::string>	mExtensions;
	std::map< std::string, std::string>  mOverride;

public:
	XLSXContentType() {};
	~XLSXContentType() {};

	void	initFromXML(XMLBase* xml);
	void	updateXML(XMLBase* xml);

	std::string getOverride(const std::string& name);
	std::string getExtension(const std::string& name);

	std::string getContentType(const std::string name);


	std::string getPartName(const std::string contenttype);


	void	addOverride(const std::string& name, const std::string& contenttype);
	void	removeOverride(const std::string& name);
	void	addDefault(const std::string& extension, const std::string& contenttype);
	void	removeDefault(const std::string& extension);


	friend class XLSXDocument;
};

class XLSXSharedStrings
{
protected:
	std::vector<std::string>	mSharedStrings;
	std::map<std::string, u32>	mStringIndex;
	u32							mCountAccess = 0;
	friend class XLSXDocument;
	friend class XLSXSheet;

	u32			getIndex(const std::string& s)
	{
		mCountAccess++;
		u32 result = 0;
		auto f = mStringIndex.find(s);
		if (f == mStringIndex.end())
		{
			mSharedStrings.push_back(s);
			result = (u32)mSharedStrings.size() - 1;
			mStringIndex[s] = result;
		}
		else
		{
			result = (*f).second;
		}
		return result;
	}

	void reset()
	{
		mSharedStrings.clear();
		mStringIndex.clear();
		mCountAccess = 0;
	}
	void updateXML(XMLBase* toUpdate);

public:
	XLSXSharedStrings() {};
	~XLSXSharedStrings() {};

	void initFromXML(XMLBase* xml);

	std::string getString(u32 i)
	{
		return mSharedStrings[i];
	}
};

class XLSXRelationships
{
protected:

	class RelationStruct
	{
	public:
		RelationStruct()
		{

		}
		RelationStruct(const std::string& t, const std::string& ta) : mType(t), mTarget(ta) {};
		std::string mType;
		std::string mTarget;
	};

	std::vector<RelationStruct>	mRelations;

	u32	getID(const std::string& id)
	{
		std::string cut = id;
		if (id.substr(0, 3) == "rId")
		{
			cut = id.substr(3);
		}
		return atoi(cut.c_str());
	}

public:
	XLSXRelationships() {};
	~XLSXRelationships() {};

	std::string	getTarget(const std::string& id)
	{
		u32 uid = getID(id);
		if (uid <= mRelations.size())
		{
			return mRelations[uid-1].mTarget;
		}
	
		return "";
	}

	std::string	getTargetFromType(const std::string& t)
	{
		for(auto& f : mRelations)
		{
			auto found = f.mType.find(t);
			if (found != std::string::npos)
			{
				return f.mTarget;
			}
		}
		return "";
	}

	bool	hasRelation(const std::string& target)
	{
		for (auto& f : mRelations)
		{
			if (f.mTarget == target)
			{
				return true;
			}
		}
		return false;
	}

	void addRelation(u32 id, const std::string& type, const std::string& target)
	{
		if (hasRelation(target))
		{
			return;
		}
		if (id > mRelations.size())
		{
			mRelations.resize(id);
		}
		mRelations[id-1] = { type ,target };
	}

	void addRelation(const std::string& type, const std::string& target)
	{
		if (hasRelation(target))
		{
			return;
		}
		mRelations.push_back({ type ,target });
	}

	void removeRelation(u32 id)
	{
		if (id <= mRelations.size())
		{
			mRelations.erase(mRelations.begin()+id-1);
		}
	}

	void removeRelationFromType(const std::string& t)
	{
		std::vector<RelationStruct>::iterator f;
		for (f= mRelations.begin();f!= mRelations.end();++f)
		{
			auto found = (*f).mType.find(t);
			if (found != std::string::npos)
			{
				mRelations.erase(f);
				break;
			}
		}
	}

	void initFromXML(XMLBase* xml);

	void updateXML(XMLBase* xml);

	friend class XLSXDocument;
};

class XLSXDocument : public XMLArchiveManager
{
protected:
	std::vector<XLSXSheet*>						mSheets;
	XLSXContentType								mContentType;
	std::map<std::string, XLSXRelationships>	mRels;
	XLSXSharedStrings							mSharedStrings;
	XMLBase*									mSharedStringsXML=nullptr;

	void		initWorkbook(const std::string& name);
	void		initSharedStrings(const std::string& name);
	XLSXSheet*	initSheet(const std::string& file, std::string name, int id);

	// create folder hierarchy

	void		createFolderHierarchy();

	bool initAfterOpen();

public:

	XLSXDocument();
	virtual ~XLSXDocument();

	// return ref on sheet
	XLSXElementRef operator[](u32 i)
	{
		if (mSheets.size() > i)
		{
			return XLSXElementRef(mSheets[i]);
		}
		return XLSXElementRef();
	}

	XLSXElementRef operator[](const std::string n)
	{
		for(auto s: mSheets)
		{
			if(s->getName() == n)
				return XLSXElementRef(s);
		}
		return XLSXElementRef();
	}

	const std::vector<XLSXSheet*>& GetSheets() { return mSheets; }

	std::vector<XLSXElementRef>	find(const std::string& content,bool exactmatch=false);
	std::vector<XLSXElementRef>	find(int val);

	virtual bool open(const std::string& filename) override;
	virtual bool open(const SP<CoreRawBuffer>& buffer) override;

	// create empty document
	void	initEmpty();

	// add one sheet
	XLSXSheet* addSheet(std::string n = "");

	virtual SP<CoreRawBuffer> save() override;
};