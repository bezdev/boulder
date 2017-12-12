#pragma once

#include <vector>
#include <string>

typedef std::vector<float> FPSLog;

class Logger
{
public:
    Logger();
    void LogFPS(float fps);
    float GetAverageFPS() const;
private:
    FPSLog m_fpsLog;
};