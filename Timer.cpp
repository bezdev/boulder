#include <Windows.h>

#include <stdexcept>

#include "timer.h"

Timer::Timer() :
    m_pauseTime(0),
    m_prevTime(0),
    m_startTime(0),
    m_currentTime(0),
    m_stoppedTime(0),
    m_delta(0.0f),
    m_frequency(0.0),
    m_isPaused(false)
{
    LARGE_INTEGER frequency;
    if (!QueryPerformanceFrequency(&frequency))
    {
        throw std::runtime_error("QueryPerformanceFrequency failed.");
    }

    m_frequency = 1.0 / frequency.QuadPart;
}

float Timer::GetTime() const
{
    if (m_isPaused)
    {
        return (float)((m_pauseTime - m_stoppedTime - m_startTime) * m_frequency * 1000);
    }
    else
    {
        return (float)((m_currentTime - m_stoppedTime - m_startTime) * m_frequency * 1000);
    }
}

float Timer::GetDelta() const
{
    return m_delta;
}

void Timer::Update()
{
    if (m_isPaused)
    {
        m_delta = 0.0f;
        return;
    }

    m_currentTime = GetCurrentTime();
    m_delta = static_cast<float>((m_currentTime - m_prevTime) * m_frequency * 1000);

    m_prevTime = m_currentTime;

    if (m_delta < 0.0f)
    {
        m_delta = 0.0f;
    }
}

void Timer::Reset()
{
    m_currentTime = GetCurrentTime();

    m_startTime = m_currentTime;
    m_prevTime = m_currentTime;
    m_pauseTime = 0;
    m_stoppedTime = 0;
    m_isPaused = false;
}

void Timer::Start()
{
    if (m_isPaused)
    {
        m_currentTime = GetCurrentTime();

        m_stoppedTime += (m_currentTime - m_pauseTime);
        m_startTime = m_currentTime;
        m_prevTime = m_currentTime;
        m_pauseTime = 0;
        m_isPaused = false;
        m_delta = 0.0f;
    }
}

void Timer::Pause()
{
    if (!m_isPaused)
    {
        m_currentTime = GetCurrentTime();
        m_pauseTime = m_currentTime;
        m_isPaused = true;
    }
}

__int64 Timer::GetCurrentTime()
{
    LARGE_INTEGER count;
    if (!QueryPerformanceCounter(&count))
    {
        throw std::runtime_error("QueryPerformanceFrequency failed.");
    }

    return count.QuadPart;
}