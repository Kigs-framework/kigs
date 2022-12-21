// **********************************************************************
// * FILE  : AnimationResourceInfo.h
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE :  class giving info on the resource type, and the resource itself  
// *            This class is the common header of the animation files.
// * COMMENT : 
// *---------------------------------------------------------------------
// * DATES     : 24/05/2000
// **********************************************************************

#ifndef __ANIMATIONRESOURCEINFO_H__
#define __ANIMATIONRESOURCEINFO_H__

#include "AMDefines.h"


// the animation resource info file is organized like this :

// the header : AResourceFileHeader
//                                  - mID                      // 4 bytes 
//                                  - m_StreamCount             // 4 bytes 
//                                  - m_StreamTypeNameSize;		// 4 bytes 

// stream type name
//                                  - char[m_StreamTypeNameSize]		// m_StreamTypeNameSize bytes. !!!! Must be aligned to 4 bytes !!!!!

// then m_StreamCount (offset + size) on stream info
//                                  - Offset[0]                 // 4 bytes offset from start of resource to current stream info data
//									- Size[0]                   // 4 bytes size of current stream info data
//                                  - Offset[1]                 // 4 bytes offset from start of resource to current stream info data (should be
//																// Offset[n-1]+Size[n-1]
//                                       .              
//                                       .
//                                       .
//                                  - Offset[m_StreamCount-1]   // 4 bytes 
//                                  - Size[m_StreamCount-1]   // 4 bytes 


//  then for each stream a AStreamResourceInfo pointed by the previous offsets

//                                  - m_StreamGroupID           // 4 bytes
//                                  - the stream data           // ?   

//                                  - m_StreamGroupID           // 4 bytes
//                                  - the stream data           // ? 

//                                          .
//                                          .
//                                          .

//                                  - m_StreamGroupID           // 4 bytes
//                                  - the stream data           // ?    


// +---------
// | usefull classes
// +---------


// +---------
// | AResourceFileHeader definition
/*!  Animation resource format : header
     \ingroup Animation
*/
// +---------

class   AResourceFileHeader
{
	public:
	
    IntU32          mID;               // todo 
    IntU32          m_StreamCount;
	IntU32          m_StreamTypeNameSize;
};

// +---------
// | AStreamResourceInfo definition
/*!  Animation resource format : resource for one stream
     \ingroup Animation
*/
// +---------

class   AStreamResourceInfo
{
	public:
	
    IntU32 m_StreamGroupID;
    // the rest of the resource for this stream should be here
	void*	getData()
	{
		return ((IntU8*)this) + sizeof(IntU32);
	}
};

// +---------
// | AnimationResourceInfo definition
/*!  Animation resource format 
     \ingroup Animation
*/
// +---------

class   AnimationResourceInfo
{
	public:
    AResourceFileHeader         m_head;
	
    // Then the rest of the file :
	
    // *******************
    // * GetStreamCount
    // * -----------------
    /*! return the stream count in this resource
    */ 
    // *******************
	
    IntU32            GetStreamCount()
    {
        return m_head.m_StreamCount;
    };
	
    // *******************
    // * SetStreamResourceInfo
    // * -----------------
    /*! used to construct the resource 
    */ 
    // *******************
	
	// set infos (offset and size)
    void SetStreamResourceInfo(AStreamResourceInfo* start,IntU32 index, int streamresourceinfosize)
    {
		IntU8*   tmp = ((IntU8*)this) // start at this
		+ sizeof(AResourceFileHeader) // add header size
		+ m_head.m_StreamTypeNameSize // add type name (char [...])  size 
		+ index * sizeof(IntU32) * 2; // search offset / size slot
		
        IntU32* write=(IntU32*)tmp;
        IntU32  offset=IntU32((IntU8*)start-(IntU8*)this);
		
        *write=offset;
		
		++write;
		*write = streamresourceinfosize;
    };
	
	// set stream resource info at index (offset and size) if previous index is already set
	// and if header is set (used for building resource info)
	AStreamResourceInfo* SetStreamResourceInfo(IntU32 index, int streamresourceinfosize)
	{
		AStreamResourceInfo* result = 0;
		IntU8*   tmp = ((IntU8*)this) // start at this
		+ sizeof(AResourceFileHeader) // add header size
		+ m_head.m_StreamTypeNameSize; // add type name (char [...])  size 
		
		if (index == 0)
		{
			IntU32* write = (IntU32*)tmp;
			IntU32  offset = sizeof(AResourceFileHeader) + m_head.m_StreamTypeNameSize + m_head.m_StreamCount * (sizeof(IntU32)*2);
			write[0] = offset;
			write[1] = streamresourceinfosize;
			
			result =(AStreamResourceInfo*) (((IntU8*)this) + offset);
		}
		else
		{
			tmp += (index - 1) * sizeof(IntU32) * 2; // search previous offset / size slot
			IntU32* read = (IntU32*)tmp;
			tmp += sizeof(IntU32) * 2;
			IntU32* write = (IntU32*)tmp;
			write[0] = read[0]+read[1];
			write[1] = streamresourceinfosize;
			
			result = (AStreamResourceInfo*)(((IntU8*)this) + write[0]);
		}
		return result;
	};
	
    // *******************
    // * GetStreamResourceInfo
    // * -----------------
    /*! return a pointer on the AStreamResourceInfo for the given index
    */ 
    // *******************
	
    AStreamResourceInfo* GetStreamResourceInfo(IntU32 index)
    {
		IntU8*   tmp = ((IntU8*)this) // start at this
		+ sizeof(AResourceFileHeader) // add header size
		+ m_head.m_StreamTypeNameSize // add type name (char [...])  size 
		+ index * sizeof(IntU32) * 2; // search offset / size slot
		
		IntU32* read = (IntU32*)tmp;
		IntU32  offset = *read;
		
        tmp=((IntU8*)this)+ offset;
        return ((AStreamResourceInfo*)tmp);
    };
	
    // *******************
    // * GetStreamGroupID
    // * -----------------
    /*! return the group id of the stream with the given index
    */ 
    // *******************
	
    IntU32            GetStreamGroupID(IntU32 index)
    {
        AStreamResourceInfo*    tmp=GetStreamResourceInfo(index);
        return (tmp->m_StreamGroupID);
    };
	
	std::string	getStreamType()
	{
		std::string result { ((char*)this) + sizeof(AResourceFileHeader) };
		return result;
	}
	
	void setStreamType(const std::string& strtype)
	{
		//m_head.m_StreamTypeNameSize = strtype.length()+1;
		IntU8*   tmp = ((IntU8*)this) // start at this
		+ sizeof(AResourceFileHeader); // add header size;
		KIGS_ASSERT(strtype.length()+1 <= m_head.m_StreamTypeNameSize);
		memcpy(tmp, strtype.c_str(), strtype.length()+1);
	}
};



#endif //__ANIMATIONRESOURCEINFO_H__


