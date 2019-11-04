#pragma once

#include <codecvt>
#include <collection.h>


namespace CXHelpers
{
	inline std::string ToString(Platform::String^ str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		return convert.to_bytes(str->Data());
	}

	inline Platform::String^ FromString(const std::string& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		return ref new Platform::String(convert.from_bytes(str.c_str()).c_str());
	}

	template<typename TResult>
	bool WaitForAsyncOperation(Windows::Foundation::IAsyncOperation<TResult>^ op)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		for (;;)
		{
			auto s = op->Status;
			if (s == Windows::Foundation::AsyncStatus::Completed)
				break;

			else if (op->Status != Windows::Foundation::AsyncStatus::Started)
			{
				return false;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
		return true;
	}

}