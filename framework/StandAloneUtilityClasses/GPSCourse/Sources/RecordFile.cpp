#include "GPSCourse/Headers/RecordFile.h"
#include "CoreModifiable.h"
#include "CoreItem.h"
#include "CoreMap.h"
#include "JSonFileParser.h"
#include "FilePathManager.h"

// #RecordFileWriter
RecordFileWriter::RecordFileWriter()
	: mFD(NULL)
	, mCurrentFrame(0)
	, mBuffer(NULL)
{
}
RecordFileWriter::~RecordFileWriter()
{
	if (mFD != NULL)
		delete mFD;
	 
	if (mBuffer)
		delete mBuffer;
}

void RecordFileWriter::InitWithDescriptor(FileDescriptor * aFD)
{
	mFD = aFD;
	mBuffer = new AbstractDynamicGrowingBuffer(128, aFD->GetFrameSize());
	mCurrentFrame = 0;
}

void RecordFileWriter::Flush(const char * aFileName)
{
	// open the file to write in
	SmartPointer<FileHandle> file = Platform_FindFullName(aFileName);
	Platform_fopen(file.get(), "wb");

	kstl::string desc = mFD->Serialize();
	int lDescSize = desc.length();
	int lHeaderSize = lDescSize + sizeof(RecordHeader) - sizeof(void*);
	int lDataSize = mFD->GetFrameSize()*mCurrentFrame;
	int lBufferSize = lHeaderSize + lDataSize;

	// allocate buffer
	unsigned char * buff = reinterpret_cast<unsigned char *>(malloc(lBufferSize));

	// fill header
	RecordHeader * rh = reinterpret_cast<RecordHeader*>(buff);
	rh->FrameCount = mCurrentFrame;
	rh->FrameSize = mFD->GetFrameSize();
	rh->HeaderSize = lHeaderSize;

	// copy json desc
	memcpy(&rh->Descriptor, desc.c_str(), lDescSize);
	// copy data
	mBuffer->getArray(buff + lHeaderSize);


	Platform_fwrite(buff, lBufferSize, 1, file.get());
	Platform_fclose(file.get());

	free(buff);
}

// #RecordFileReader
RecordFileReader::RecordFileReader()
	: mCurrentFrame(0)
	, mTotalFrame(0)
	, mFrameSize(0)
	, mFrames(NULL)
	, mFD(NULL)
{

}

RecordFileReader::~RecordFileReader()
{
	if (mFD)
		delete mFD;

	if (mFrames)
		free(mFrames);
}

void RecordFileReader::InitWithFile(const char * aPath)
{

	auto& pathManager = KigsCore::Singleton<FilePathManager>();
	SmartPointer<FileHandle> lFile = pathManager->FindFullName(aPath);
	Platform_fopen(lFile.get(), "rb");

	// open the record file
	Platform_fseek(lFile.get(), 0, SEEK_END);
	int lFileSize = Platform_ftell(lFile.get());
	Platform_fseek(lFile.get(), 0, SEEK_SET);

	void * data = malloc(lFileSize);
	Platform_fread(data, lFileSize, 1, lFile.get());
	Platform_fclose(lFile.get());

	InitWithRawData(data, lFileSize);

	free(data);
}

void RecordFileReader::InitWithRawData(void * aData, unsigned  int size)
{
	// get the header info
	RecordHeader * rh = (RecordHeader*)aData;

	mTotalFrame = rh->FrameCount;
	mFrameSize = rh->FrameSize;

	// get the desc size
	unsigned int lDescSize = rh->HeaderSize - sizeof(RecordHeader) + sizeof(void*);
	char * lDesc = new char[lDescSize + 1];
	lDesc[lDescSize] = 0;

	memccpy(lDesc, &rh->Descriptor, 1, lDescSize);

	mFD = new FileDescriptor();
	mFD->Unserialize(lDesc);


	void * frame;
	frame = static_cast<char*>(aData)+rh->HeaderSize;

	mFrames = malloc(mTotalFrame*mFrameSize);
	memcpy(mFrames, frame, mTotalFrame*mFrameSize);

	delete[] lDesc;
}

// #FrameDecriptor
FileDescriptor::FileDescriptor()
	: mFrameSize(0)
{
	mRoot = CoreItemSP::getCoreMap();
}


FileDescriptor::~FileDescriptor()
{

	kstl::map<kstl::string, RecordFieldBase*>::iterator itr = mFields.begin();
	kstl::map<kstl::string, RecordFieldBase*>::iterator end = mFields.end();
	for (; itr != end; ++itr)
	{
		delete itr->second;
	}
}

kstl::string FileDescriptor::Serialize()
{
	CoreItemSP	roottmp = CoreItemSP::getCoreMap();
	mRoot->set("Fields", roottmp);
	
	kstl::map<kstl::string, RecordFieldBase*>::iterator itr = mFields.begin();
	kstl::map<kstl::string, RecordFieldBase*>::iterator end = mFields.end();
	for (; itr != end; ++itr)
	{
		CoreItemSP tmp = CoreItemSP::getCoreMap();
		roottmp->set(itr->first, tmp);
		
		tmp->set("Offset", CoreItemSP::getCoreValue((int)itr->second->GetOffset()));
		tmp->set("Type", CoreItemSP::getCoreValue((int)itr->second->GetType()));
		tmp->set("Count", CoreItemSP::getCoreValue((int)itr->second->GetCount()));
	}

	kstl::string out;
	JSonFileParser jfp;
	jfp.ExportToString((CoreMap<kstl::string>*)(mRoot.get()), out);
	return out;
}

void FileDescriptor::Unserialize(const char * data)
{
	JSonFileParser jsp;

	mRoot = jsp.Get_JsonDictionaryFromString(data);

	CoreItemSP lFields = mRoot["Fields"];

	CoreItemIterator itr = lFields.begin();
	CoreItemIterator end = lFields.end();
	for (; itr != end; ++itr)
	{
		CoreItemSP lVals = *itr;

		int lType = lVals["Type"];
		int lOffset = lVals["Offset"];
		int lCount = lVals["Count"];
		
		kstl::string key;
		itr.getKey(key);

		switch (lType)
		{
		case Type::CHAR:
			mFields[key] = new RecordField<char>(lOffset, lCount);
			break;
		case Type::SHORT:
			mFields[key] = new RecordField<short>(lOffset, lCount);
			break;
		case Type::INT:
			mFields[key] = new RecordField<int>(lOffset, lCount);
			break;
		case Type::UCHAR:
			mFields[key] = new RecordField<unsigned char>(lOffset, lCount);
			break;
		case Type::USHORT:
			mFields[key] = new RecordField<unsigned short>(lOffset, lCount);
			break;
		case Type::UINT:
			mFields[key] = new RecordField<unsigned int>(lOffset, lCount);
			break;
		case Type::FLOAT:
			mFields[key] = new RecordField<float>(lOffset, lCount);
			break;
		case Type::DOUBLE:
			mFields[key] = new RecordField<double>(lOffset, lCount);
			break;
		default:
			printf("Error on field type ! FIELD : %s (o:%d t:%d c:%d)\n", key.c_str(), lOffset, lType, lCount);
			break;
		}


		printf("FIELD : %s (o:%d t:%d c:%d)\n", key.c_str(), lOffset, lType, lCount);

	}
}
