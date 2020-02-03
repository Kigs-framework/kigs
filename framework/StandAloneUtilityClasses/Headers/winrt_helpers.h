#pragma once

#include "FilePathManager.h"
#include <pplawait.h>


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
	auto file_copy = co_await file.CopyAsync(winrt::Windows::Storage::ApplicationData::Current().TemporaryFolder(), name, winrt::Windows::Storage::NameCollisionOption::ReplaceExisting);
	if (file_copy)
	{
		result = FilePathManager::CreateFileHandle(to_utf8(file_copy.Path().data()));
	}
	co_return result;
}