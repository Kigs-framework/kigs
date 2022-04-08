#pragma once

#include "Platform/Main/BaseApp.h"
#include "FilePathManager.h"
#include "TecLibs/Tec3D.h"

#include <pplawait.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>

#if defined(KIGS_TOOLS) && !defined(KIGS_HOLOLENS2)
#include <winrt/Windows.System.Diagnostics.h>
#endif

inline mat4 Mat4FromFloat4x4(const winrt::Windows::Foundation::Numerics::float4x4& transform)
{
	auto mat_row_major = mat4(
		v4f(transform.m11, transform.m12, transform.m13, transform.m14),
		v4f(transform.m21, transform.m22, transform.m23, transform.m24),
		v4f(transform.m31, transform.m32, transform.m33, transform.m34),
		v4f(transform.m41, transform.m42, transform.m43, transform.m44));

	return mat_row_major;
}

inline mat3x4 Mat3x4FromFloat4x4(const winrt::Windows::Foundation::Numerics::float4x4& transform)
{
	auto mat = mat4(
		transform.m11, transform.m21, transform.m31, transform.m41,
		transform.m12, transform.m22, transform.m32, transform.m42,
		transform.m13, transform.m23, transform.m33, transform.m43,
		transform.m14, transform.m24, transform.m34, transform.m44);

	
	mat3x4 m{ mat.XAxis.xyz, mat.YAxis.xyz, mat.ZAxis.xyz, mat.Pos.xyz };
	return m;
}

template<typename TLambda>
auto invoke_async_lambda(TLambda lambda) -> decltype(lambda())
{
	return co_await lambda();
}

template<typename T>
winrt::fire_and_forget no_await_lambda(T lambda)
{
	co_await lambda();
}

template<typename T>
winrt::fire_and_forget no_await(T awaitable)
{
	co_await awaitable;
}

inline concurrency::task<SmartPointer<FileHandle>> MakeHandleFromStorageFile(winrt::Windows::Storage::StorageFile file)
{
	SmartPointer<FileHandle> result;
	auto name = file.Name();
	
#if defined(KIGS_TOOLS) && !defined(KIGS_HOLOLENS2)
	auto wid = to_wchar(std::to_string(winrt::Windows::System::Diagnostics::ProcessDiagnosticInfo::GetForCurrentProcess().ProcessId()));
	name = wid + L"_" + name;
#endif

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
