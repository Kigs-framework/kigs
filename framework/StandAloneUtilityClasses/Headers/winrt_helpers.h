#pragma once

#include "FilePathManager.h"
#include <pplawait.h>

#include "Platform/Main/BaseApp.h"
#include <winrt/Windows.UI.Core.h>

template<typename TLambda>
auto invoke_async_lambda(TLambda lambda) -> decltype(lambda())
{
	return co_await lambda();
}

template<typename T>
winrt::fire_and_forget no_await_lambda(T t)
{
	co_await t();
}

template<typename T>
winrt::fire_and_forget no_await(T t)
{
	co_await t;
}

inline concurrency::task<SmartPointer<FileHandle>> MakeHandleFromStorageFile(winrt::Windows::Storage::StorageFile file)
{
	SmartPointer<FileHandle> result;
	auto name = file.Name();
	/*result = FilePathManager::CreateFileHandle(to_utf8(name.c_str()));
	result->myVirtualFileAccess = new StorageFileFileAccess(file);
	result->myUseVirtualFileAccess = true;
	co_return result;*/

	auto file_copy = co_await file.CopyAsync(winrt::Windows::Storage::ApplicationData::Current().TemporaryFolder(), name, winrt::Windows::Storage::NameCollisionOption::ReplaceExisting);
	if (file_copy)
	{
		result = FilePathManager::CreateFileHandle(to_utf8(file_copy.Path().data()));
	}
	co_return result;
}

inline SmartPointer<FileHandle> MakeHandleFromStorageFileNoCopy(winrt::Windows::Storage::StorageFile file)
{
	SmartPointer<FileHandle> result;
	result = FilePathManager::CreateFileHandle(to_utf8(file.Path().data()));
	return result;
}


inline auto resume_on_kigs_thread()
{
	return winrt::resume_foreground(App::GetApp()->GetWindow().Dispatcher());
}

template<typename Pred>
winrt::Windows::Foundation::IAsyncAction wait_until(Pred predicate, winrt::Windows::Foundation::TimeSpan check_frequency, bool check_on_same_thread = false)
{
	winrt::apartment_context ctx;
	while (!predicate())
	{
		co_await winrt::resume_after(check_frequency);
		if (check_on_same_thread)
			co_await ctx;
	}
}
