#pragma once

#include "CoreSTL.h"
#include "maReference.h"
#include "maUSString.h"
#include "maString.h"
#include "maNumeric.h"
#include "maAny.h"
#include "maBuffer.h"
#include "maArray.h"
#include "PackCoreModifiableAttributes.h"

namespace Kigs
{
	namespace Action
	{
		class CoreSequence;
	}
	namespace Input
	{
		struct ClickEvent;
	}
	namespace Core
	{

		#define DEFINE_MAKE_ATTR_FUNC(type, attr_type) inline CoreModifiableAttribute* MakeAttributeSpec(type value, CoreModifiable* owner, const std::string& name = #attr_type)\
		{\
			if(owner)\
				return new attr_type(*owner, name, value); \
			else\
				return new attr_type##Orphan(name, value);\
		}

		#define DEFINE_MAKE_ATTR_FUNC_VECNF(type, attr_type) inline CoreModifiableAttribute* MakeAttributeSpec(type value, CoreModifiable* owner, const std::string& name = #attr_type)\
		{\
			if(owner)\
				return new attr_type(*owner, name, (attr_type::ArrayType::value_type*)&value);\
			else {\
				attr_type::ArrayType val;\
				memcpy(val.data(), &value, val.size() * sizeof(attr_type::ArrayType::value_type));\
				return new attr_type##Orphan(name, val);\
			}\
		}

		template<typename T>
		struct Returns
		{
			typedef T Underlying;
		};

		DEFINE_MAKE_ATTR_FUNC(s32, maInt)
		DEFINE_MAKE_ATTR_FUNC(u32, maUInt)
		DEFINE_MAKE_ATTR_FUNC(s64, maLong)
		DEFINE_MAKE_ATTR_FUNC(u64, maULong)
		DEFINE_MAKE_ATTR_FUNC(float, maFloat)
		DEFINE_MAKE_ATTR_FUNC(double, maDouble)
		DEFINE_MAKE_ATTR_FUNC(bool, maBool)

		DEFINE_MAKE_ATTR_FUNC_VECNF(v2i, maVect2DI)
		DEFINE_MAKE_ATTR_FUNC_VECNF(v3i, maVect3DI)

		DEFINE_MAKE_ATTR_FUNC_VECNF(v2f, maVect2DF)
		DEFINE_MAKE_ATTR_FUNC_VECNF(v3f, maVect3DF)
		DEFINE_MAKE_ATTR_FUNC_VECNF(v4f, maVect4DF)

		DEFINE_MAKE_ATTR_FUNC(const std::string&, maString)
		DEFINE_MAKE_ATTR_FUNC(std::string&&, maString)
		DEFINE_MAKE_ATTR_FUNC(const char*, maString)
		DEFINE_MAKE_ATTR_FUNC(const usString&, maUSString)
		DEFINE_MAKE_ATTR_FUNC(usString&&, maUSString)

		DEFINE_MAKE_ATTR_FUNC(CoreModifiable*, maRawPtr);

		template<typename T>
		inline CoreModifiableAttribute* MakeAttributeSpec(SP<T> value, CoreModifiable* owner, const std::string& name = "maStrongReference")
		{
			if (owner)
				return new maStrongReference(*owner, name, maStrongReferenceObject{ value });
			else
				return new maStrongReferenceOrphan(name, maStrongReferenceObject{ value });
		}

		template<typename T, REQUIRES(!std::is_fundamental<std::decay_t<T>>::value)>
		CoreModifiableAttribute* MakeAttributeSpec(T&& value, CoreModifiable* owner, const std::string& name = "maRawPtrStruct")
		{
			static_assert(!std::is_same<T, T>::value, "Cannot wrap rvalues of structs");
		}

		template<typename T>
		struct LuaStruct
		{
			static constexpr bool exposed = false;
			constexpr static const char* name() { return nullptr; }
		};

		#define EXPOSE_STRUCT_LUA(structclass, type) structclass type;\
		template<>\
		struct LuaStruct<type>{ static constexpr bool exposed = true; constexpr static const char* name(){ return #type; }};

		#define EXPOSE_TYPE_LUA(structclass, type, luaname)  structclass type;\
		template<>\
		struct LuaStruct<type>{ static constexpr bool exposed = true; constexpr static const char* name(){ return luaname; }};

		template<>
		struct LuaStruct<Input::ClickEvent> { static constexpr bool exposed = true; constexpr static const char* name() { return "ClickEvent"; } };
		template<>
		struct LuaStruct<Action::CoreSequence> { static constexpr bool exposed = true; constexpr static const char* name() { return "CoreSequence"; } };

		// Custom type exposed by ptr
		template<typename T, REQUIRES(LuaStruct<stripped_type<T>>::exposed)>
		CoreModifiableAttribute* MakeAttribute(T* value, CoreModifiable* owner, const std::string& name = LuaStruct<stripped_type<T>>::name())
		{
			void* ptr = (void*)value;
			if (owner)
				return new maRawPtr(*owner, name, ptr);
			else
				return new maRawPtrOrphan(name, ptr);
		}

		// Custom type exposed by ref
		template<typename T, REQUIRES(LuaStruct<stripped_type<T>>::exposed)>
		CoreModifiableAttribute* MakeAttribute(T& value, CoreModifiable* owner, const std::string& name = LuaStruct<stripped_type<T>>::name())
		{
			void* ptr = (void*)&value;
			if (owner)
				return new maRawPtr(*owner, name, ptr);
			else
				return new maRawPtrOrphan(name, ptr);
		}


		// Custom type exposed by rvalue
		template<typename T, REQUIRES(LuaStruct<stripped_type<T>>::exposed)>
		CoreModifiableAttribute* MakeAttribute(T&& value, CoreModifiable* owner, const std::string& name = LuaStruct<stripped_type<T>>::name())
		{
			static_assert(!std::is_same<T, T>::value, "Cannot wrap rvalues of structs");
		}


		// Non coremodifiable type, non exposed by pointer
		template<typename T, REQUIRES(!LuaStruct<stripped_type<T>>::exposed &&
			!std::is_base_of<CoreModifiable, std::decay_t<T>>::value &&
			!std::is_same<char, std::decay_t<T>>::value)
		>
		CoreModifiableAttribute* MakeAttribute(T* value, CoreModifiable* owner, const std::string& name = "ParamPtr")
		{
			void* ptr = (void*)value;
			if (owner)
				return new maRawPtr(*owner, name, ptr);
			else
				return new maRawPtrOrphan(name, ptr);
		}

		// Non exposed, non coremodifiable, non fundamental type by reference
		template<typename T, REQUIRES(!LuaStruct<stripped_type<T>>::exposed &&
			!std::is_fundamental<std::decay_t<T>>::value &&
			!std::is_base_of<CoreModifiable, stripped_type<T>>::value &&
			!is_detected_v<is_smart_pointer, std::decay_t<T>>)
		>
		CoreModifiableAttribute* MakeAttribute(T& value, CoreModifiable* owner, const std::string& name = "ParamRef")
		{
			void* ptr = (void*)&value;
			if (owner)
				return new maRawPtr(*owner, name, ptr);
			else
				return new maRawPtrOrphan(name, ptr);
		}

		// Non exposed, non coremodifiable, non fundamental type by rvalue
		// If the type has an equivalent CoreModifiableAttribute use that instead of a pointer to a possible temporary
		template<typename T, REQUIRES(!LuaStruct<stripped_type<T>>::exposed &&
			!std::is_fundamental<std::decay_t<T>>::value &&
			!std::is_base_of<CoreModifiable, stripped_type<T>>::value &&
			!is_detected_v<is_smart_pointer, std::decay_t<T>>)
		>
		CoreModifiableAttribute* MakeAttribute(T&& value, CoreModifiable* owner, const std::string& name = "ParamRValue")
		{
			return MakeAttributeSpec(FWD(value), owner);
		}

		// SmartPointer
		template<typename T>
		CoreModifiableAttribute* MakeAttribute(const SmartPointer<T>& value, CoreModifiable* owner, const std::string& name = "ParamSP")
		{
			return MakeAttributeSpec(value, owner, name);
		}

		// r-value reference of a fundamental type. use equivalent CoreModifiableAttribute type
		template<typename T, REQUIRES(std::is_fundamental<std::decay_t<T>>::value)>
		CoreModifiableAttribute* MakeAttribute(T&& value, CoreModifiable* owner, const std::string& name = "ParamFundamental")
		{
			return MakeAttributeSpec(std::forward<T>(value), owner, name);
		}


		// nullptr
		template<>
		inline CoreModifiableAttribute* MakeAttribute(std::nullptr_t&& value, CoreModifiable* owner, const std::string& name)
		{
			return MakeAttribute((void*)value, owner, name);
		}

		// c-string. use maString
		inline CoreModifiableAttribute* MakeAttribute(const char* value, CoreModifiable* owner, const std::string& name = "ParamCString")
		{
			return MakeAttributeSpec(value, owner, name);
		}

		template<size_t N>
		inline CoreModifiableAttribute* MakeAttribute(char(&value)[N], CoreModifiable* owner, const std::string& name = "ParamCString")
		{
			return MakeAttributeSpec((const char*)value, owner, name);
		}

		// CoreModifiable ptr. use maReference
		inline CoreModifiableAttribute* MakeAttribute(CoreModifiable* value, CoreModifiable* owner, const std::string& name = "ParamCMPtr")
		{
			return MakeAttributeSpec(value, owner, name);
		}

		// CoreModifiable ref. use maReference
		inline CoreModifiableAttribute* MakeAttribute(CoreModifiable& value, CoreModifiable* owner, const std::string& name = "ParamCMRef")
		{
			return MakeAttributeSpec(&value, owner, name);
		}



		#define PUSH_RETURN_VALUE(val) params.push_back(MakeAttribute((std::move(val)), nullptr, "Result"))


		template<typename T>
		struct ptr_to_ref
		{
			using type = T;

			ptr_to_ref() : ptr{ nullptr } {}

			ptr_to_ref(const ptr_to_ref<T>&) = default;
			ptr_to_ref(ptr_to_ref<T>&&) = default;

			ptr_to_ref<T>& operator=(const ptr_to_ref<T>&) = default;
			ptr_to_ref<T>& operator=(ptr_to_ref<T>&&) = default;

			ptr_to_ref(T&& el) : ptr{ &el } {}
			ptr_to_ref(T* el) : ptr{ el } {}
			ptr_to_ref<T>& operator=(T&& el) { ptr = &el; return *this; }
			ptr_to_ref<T>& operator=(T* el) { ptr = el; return *this; }

			operator T* () { return ptr; }
			operator T& () { return *ptr; }

			operator const T* () const { return ptr; }
			operator const T& () const { return *ptr; }

			T* ptr;
		};

		namespace kigs_impl
		{
			using namespace std;

			template<bool...> struct bool_pack;
			template<bool... bs>
			using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

			// Utility functions to manipulate std::vector<CoreModifiableAttribute*>
			template<int> struct int2type {};

			template<typename T, typename U = decay_t<T>,
				typename Enable = enable_if_t<is_fundamental<U>::value, U>>
				void UnpackGetValue(T& value, CoreModifiableAttribute* attr)
			{
				if (!attr->getValue(value,nullptr))
				{
					void* ptr = nullptr;
					if (attr->getValue(ptr,nullptr))
						value = *(T*)ptr;
				}
			}

			template<typename T>
			auto UnpackGetValue(SP<T>& value, CoreModifiableAttribute* attr)
			{
				void* ptr;
				if (attr->getValue(ptr, nullptr))
					value = ((T*)ptr)->SharedFromThis();
			}

			template<typename T>
			auto UnpackGetValue(ptr_to_ref<T>& value, CoreModifiableAttribute* attr) -> enable_if_t<!is_base_of<CoreModifiable, T>::value>
			{
				void* ptr;
				if (attr->getValue(ptr, nullptr))
					value = (T*)ptr;
			}

			template<typename T>
			auto UnpackGetValue(ptr_to_ref<T>& value, CoreModifiableAttribute* attr) -> enable_if_t<is_base_of<CoreModifiable, T>::value>
			{
				void* vcm = nullptr;
				if (attr->getValue(vcm,nullptr) && vcm)
				{
					CoreModifiable* cm = static_cast<CoreModifiable*>(vcm);
					if (cm->isSubType(T::mClassID))
					{
						value = static_cast<T*>(cm);
					}
				}
			}

			inline void UnpackGetValue(ptr_to_ref<const char>& value, CoreModifiableAttribute* attr)
			{
				void* ptr;
				if (attr->getValue(ptr, nullptr))
				{
					if (attr->getType() == CoreModifiable::ATTRIBUTE_TYPE::STRING)
						value = ((std::string*)ptr)->c_str();
					else
						value = (const char*)ptr;
				}
			}

			inline void UnpackGetValue(void*& value, CoreModifiableAttribute* attr)
			{
				attr->getValue(value, nullptr);
			}


			template<typename T>
			void UnpackParam(T& into, std::vector<CoreModifiableAttribute*>& params, int2type<-1>)
			{

			}

			template<typename T>
			void UnpackParam(T& into, std::vector<CoreModifiableAttribute*>& params, int2type<0>)
			{
				if (0 < params.size())
					UnpackGetValue(get<0>(into), params[0]);
			}

			template<int N, typename T>
			void UnpackParam(T& into, std::vector<CoreModifiableAttribute*>& params, int2type<N>)
			{
				if (N < params.size())
				{
					UnpackGetValue(get<N>(into), params[N]);
				}
				UnpackParam(into, params, int2type<N - 1>{});
			}


			// Only fundamental types can be passed by value, everything else if wrapped with ptr_to_ref
			template<typename T,
				typename U = remove_pointer_t<decay_t<T>>,
				//		typename PtrToRefType = enable_if_t<is_fundamental<U>::value || !is_same<T, U>::value, ptr_to_ref<U>>>
				typename PtrToRefType = conditional_t<is_void<U>::value, U*, ptr_to_ref<U>>>
			struct type_for_unpack
			{
			public:
				typedef conditional_t<is_fundamental<T>::value, T, conditional_t<is_detected_v<is_smart_pointer, U>, U, PtrToRefType>> type;
			};

			template<typename T>
			using type_for_unpack_t = typename type_for_unpack<T>::type;

			template<typename... T>
			tuple<T...> Unpack(std::vector<CoreModifiableAttribute*>& params)
			{
				tuple<T...> result = {};
				UnpackParam(result, params, int2type<((int)sizeof...(T)) - 1>{});
				return result;
			}

			template<typename T>
			struct LambdaUnpacker : public LambdaUnpacker<decltype(&T::operator())>
			{

			};

			template<typename T, typename Ret, typename ... Args>
			struct LambdaUnpacker<Ret(T::*)(Args...) const>
			{
				typedef LambdaUnpacker<decltype(&T::operator())> PackerType;
				typedef Ret ReturnType;
				typedef typename std::tuple<kigs_impl::type_for_unpack_t<Args>...> tuple_type;
				static_assert(all_true<(!(is_fundamental<std::decay_t<Args>>::value&& is_reference<Args>::value))...>::value, "Cannot wrap reference to fundamental type, use a pointer instead !");
				static_assert(all_true<(!is_same<std::decay_t<Args>, const char*>::value)...>::value, "Cannot wrap c-string parameter, it is prone to errors");
				static auto Unpack(std::vector<CoreModifiableAttribute*>& params) -> tuple_type
				{
					tuple_type result = {};
					UnpackParam(result, params, int2type<((int)sizeof...(Args)) - 1>{});
					return result;
				}
			};

			template<typename T, typename Ret, typename ... Args>
			struct LambdaUnpacker<Ret(T::*)(Args...)>
			{
				typedef LambdaUnpacker<decltype(&T::operator())> PackerType;
				typedef Ret ReturnType;
				typedef typename std::tuple<kigs_impl::type_for_unpack_t<Args>...> tuple_type;
				static_assert(all_true<(!(is_fundamental<std::decay_t<Args>>::value&& is_reference<Args>::value))...>::value, "Cannot wrap reference to fundamental type, use a pointer instead !");
				static_assert(all_true<(!is_same<std::decay_t<Args>, const char*>::value)...>::value, "Cannot wrap c-string parameter, it is prone to errors");
				static auto Unpack(std::vector<CoreModifiableAttribute*>& params) -> tuple_type
				{
					tuple_type result = {};
					UnpackParam(result, params, int2type<((int)sizeof...(Args)) - 1>{});
					return result;
				}
			};


			// Use a tuple as arguments to a method call
			template <typename F, typename T, typename Tuple, size_t... I>
			decltype(auto) apply_impl(F&& f, T&& obj, Tuple&& t, index_sequence<I...>)
			{
				return (obj->*std::forward<F>(f))(get<I>(std::forward<Tuple>(t))...);
			}

			template <typename F, typename T, typename Tuple>
			decltype(auto) apply(F&& f, T&& obj, Tuple&& t)
			{
				using Indices = make_index_sequence<tuple_size<decay_t<Tuple>>::value>;
				return apply_impl(std::forward<F>(f), std::forward<T>(obj), std::forward<Tuple>(t), Indices{});
			}


			// Use a tuple as arguments to a function call
			template <typename F, typename Tuple, size_t... I>
			decltype(auto) apply_func_impl(F&& f, Tuple&& t, index_sequence<I...>)
			{
				return f(get<I>(std::forward<Tuple>(t))...);
			}

			template <typename F, typename Tuple>
			decltype(auto) apply_func(F&& f, Tuple&& t)
			{
				using Indices = make_index_sequence<tuple_size<decay_t<Tuple>>::value>;
				return apply_func_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
			}




			//	static_assert(all_true<(Numbers == 0 || Numbers == 1)...>::value, "");




			template<typename Ret, typename T, typename T2, typename... Args>
			void UnpackAndCall(Ret(T::* Func)(Args...), T2* obj, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& params)
			{
				auto t = Unpack<type_for_unpack_t<Args>...>(params);
				static_assert(all_true<(!(is_fundamental<std::decay_t<Args>>::value&& is_reference<Args>::value))...>::value, "Cannot wrap reference to fundamental type, use a pointer instead !");
				static_assert(all_true<(!is_same<std::decay_t<Args>, const char*>::value)...>::value, "Cannot wrap c-string parameter, it is prone to errors");
				params.push_back(MakeAttribute((apply(FWD(Func), FWD(obj), t)), nullptr));
			}

			template<typename Ret, typename T, typename T2, typename... Args>
			void UnpackAndCall(Ret(T::* Func)(Args...) const, const T2* obj, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& params)
			{
				auto t = Unpack<type_for_unpack_t<Args>...>(params);
				static_assert(all_true<(!(is_fundamental<std::decay_t<Args>>::value&& is_reference<Args>::value))...>::value, "Cannot wrap reference to fundamental type, use a pointer instead !");
				static_assert(all_true<(!is_same<std::decay_t<Args>, const char*>::value)...>::value, "Cannot wrap c-string parameter, it is prone to errors");
				params.push_back(MakeAttribute((apply(FWD(Func), FWD(obj), t)), nullptr));
			}

			template<typename T, typename T2, typename... Args>
			void UnpackAndCall(void(T::* Func)(Args...), T2* obj, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& params)
			{
				auto t = Unpack<type_for_unpack_t<Args>...>(params);
				static_assert(all_true<(!(is_fundamental<std::decay_t<Args>>::value&& is_reference<Args>::value))...>::value, "Cannot wrap reference to fundamental type, use a pointer instead !");
				static_assert(all_true<(!is_same<std::decay_t<Args>, const char*>::value)...>::value, "Cannot wrap c-string parameter, it is prone to errors");
				apply(FWD(Func), FWD(obj), t);
			}

			template<typename T, typename T2, typename... Args>
			void UnpackAndCall(void(T::* Func)(Args...) const, const T2* obj, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& params)
			{
				auto t = Unpack<type_for_unpack_t<Args>...>(params);
				static_assert(all_true<(!(is_fundamental<std::decay_t<Args>>::value&& is_reference<Args>::value))...>::value, "Cannot wrap reference to fundamental type, use a pointer instead !");
				static_assert(all_true<(!is_same<std::decay_t<Args>, const char*>::value)...>::value, "Cannot wrap c-string parameter, it is prone to errors");
				apply(FWD(Func), FWD(obj), t);
			}


			template<typename F, typename Unpacker = typename LambdaUnpacker<std::remove_reference_t<F>>::PackerType, REQUIRES(std::is_void<typename Unpacker::ReturnType>::value)>
			void UnpackAndCall(F&& func, std::vector<CoreModifiableAttribute*>& params)
			{
				auto t = Unpacker::Unpack(params);
				apply_func(FWD(func), t);
			}

			template<typename F, typename Unpacker = typename LambdaUnpacker<std::remove_reference_t<F>>::PackerType, REQUIRES(!std::is_void<typename Unpacker::ReturnType>::value)>
			void UnpackAndCall(F&& func, std::vector<CoreModifiableAttribute*>& params)
			{
				auto t = Unpacker::Unpack(params);
				params.push_back(MakeAttribute((apply_func(FWD(func), t)), nullptr));
			}
		}

		template<typename Ret, typename... T>
		Ret CoreModifiable::SimpleCall(KigsID methodNameID, T&&... params)
		{
			PackCoreModifiableAttributes	attr(nullptr);
			int expander[]
			{
				(attr << std::forward<T>(params), 0)...
			};
			(void)expander;

			auto& attr_list = (std::vector<CoreModifiableAttribute*>&)attr;
			size_t before = attr_list.size();
			CallMethod(methodNameID, attr_list);
			kigs_impl::type_for_unpack_t<Ret> result = {};
			if (attr_list.size() == before + 1)
			{
				kigs_impl::UnpackGetValue(result, attr_list.back());
			}
			return result;
		}

		template<typename Ret>
		Ret CoreModifiable::SimpleCall(KigsID methodNameID)
		{
			std::vector<CoreModifiableAttribute*> attr;
			CallMethod(methodNameID, attr);
			Ret result = {};
			if (attr.size())
			{
				attr.back()->getValue(result,nullptr);

				// destroy attributes
				while (attr.size())
				{
					delete attr.back();
					attr.pop_back();
				}

			}
			return result;
		}

		template<typename... T>
		bool CoreModifiable::SimpleCall(KigsID methodNameID, T&&... params)
		{
			PackCoreModifiableAttributes	attr(nullptr);
			int expander[]
			{
				(attr << std::forward<T>(params), 0)...
			};
			(void)expander;
			auto& attr_list = (std::vector<CoreModifiableAttribute*>&)attr;
			return CallMethod(methodNameID, attr_list);
		}


		template<typename... T>
		inline bool CoreModifiable::EmitSignal(const KigsID& SignalID, T&&... params)
		{
			if (!mLazyContent) return false;
			auto& mutex = GetMutex();
			mutex.lock_shared();
			auto& connected_to = GetLazyContent()->mConnectedTo;
			auto it = connected_to.find(SignalID);
			if (it != connected_to.end())
			{
				auto copy = it->second;
				mutex.unlock_shared();

				PackCoreModifiableAttributes	attr(nullptr);
				int expander[]
				{
					(attr << std::forward<T>(params), 0)...
				};
				(void)expander;
				auto& attr_list = (std::vector<CoreModifiableAttribute*>&)attr;

				for (auto& p : copy)
				{
					if (auto ptr = p.second.lock())
					{
						ptr->CallMethod(p.first, attr_list, nullptr, this);
					}
				}
				return true;
			}
			mutex.unlock_shared();
			return false;
		}


		template<typename T>
		PackCoreModifiableAttributes& PackCoreModifiableAttributes::operator<<(T&& V)
		{
			mAttributeList.push_back(MakeAttribute(std::forward<T>(V), mOwner));
			return *this;
		}


		template<typename F>
		inline void CoreModifiable::InsertFunction(KigsID labelID, F&& func)
		{
			auto& methods = GetLazyContent()->mMethods;
			ModifiableMethodStruct toAdd{ [f = std::move(func)](CoreModifiable* localthis, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& params, void* privateParams) mutable
			{
				kigs_impl::UnpackAndCall(f, params);
				return false;
			} };
#ifdef KIGS_TOOLS
			toAdd.xmlattr = nullptr;
#endif
			methods[labelID] = toAdd;
		}

		template<typename F>
		inline void CoreModifiable::InsertFunctionNoUnpack(KigsID labelID, F&& func)
		{
			auto& methods = GetLazyContent()->mMethods;
			ModifiableMethodStruct toAdd{ [f = std::move(func)](CoreModifiable* localthis, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& params, void* privateParams) mutable
			{
				f(params);
				return false;
			} };
#ifdef KIGS_TOOLS
			toAdd.xmlattr = nullptr;
#endif
			methods[labelID] = toAdd;
		}

		template<typename Ret, typename ... Args>
		inline void CoreModifiable::InsertFunction(KigsID labelID, Ret(*func)(Args...))
		{
			InsertFunction(labelID, [func](Args... args) -> Ret
				{
					return func(args...);
				});
		}
	}
}