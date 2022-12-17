#ifndef _CORETYPES_H_
#define _CORETYPES_H_

#include "CoreSTLAllocator.h"
#include "kstlstring.h"
#include "robin_hood.h"
#include <unordered_map>

#ifdef RIM
#include <stdio.h>
#include <string.h>
#endif

#ifdef _NINTENDO_3DS_
#include <stdio.h>
#include <string.h>
#endif


#include "Platform/Core/PlatformCore.h"


using s8 = int8_t;
using u8 = uint8_t;
using s16 = int16_t;
using u16 = uint16_t;
using s32 = int32_t;
using u32 = uint32_t;
using s64 = int64_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using uptr = uintptr_t;
using sptr = intptr_t;

namespace kigs
{
#if defined(_DEBUG) 
	template <typename Key, typename T, typename Hash = std::hash<Key>>
	using unordered_map = std::unordered_map<Key, T, Hash>;
#else
#if (defined(WIN32) || defined(WUP))
	// std::unordered_map seems more efficient on Windows platform
	template <typename Key, typename T, typename Hash = std::hash<Key>>
	using unordered_map = std::unordered_map<Key, T, Hash>;
#else
	template <typename Key, typename T, typename Hash = robin_hood::hash<Key>>
	using unordered_map = robin_hood::unordered_map<Key, T, Hash>;
#endif
#endif
}

#if defined(_DEBUG) || defined(KIGS_TOOLS)
	#ifndef KEEP_NAME_AS_STRING
		#define KEEP_NAME_AS_STRING
	#endif
#endif

///////////////////////////////////////////////////////////////////////
////EDITOR IS BUILD WITH KEEPNAME AS STRING
#ifdef WIN32
#if defined(_KIGS_ID_RELEASE_)
	#define KEEP_NAME_AS_STRING
#endif
#endif
///////////////////////////////////////////////////////////////////////

#define CC_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100)

#ifdef JAVASCRIPT

inline unsigned int readInt32(const unsigned int* i)
{
    unsigned char* a=(unsigned char*)i;
    unsigned int result = 0;
    if(CC_HOST_IS_BIG_ENDIAN == true)
    {
        result=((unsigned int)(*a))<<24;
        result|=((unsigned int)(*(a+1)))<<16;
        result|=((unsigned int)(*(a+2)))<<8;
        result|=((unsigned int)(*(a+3)));
    }
    else
    {
        result=((unsigned int)(*a));
        result|=((unsigned int)(*(a+1)))<<8;
        result|=((unsigned int)(*(a+2)))<<16;
        result|=((unsigned int)(*(a+3)))<<24;
    }

    return result;
}

#else
inline unsigned int readInt32(const unsigned int* i)
{
    return *i;
}

#endif

inline unsigned int rotl(unsigned int value, unsigned int shift) {
    return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}

// fast meta coded (at compile time) ID creation
template<int unsignedICount>
inline unsigned int    fastIDMem(const unsigned int* a)
{
	constexpr bool impair = unsignedICount & 1;

    int index=unsignedICount>>1;
    unsigned int ID=0;
    while(index--)
    {
        ID^=readInt32(--a);
        ID=rotl(ID,6);
		ID^= readInt32(--a);
		ID= rotl(ID, 6);
	}
	if (impair)
	{
		ID ^= readInt32(--a);
		ID = rotl(ID, 6);
	}

    return ID;
}

inline unsigned int    fastIDMem(const unsigned int* a,unsigned int unsignedICount)
{
	bool impair = unsignedICount & 1;
    int index=unsignedICount>>1;
    unsigned int ID=0;
    while(index--)
    {
		ID ^= readInt32(--a);
		ID = rotl(ID, 6);
		ID ^= readInt32(--a);
		ID = rotl(ID, 6);
	}
	if (impair)
	{
		ID ^= readInt32(--a);
		ID = rotl(ID, 6);
	}
    return ID;
}


template<>
inline unsigned int    fastIDMem<0>(const unsigned int* a)
{
    return 0;
}

template<>
inline unsigned int    fastIDMem<1>(const unsigned int* a)
{
    unsigned int ID=readInt32(--a);
    ID=rotl(ID,6);
    return ID;
}

template<>
inline unsigned int    fastIDMem<2>(const unsigned int* a)
{
    unsigned int ID=readInt32(--a);
    ID=rotl(ID,6);
    ID^=readInt32(--a);
    ID=rotl(ID,6);
    return ID;
}

template<>
inline unsigned int    fastIDMem<3>(const unsigned int* a)
{
    unsigned int ID=readInt32(--a);
    ID=rotl(ID,6);
    ID^=readInt32(--a);
    ID=rotl(ID,6);
    ID^=readInt32(--a);
    ID=rotl(ID,6);
    return ID;
}

template<>
inline unsigned int    fastIDMem<4>(const unsigned int* a)
{
	unsigned int ID = readInt32(--a);
	ID = rotl(ID, 6);
	ID ^= readInt32(--a);
	ID = rotl(ID, 6);
	ID ^= readInt32(--a);
	ID = rotl(ID, 6);
	ID ^= readInt32(--a);
	ID = rotl(ID, 6);
	return ID;
}

template<unsigned int mask>
inline unsigned int    fastIDMask(const unsigned char* a)
{
	printf("can not be in not specialized func here\n");
	return 0;
}

template<>
inline unsigned int    fastIDMask<0>(const unsigned char* a)
{
	return 0;
}

template<>
inline unsigned int    fastIDMask<1>(const unsigned char* a)
{
	return (unsigned int)(a[0]);
}

template<>
inline unsigned int    fastIDMask<2>(const unsigned char* a)
{
	unsigned int a1 = (unsigned int)(a[1]);
	return (a1 << 8) | (unsigned int)(a[0]);
}

template<>
inline unsigned int    fastIDMask<3>(const unsigned char* a)
{
	unsigned int a2 = (unsigned int)(a[2]);
	unsigned int a1 = (unsigned int)(a[1]);
	return (a2 << 16) | (a1 << 8) | (unsigned int)(a[0]);
}

inline unsigned int    fastIDMask(const unsigned char* a,unsigned int mask)
{
    switch(mask)
    {
    case 1:
        return fastIDMask<1>(a);
        break;
    case 2:
        return fastIDMask<2>(a);
        break;
    case 3:
        return fastIDMask<3>(a);
        break;
    default:
        return fastIDMask<0>(a);
        break;
    }
}

template<unsigned int stringSize>
inline unsigned int    fastGetID(const char* a)
{
    const int indexMask=stringSize&3;
    a+=stringSize-indexMask;

    return fastIDMask<indexMask>((const unsigned char*)a) ^ fastIDMem<stringSize/4 >((const unsigned int*)a) ;
}

inline unsigned int    fastGetID(const char* a,unsigned int stringSize)
{
    int indexMask=stringSize&3;
    a+=stringSize-indexMask;

    return fastIDMask((const unsigned char*)a, indexMask) ^ fastIDMem((const unsigned int*)a,stringSize>>2);
}



#ifdef KIGS_TOOLS
#define KIGSID_CHECK_COLLISIONS 0
#endif

#if KIGSID_CHECK_COLLISIONS
#include <set>
inline std::set<std::string>& GetKigsIDCollisionStrings()
{
	static std::set<std::string> value;
	return value;
}
#define KIGSID_ADD_NAME GetKigsIDCollisionStrings().insert(a);
#else
#define KIGSID_ADD_NAME
#endif

// ****************************************
// * CharToID class
// * --------------------------------------
/**
 * \file	CoreType.h
 * \class	CharToID
 * \ingroup Core
 * \brief	CharToID utility class, convert a string to an int ID  
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * We use the size of the character array (including the terminating 0) as a template argument
 * The goal is to have a loop with a repeating count which is easily computable by the optimizer.
 */
// ****************************************
class CharToID
{
public:
	/**
	 * \fn		template<size_t _Size> static inline unsigned int GetID(const char (&a)[_Size])
	 * \brief	only one static method to convert the given string to an unsigned int, used as an ID 
	 * \param	a : char array to convert to an Id
	 * \return	the unique Id of the char array
	 */

	template<size_t _Size> static inline unsigned int GetID(const char (&a)[_Size])
	{ 
		KIGSID_ADD_NAME;
		return fastGetID<_Size-1>(a); // remove useless 0 
	}
	

	/**
	 * \fn		static inline unsigned int GetID(const std::string& a)
	 * \brief	only one static method to convert the given string to an unsigned int, used as an ID 
	 * \param	a : string to convert to an Id
	 * \return	the unique Id of the string
	 */
	static inline unsigned int GetID(const std::string& a)
	{ 
		KIGSID_ADD_NAME;
		return fastGetID(a.c_str(),(unsigned int)a.length()); 
	}
	static inline unsigned int GetID(const std::string_view& a)
	{
		KIGSID_ADD_NAME;
		return fastGetID(a.data(), (unsigned int)a.length());
	}
};



struct KigsID
{
	KigsID() = default;
	KigsID(const KigsID& other) = default;
	KigsID(KigsID&& other) = default;

	KigsID& operator=(const KigsID& other) = default;
	KigsID& operator=(KigsID&& other) = default;

	bool operator==(const KigsID& other) const
	{
		return (_id == other._id);
	}

	bool operator<(const KigsID& other) const
	{
		return _id < other._id; 
	}
	unsigned int toUInt() const { return _id; }

	std::string toString() const
	{
#ifdef KEEP_NAME_AS_STRING
		return _id_name.size() ? _id_name : std::to_string(_id);
#else
		return std::to_string(_id);
#endif
	}


#ifdef KEEP_NAME_AS_STRING
	template<size_t _Size> KigsID(const char(&aid)[_Size]) : _id_name(aid), _id(CharToID::GetID(aid)) {
	};
	KigsID(const std::string& aid) : _id_name(aid), _id(CharToID::GetID(aid)) {
	};
	KigsID(const std::string_view& aid) : _id_name(aid), _id(CharToID::GetID(aid)) {
	};

	KigsID(unsigned int aid) : _id_name("*unknown*"), _id(aid) {
	};

	KigsID& operator=(const std::string& aid) { _id_name = aid; _id = CharToID::GetID(aid); return *this; };
	template<size_t _Size> KigsID& operator=(const char(&aid)[_Size]) { _id_name = aid; _id = CharToID::GetID(aid); return *this; };

	KigsID& operator=(unsigned int aid) { _id_name = "*unknown*"; _id = aid; return *this; };
	
	// Extra name
	// Dont set this field manually!
	std::string _id_name;

#else
	template<size_t _Size>
	KigsID(const char(&aid)[_Size]) : _id(CharToID::GetID<_Size>(aid)) {};
	KigsID(const std::string& aid) : _id(CharToID::GetID(aid)) {};
	KigsID(const std::string_view& aid) : _id(CharToID::GetID(aid)) {};
	//KigsID(const char*& aid) : mID(CharToID::GetID(aid)) {};
	KigsID(unsigned int aid) : _id(aid) {};

	template<size_t _Size>
	KigsID& operator=(const char(&aid)[_Size]) { _id = CharToID::GetID<_Size>(aid); return *this; };
	KigsID& operator=(const std::string& aid) { _id = CharToID::GetID(aid); return *this; };
	KigsID& operator=(const std::string_view& aid) { _id = CharToID::GetID(aid); return *this; };
	//KigsID& operator=(const char*& aid) { mID = CharToID::GetID(aid); return *this; };
	KigsID& operator=(unsigned int aid) { _id = aid; return *this; };

#endif

	// Dont set this field manually!
	unsigned int _id;
};

//inline bool operator==(const KigsID& a, const KigsID& b) { return a.mID == b.mID; }
inline bool operator==(const KigsID& a, unsigned int id) { return a._id == id; }
inline bool operator==(unsigned int id, const KigsID& a) { return a._id == id; }

inline bool operator!=(const KigsID& a, const KigsID& b) { return a._id != b._id; }
inline bool operator!=(const KigsID& a, unsigned int id) { return a._id != id; }
inline bool operator!=(unsigned int id, const KigsID& a) { return a._id != id; }


struct KigsIDHash
{
	std::size_t operator()(const KigsID& k) const
	{
		return k._id;
	}
};

// Hash for usage in maps
#include <functional>
namespace std
{
	template<> struct hash<KigsID>
	{
		typedef KigsID argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const
		{
			//return std::hash<decltype(s.mID)>{}(s.mID);
			return s._id;
		}
	};
}

#define CONCAT_TOKENS_(a, b) a##b
#define CONCAT_TOKENS(a, b) CONCAT_TOKENS_(a, b)
// Make a FOREACH macro
#define EXPAND_FE(x) x
#define FE_1(WHAT, X) EXPAND_FE(WHAT(X))
#define FE_2(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_1(WHAT, __VA_ARGS__))
#define FE_3(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_2(WHAT, __VA_ARGS__))
#define FE_4(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_3(WHAT, __VA_ARGS__))
#define FE_5(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_4(WHAT, __VA_ARGS__))
#define FE_6(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_5(WHAT, __VA_ARGS__))
#define FE_7(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_6(WHAT, __VA_ARGS__))
#define FE_8(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_7(WHAT, __VA_ARGS__))
#define FE_9(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_8(WHAT, __VA_ARGS__))
#define FE_10(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_9(WHAT, __VA_ARGS__))
#define FE_11(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_10(WHAT, __VA_ARGS__))
#define FE_12(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_11(WHAT, __VA_ARGS__))
#define FE_13(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_12(WHAT, __VA_ARGS__))
#define FE_14(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_13(WHAT, __VA_ARGS__))
#define FE_15(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_14(WHAT, __VA_ARGS__))
#define FE_16(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_15(WHAT, __VA_ARGS__))
#define FE_17(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_16(WHAT, __VA_ARGS__))
#define FE_18(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_17(WHAT, __VA_ARGS__))
#define FE_19(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_18(WHAT, __VA_ARGS__))
#define FE_20(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_19(WHAT, __VA_ARGS__))
#define FE_21(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_20(WHAT, __VA_ARGS__))
#define FE_22(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_21(WHAT, __VA_ARGS__))
#define FE_23(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_22(WHAT, __VA_ARGS__))
#define FE_24(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_23(WHAT, __VA_ARGS__))
#define FE_25(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_24(WHAT, __VA_ARGS__))
#define FE_26(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_25(WHAT, __VA_ARGS__))
#define FE_27(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_26(WHAT, __VA_ARGS__))
#define FE_28(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_27(WHAT, __VA_ARGS__))
#define FE_29(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_28(WHAT, __VA_ARGS__))
#define FE_30(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_29(WHAT, __VA_ARGS__))
#define FE_31(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_30(WHAT, __VA_ARGS__))
#define FE_32(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_31(WHAT, __VA_ARGS__))
#define FE_33(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_32(WHAT, __VA_ARGS__))
#define FE_34(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_33(WHAT, __VA_ARGS__))
#define FE_35(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_34(WHAT, __VA_ARGS__))
#define FE_36(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_35(WHAT, __VA_ARGS__))
#define FE_37(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_36(WHAT, __VA_ARGS__))
#define FE_38(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_37(WHAT, __VA_ARGS__))
#define FE_39(WHAT, X, ...) EXPAND_FE(WHAT(X)FE_38(WHAT, __VA_ARGS__))


//... repeat as needed
#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,NAME,...) NAME 
#define FOR_EACH(action,...) \
	EXPAND_FE(GET_MACRO(__VA_ARGS__,FE_39,FE_38,FE_37,FE_36,FE_35,FE_34,FE_33,FE_32,FE_31,FE_30,FE_29,FE_28,FE_27,FE_26,FE_25,FE_24,FE_23,FE_22,FE_21,FE_20,FE_19,FE_18,FE_17,FE_16,FE_15,FE_14,FE_13,FE_12,FE_11,FE_10,FE_9,FE_8,FE_7,FE_6,FE_5,FE_4,FE_3,FE_2,FE_1,)(action,__VA_ARGS__))


#ifdef KIGS_TOOLS
#define KIGS_TOOLS_ONLY(a) a
#else
#define KIGS_TOOLS_ONLY(a) 
#endif






#endif
