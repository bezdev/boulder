#pragma once

#include <Windows.h>
#include <utility>
#include <stdexcept>

#define RETURN_IF_FAILED(hr) do { if (FAILED(hr)) { return hr; } } while(0)

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

inline void THROW_IF_FAILED(HRESULT hr, const char* message)
{
    if (FAILED(hr))
    {
        DebugBreak();
        throw std::runtime_error(message);
    }
}

template <typename T>
class lambda_call
{
public:
    explicit lambda_call(T&& lambda) : m_lambda(std::move(lambda))
    {
    }

    ~lambda_call()
    {
        m_lambda();
    }
protected:
    T m_lambda;
};

template <typename T>
inline auto scope_exit(T&& lambda)
{
    return lambda_call<T>(std::forward<T>(lambda));
}