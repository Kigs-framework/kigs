#pragma once

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