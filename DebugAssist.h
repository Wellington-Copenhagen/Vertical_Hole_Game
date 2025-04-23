#pragma once
#include "Resource.h"
#include "framework.h"
#define DebugLogOutput DebugAssist::_SetPrefix(__LINE__,__func__,__FILE__);DebugAssist::Append
extern int Tick;
class DebugAssist {
public:
	static std::fstream LogTo;
	static std::string logPrefix;
	static bool useFileLog;
	static bool useDebugOutput;
	// __FILE__がCから記述されるので削った方がいい
	//"C:\\Users\\1011g\\source\\repos\\"を削るので23
	static int filePathCut;
	static void Init() {
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm now_tm;
		if (_localtime64_s(&now_tm, &now_c)) {
			// たぶんないとは思うが…
			throw("");
		}
		char c[256];
		strftime(c, 256, "log/log_%Y-%m-%d-%H-%M-%S.txt", &now_tm);
		LogTo.open(c,std::ios::out);
		if (!LogTo) {
			throw("");
		}
		filePathCut = 28;
		useFileLog = true;
		useDebugOutput = true;
	}
	static void Push(std::string toPush) {
		if (useFileLog) {
			LogTo << toPush << std::endl;
		}
		if (useDebugOutput) {
			OutputDebugStringA((toPush + "\n").c_str());
		}
	}
	static void _SetPrefix(int line, const char* func, const char* file) {
		logPrefix = std::string(&file[filePathCut]) + "/line" + std::to_string(line) + ",funcName:" + std::string(func) + ",Tick:" + std::to_string(Tick) + ",";
	}
	template<typename...Args>
	static void Append(std::format_string<Args...> fStr, Args&&... args) {
		std::string formatted = std::format(fStr, std::forward<Args>(args)...);
		Push(logPrefix + formatted);
	}
	static void Save() {
		LogTo.close();
	}
};