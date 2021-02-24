#ifndef _RecordFile_
#define _RecordFile_

#include "kstlmap.h"
#include "kstlstring.h"
#include "CoreValue.h"
#include "CoreMap.h"
#include "DynamicGrowingBuffer.h"

// NEVER CHANGE THE ORDER
// always add new value after the last one
struct Type
{
	enum ENUM
	{
		UNKNOW = 0,
		CHAR,
		UCHAR,
		SHORT,
		USHORT,
		INT,
		UINT,
		FLOAT,
		DOUBLE
	};
};

template<class c> class CoreMap;

// Header struct of the **recordFile**
struct RecordHeader
{
	unsigned int HeaderSize; // header size
	unsigned int FrameSize;  // frame size
	unsigned int FrameCount; // frame count
	void * Descriptor;		 // data descriptor in json format
};

/*
**RecordFieldBase** is the generic, untemplated and abstract class of **recorfField**.
It defines one value in the data frame.
*/
class RecordFieldBase
{
public:
	RecordFieldBase(unsigned short aOffset, unsigned int aCount)
		: mOffset(aOffset)
		, mCount(aCount) {}
	virtual ~RecordFieldBase() {}

	// return the type of the field
	// see enum _Type_ in class **FileDescriptor**
	// virtual pure, defined in **RecordField**
	virtual int GetType() = 0;

	// return the position of the value in the data frame
	inline int GetOffset() { return mOffset; }

	// return the number of element
	virtual  unsigned int GetCount() { return mCount; }

protected:
	// position of the value in the data frame
	int mOffset;

	// number of element
	unsigned int mCount;
};

/*
**RecordField** defines one value in the data frame.
*/
template<typename c>
class RecordField : public RecordFieldBase
{
public:
	RecordField(unsigned short aOffset, unsigned int aCount = 1) : RecordFieldBase(aOffset, aCount) {}

	// return the type of the field
	// see enum _Type_ in class **FileDescriptor**
	virtual int GetType() { return Type::UNKNOW; }
};


/*
	RecordDescriptor contains globals informations about data recorded.
	It will be write in the Record file header as a json data
	*/
class FileDescriptor
{
public:
	FileDescriptor();
	~FileDescriptor();

	/*
	Add a global information in the header descriptor
	*/
	template<typename c>
	void SetParameter(const char * aFieldName, c aValue)
	{
		CoreItemSP rcbc = CoreItemSP(new CoreValue<c>(aValue), StealRefTag{});
		mRoot->set(aFieldName, rcbc);
	}

	template<typename c>
	int GetParameter(const char * aFieldName, c& aValue)
	{
		CoreItemSP param = mRoot[aFieldName];

		if (param.isNil())
			return -1; // not found

		if (param->getValue(aValue))
			return 0;

		return -2; // bad type
	}

	/*
	Add a recorded field description in the header descriptor
	*/
	template<typename c>
	RecordField<c> * AddFieldDescriptor(const char * aFieldName, int elemCount = 1)
	{
		// can't add two fields with the same name
		assert(mFields[aFieldName] == NULL);

		RecordField<c> * add = new RecordField<c>(mFrameSize, elemCount);

		mFields[aFieldName] = add;
		mFrameSize += sizeof(c)*elemCount;

		return add;
	}

	/*
	Add a recorded field description in the header descriptor
	*/
	//template<typename c>
	int GetField(RecordFieldBase ** aRF, const char * aName)
	{
		kstl::map<kstl::string, RecordFieldBase*>::iterator found = mFields.find(aName);
		if (found != mFields.end())
		{
			//if (found->second->GetType() == (*aRF)->GetType())
			{
				*aRF = found->second;
				return 0;
			}

			//return -2;
		}
		return -1;
	}


	// convert to _json_ file
	kstl::string Serialize();

	// init from _json_ file
	void Unserialize(const char * data);

	// return the number of recorded frames
	inline int GetFrameSize() { return mFrameSize; }

private:
	// Size of a data frame in byte
	int mFrameSize;

	// root of the **CoreItem** tree used tu create the _json_ file
	CoreItemSP mRoot= CoreItemSP(nullptr);

	// list of the **RecordFieldBase** in a data frame
	kstl::map<kstl::string, RecordFieldBase*> mFields;
};


/*
**RecordFile** read/write data frame from/to a file
*/
class RecordFileWriter
{
private:
	// description of the file to record
	FileDescriptor * mFD;

	// buffer of recorded frame
	AbstractDynamicGrowingBuffer * mBuffer;

	int mCurrentFrame;

public:
	RecordFileWriter();
	~RecordFileWriter();

	void InitWithDescriptor(FileDescriptor * aFD);

	// record a value for a given field in the current data frame
	template<class c>
	void RecordValue(RecordField<c> * aField, c * aValue)
	{
		void * frame = (*mBuffer)[mCurrentFrame];
		frame = static_cast<char*>(frame) + aField->GetOffset();
		memcpy(frame, aValue, sizeof(c)*aField->GetCount());
	}

	// validate the current data frame and go to the next one
	void FlushFrame()
	{
		mCurrentFrame++;
	}

	// validate the current data frame and go to the next one
	void Flush(const char * aFileName);
};

/*
**RecordFile** read/write data frame from/to a file
*/
class RecordFileReader
{
private:
	// description of the file to record
	FileDescriptor * mFD;

	// buffer of recorded frame
	void * mFrames;

	int mTotalFrame;
	int mCurrentFrame;

	int mFrameSize;

public:
	RecordFileReader();
	~RecordFileReader();

	void InitWithFile(const char * aPath);
	void InitWithRawData(void * aData, unsigned int size);

	int  GetFrameCount() { return mTotalFrame; }
	bool IsValid() { return mFD; }

	void SetFrame(int i) { mCurrentFrame = i; }
	int  GetFrame() { return mCurrentFrame; }

	// record a value for a given field in the current data frame
	template<class c>
	int GetFromFrame(RecordFieldBase * aField, c ** aValue)
	{
		// no field
		if (aField == NULL)
			return -1;

		// out of bound
		if (mCurrentFrame < 0 || mCurrentFrame >= mTotalFrame)
			return -2;

		char * addr = static_cast<char*>(mFrames);
		addr = addr + (mFrameSize*mCurrentFrame) + aField->GetOffset();

		*aValue = reinterpret_cast<c*>(addr);

		return 0;
	}

	//template<class c>
	int GetField(RecordFieldBase ** aRF, const char * aName)
	{
		if (mFD)
			return mFD->GetField(aRF, aName);

		return -1;
	}

	template<class c>
	int GetParameter(const char * aName, c& aValue)
	{
		return mFD->GetParameter<c>(aName, aValue);
	}
};


// template<c> RecordField<c>::GetType() specification for each used type
template<> inline
int RecordField<char>::GetType() { return Type::CHAR; }
template<> inline
int RecordField<short>::GetType() { return Type::SHORT; }
template<> inline
int RecordField<int>::GetType() { return Type::INT; }
template<> inline
int RecordField<float>::GetType() { return Type::FLOAT; }
template<> inline
int RecordField<double>::GetType() { return Type::DOUBLE; }
template<> inline
int RecordField<unsigned char>::GetType() { return Type::UCHAR; }
template<> inline
int RecordField<unsigned short>::GetType() { return Type::USHORT; }
template<> inline
int RecordField<unsigned int>::GetType() { return Type::UINT; }

#endif //_RecordFile_
