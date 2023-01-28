#pragma once
#include <algorithm>
#include <string>
#include <array>

namespace Kigs
{
	namespace impl
	{
		template <typename fromT, typename toT, typename Enabled = void>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, ...)
		{
			// ERROR
			return false;
		}

		// utility

		// check if it is an array
		template<typename>
		struct is_array : std::false_type {};

		template<typename T, std::size_t N>
		struct is_array<std::array<T, N>> : std::true_type {};

		template<>
		struct is_array<v2f> : std::true_type {};
		template<>
		struct is_array<v3f> : std::true_type {};
		template<>
		struct is_array<Vector3D> : std::true_type {};
		template<>
		struct is_array<v4f> : std::true_type {};
		template<>
		struct is_array<quat> : std::true_type {};
		template<>
		struct is_array<v2i> : std::true_type {};
		template<>
		struct is_array<v3i> : std::true_type {};
		template<>
		struct is_array<v2u> : std::true_type {};
		template<>
		struct is_array<v3u> : std::true_type {};
		template<>
		struct is_array<Matrix3x3> : std::true_type {};
		template<>
		struct is_array<Matrix3x4> : std::true_type {};
		template<>
		struct is_array<Matrix4x4> : std::true_type {};

		template <typename>
		struct get_array_size;
		template <typename T, size_t S>
		struct get_array_size<std::array<T, S>> {
			constexpr static size_t size = S;
		};

		template <typename T, class Enable = void>
		struct get_array_type;

		template <typename T, size_t S>
		struct get_array_type<std::array<T, S>> {
			using type = T;
		};

		template<typename T, typename std::enable_if<
				std::is_same<std::remove_cv_t<T>, v2f>::value 
			||	std::is_same<std::remove_cv_t<T>, v2i>::value
			||	std::is_same<std::remove_cv_t<T>, v2u>::value
		>::type* = nullptr>
		constexpr size_t arraySize()
		{
			return 2ull;
		}

		// return line number for matrix
		template<typename T, typename std::enable_if<
			std::is_same<std::remove_cv_t<T>, v3f>::value
			|| std::is_same<std::remove_cv_t<T>, Vector3D>::value
			|| std::is_same<std::remove_cv_t<T>, v3i>::value
			|| std::is_same<std::remove_cv_t<T>, v3u>::value
			|| std::is_same<std::remove_cv_t<T>, Matrix3x3>::value // column first matrix
		>::type* = nullptr>
		constexpr size_t arraySize()
		{
			return 3ull;
		}

		// return line number for matrix
		template<typename T, typename std::enable_if<
			std::is_same<std::remove_cv_t<T>, v4f>::value
			|| std::is_same<std::remove_cv_t<T>, quat>::value
			|| std::is_same<std::remove_cv_t<T>, Matrix3x4>::value // colum first matrix
			|| std::is_same<std::remove_cv_t<T>, Matrix4x4>::value
		>::type* = nullptr>
		constexpr size_t arraySize()
		{
			return 4ull;
		}

		template<typename T, typename std::enable_if<
			std::is_same<std::remove_cv_t<T>, std::array<typename get_array_type<T>::type,get_array_size<T>::size>>::value
		>::type* = nullptr>
		constexpr size_t arraySize()
		{
			return get_array_size<T>::size;
		}

		template<typename T>
		constexpr size_t arrayColumnCount()
		{
			return arraySize<T>();
		}

		template<typename T, typename std::enable_if<
			std::is_same<std::remove_cv_t<T>, v2f>::value
			|| std::is_same<std::remove_cv_t<T>, v2i>::value
			|| std::is_same<std::remove_cv_t<T>, v2u>::value
			|| std::is_same<std::remove_cv_t<T>, v3f>::value
			|| std::is_same<std::remove_cv_t<T>, Vector3D>::value
			|| std::is_same<std::remove_cv_t<T>, v3i>::value
			|| std::is_same<std::remove_cv_t<T>, v3u>::value
			|| std::is_same<std::remove_cv_t<T>, v4f>::value
			|| std::is_same<std::remove_cv_t<T>, quat>::value
		>::type* = nullptr>
		constexpr size_t arrayLineCount()
		{
			return 1ull;
		}

		template<typename T, typename std::enable_if<
				std::is_same<std::remove_cv_t<T>, Matrix3x3>::value // column first
			||	std::is_same<std::remove_cv_t<T>, Matrix3x4>::value
		>::type* = nullptr>
		constexpr size_t arrayLineCount()
		{
			return 3ull;
		}

		template<typename T, typename std::enable_if<
			std::is_same<std::remove_cv_t<T>, Matrix4x4>::value
		>::type* = nullptr>
		constexpr size_t arrayLineCount()
		{
			return 4ull;
		}

		template<typename T, typename std::enable_if<
			std::is_same<std::remove_cv_t<T>, std::array<typename get_array_type<T>::type, get_array_size<T>::size>>::value
		>::type* = nullptr>
		constexpr size_t arrayLineCount()
		{
			if constexpr (is_array<typename get_array_type<std::remove_cv_t<T>>::type>::value) // 2d array column first
			{
				return get_array_size< std::remove_cv_t<typename get_array_type<T>::type>>::size;
			}
			return 1ull;
		}

		// declaration
		template<typename T, typename strT> bool CoreConvertString2Array_impl(const strT& stringValue, T* arrayValue, size_t arrayNbElements);
		template<typename T, typename strT> bool CoreConvertArray2String_impl(strT& stringValue, T* arrayValue, size_t arrayNbElements);

		template<typename fromT, typename toT, typename std::enable_if<std::is_arithmetic<fromT>::value&& std::is_arithmetic<toT>::value && !std::is_same<fromT, toT>::value>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			toval = (toT)fromval;
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<std::is_arithmetic<fromT>::value && 
			(
					std::is_same<toT, std::string>::value
				||  std::is_same<toT, usString>::value
				)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			toval = std::to_string(fromval);
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<
			std::numeric_limits<toT>::is_integer
			&& std::numeric_limits<toT>::is_signed
			&& (
					std::is_same<fromT, std::string>::value 
				||	std::is_same<fromT, usString>::value
				)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			std::string toParse(fromval);
			if constexpr (sizeof(toT) > 4) // 64 bits
			{
				toval = (toT)std::stoll(toParse);
			}
			else // 32 bits signed integer
			{
				toval = (toT)std::stol(toParse);
			}
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<
			std::numeric_limits<toT>::is_integer
			&& !std::numeric_limits<toT>::is_signed
			&& (
					std::is_same<fromT, std::string>::value 
				||	std::is_same<fromT, usString>::value
				)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			std::string toParse(fromval);
			if constexpr (sizeof(toT) > 4) // 64 bits
			{
				toval = (toT)std::stoull(toParse);
			}
			else // 32 bits signed integer
			{
				toval = (toT)std::stoul(toParse);
			}
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<
			std::is_floating_point<toT>::value
			&& (
					std::is_same<fromT, std::string>::value 
				||  std::is_same<fromT, usString>::value
				)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			std::string toParse(fromval);
			if constexpr (sizeof(toT) > 4) // 64 bits
			{
				toval = (toT)std::stod(toParse);
			}
			else // 32 bits signed integer
			{
				toval = (toT)std::stof(toParse);
			}
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<std::is_same<fromT, toT>::value>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			toval = fromval;
			return true;
		}

		// v2f, v3f, v4f, quaternion, v2i, v3i, v2u, v3u
		template<typename fromT, typename toT, typename std::enable_if<
			(is_array<std::remove_cv_t<fromT>>::value)
			&& (is_array<toT>::value)
			&& (!std::is_same<fromT, toT>::value)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			constexpr size_t copySize = std::min(arraySize< std::remove_cv_t<fromT>>(), arraySize<toT>());
			for (size_t i = 0; i < copySize; i++)
			{
				if (!CoreConvertValue_impl(fromval[i], toval[i], int{})) return false;
			}
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<
			(
				is_array<std::remove_cv_t<fromT>>::value
				)
			&& (
				std::is_same<toT, std::string>::value
				|| std::is_same<toT, usString>::value
				)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			CoreConvertArray2String_impl(toval, &fromval[0], arraySize<fromT>());
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<
			(
				is_array< toT>::value
				)
			&& (
				std::is_same<std::remove_cv_t<fromT>, std::string>::value
				|| std::is_same<std::remove_cv_t<fromT>, usString>::value
				)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			CoreConvertString2Array_impl(fromval, &toval[0], arraySize<toT>());
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<
			(std::is_same<std::remove_cv_t<fromT>, std::string>::value)
			&& (std::is_same<std::remove_cv_t<toT>, usString>::value)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			toval = toT(fromval);
			return true;
		}

		template<typename fromT, typename toT, typename std::enable_if<
			(std::is_same<std::remove_cv_t<fromT>, usString>::value)
			&& (std::is_same<std::remove_cv_t<toT>, std::string>::value)
		>::type* = nullptr>
		inline bool CoreConvertValue_impl(const fromT& fromval, toT& toval, int)
		{
			toval = fromval.ToString();
			return true;
		}


		// set array values from braced string like : {1.0,0.0,2.0}
		template<typename T,typename strT> bool CoreConvertString2Array_impl(const strT& stringValue, T* arrayValue, size_t arrayNbElements)
		{
			std::string stringToParse=(std::string)stringValue;

			string::size_type posToParse = stringToParse.find('{', 0);
			if (posToParse == string::npos) return false;
			posToParse++;

			for (size_t i = 0; i < arrayNbElements; i++)
			{
				// End of string, return
				if (posToParse >= stringToParse.size()) return false;

				// T is itself an array, recurse
				string::size_type	nextPosToParse = 0;
				std::string			stringToConvert;
				if constexpr (is_array<std::remove_cv_t<T> >::value)
				{
					nextPosToParse = stringToParse.find('}', posToParse);
					stringToConvert.assign(stringToParse, posToParse, nextPosToParse - posToParse);
					if (!CoreConvertString2Array_impl(stringToConvert, &(arrayValue[i][0]), arraySize<std::remove_cv_t<T> >())) return false;
					posToParse = nextPosToParse + 1;
				}
				
				// search next separator : ',' or '}'
				nextPosToParse = stringToParse.find(',', posToParse);

				if (nextPosToParse == string::npos)
				{
					nextPosToParse = stringToParse.find('}', posToParse);
				}
				if (nextPosToParse == string::npos) // next separator not found, go to the end of the string
				{
					nextPosToParse = stringToParse.size();
				}
				if constexpr (!is_array<std::remove_cv_t<T> >::value)
				{
					stringToConvert.assign(stringToParse, posToParse, nextPosToParse - posToParse);
					if (!CoreConvertValue_impl(stringToConvert, arrayValue[i], int{})) return false;
				}
				posToParse = nextPosToParse + 1;
				
			}
			return true;
		}

		// create a string using braces like this : {val1,val2...} from an array
		template<typename T, typename strT> bool CoreConvertArray2String_impl(strT& stringValue, T* arrayValue, size_t arrayNbElements)
		{
			if (arrayNbElements)
			{
				// work with std::string
				std::string returnedValue = "{";
				for (size_t i = 0; i < arrayNbElements; i++)
				{
					if (i != 0) returnedValue += ",";
					std::string val;
					/*if constexpr (is_array<std::remove_cv_t<T>>::value)
					{
						if (!CoreConvertArray2String_impl(val, &(arrayValue[i][0]), arraySize<std::remove_cv_t<T>>())) return false;
					}
					else
					{*/
						if (!CoreConvertValue_impl(arrayValue[i], val, int{})) return false;
					//}
					returnedValue += val;
				}
				returnedValue += "}";
				stringValue = strT(returnedValue);
				return true;
			}
			return false;
		}



	}

	template <typename fromT, typename toT>
	inline bool CoreConvertValue(const fromT& fromval, toT& toval)
	{
		return impl::CoreConvertValue_impl(fromval, toval, int{});
	}

	template<typename toT>
	inline bool CoreConvertValue(const char* fromval, toT& toval)
	{
		return CoreConvertValue(std::string(fromval), toval);
	}

	template<typename toT>
	inline bool CoreConvertValue(const unsigned short* fromval, toT& toval)
	{
		return CoreConvertValue(usString(fromval), toval);
	}

	template<typename toT>
	inline bool CoreConvertValue(const UTF8Char* fromval, toT& toval)
	{
		return CoreConvertValue(usString(fromval), toval);
	}

	// other specialization

	// bool
	template<>
	inline bool CoreConvertValue(const bool& fromval, std::string& toval)
	{
		fromval ? (toval = "true") : (toval = "false");
		return true;
	}

	template<>
	inline bool CoreConvertValue(const std::string& fromval, bool& toval)
	{
		toval = ((fromval == "true") || (fromval == "TRUE"));
		return true;
	}

	inline bool CoreConvertValue(const char* fromval, bool& toval)
	{
		return CoreConvertValue<std::string, bool>(fromval, toval);
	}


}