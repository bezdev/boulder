#pragma once

class Timer
{
public:
    Timer();

    float GetTime() const;
    float GetDelta() const;

    void Update();

    void Reset();
    void Start();
    void Pause();

private:
    __int64 m_pauseTime;
    __int64 m_prevTime;
    __int64 m_startTime;
    __int64 m_currentTime;
    __int64 m_stoppedTime;

    float m_delta;
    double m_frequency;

    bool m_isPaused;

    __int64 GetCurrentTime();
};