#include "Logger.h"

using namespace std;

Logger::Logger()
{
    m_fpsLog = std::vector<float>();
}

void Logger::LogFPS(float fps)
{
    m_fpsLog.push_back(fps);
}

float Logger::GetAverageFPS() const
{
    float average = 0;
    int count = 0;
    for (auto itFPS = m_fpsLog.begin(); itFPS != m_fpsLog.end(); itFPS++)
    { 
        average = ((average * count) + (*itFPS)) / (count + 1);
        count++;
    }

    return average;
}
