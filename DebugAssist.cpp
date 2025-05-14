#include "DebugAssist.h"
std::fstream DebugAssist::LogTo;
bool DebugAssist::useFileLog;
bool DebugAssist::useDebugOutput;
int DebugAssist::filePathCut;
std::string DebugAssist::logPrefix;
std::unordered_map<std::string, std::chrono::system_clock::time_point> PerformanceLog::StartTime;
std::unordered_map<std::string, float> PerformanceLog::Average;
std::unordered_map<std::string, int> PerformanceLog::SumOnTick;