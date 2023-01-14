#pragma once

namespace Kigs
{
    namespace Core
    {

#define SIGNAL_ARRAY_CONTENT(a) #a, 
#define SIGNAL_ENUM_CONTENT(a) a, 
#define SIGNAL_PUSH_BACK(a) signals.push_back(#a);

#define SIGNAL_CASE(a) case Signals::a: return CoreModifiable::EmitSignal(#a, std::forward<T>(params)...); break;

#define SIGNALS(...)\
enum class Signals : u32\
{\
	FOR_EACH(SIGNAL_ENUM_CONTENT, __VA_ARGS__)\
};\
template<typename... T>\
inline bool EmitSignal(Signals signal,  T&&... params){\
switch(signal){\
FOR_EACH(SIGNAL_CASE, __VA_ARGS__)\
default: break;} return false;\
}\
virtual std::vector<KigsID> GetSignalList() override\
{\
    auto signals = ParentClassType::GetSignalList();\
    FOR_EACH(SIGNAL_PUSH_BACK, __VA_ARGS__)\
    return signals;\
}

#define SIGNALS_BASE(...)\
enum class Signals : u32\
{\
	FOR_EACH(SIGNAL_ENUM_CONTENT, __VA_ARGS__)\
};\
template<typename... T>\
inline bool EmitSignal(Signals signal,  T&&... params){\
switch(signal){\
FOR_EACH(SIGNAL_CASE, __VA_ARGS__)\
default: break;} return false;\
}\
virtual std::vector<KigsID> GetSignalList()\
{\
    std::vector<KigsID> signals;\
    FOR_EACH(SIGNAL_PUSH_BACK, __VA_ARGS__)\
    return signals;\
}

    }

}

