#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <mutex>

enum class LogLevel {DEBUG = 0, INFO = 1, WARN = 2, ERR = 3};


namespace LogColors {
	constexpr std::string_view RESET = "\033[0m";
	constexpr std::string_view RED = "\033[31m";
	constexpr std::string_view GREEN = "\033[32m";
	constexpr std::string_view YELLOW = "\033[33m";
	constexpr std::string_view BLUE = "\033[34m";
}

class Logger {
public:
	static void setLogLevel(LogLevel level);

	template<typename... Args>
	static void log(LogLevel level, const std::string& formatStr, Args&&... args);

	static void log(LogLevel level, const std::string& message);

	static void debug(const std::string& message);
	static void info(const std::string& message);
	static void warn(const std::string& message);
	static void error(const std::string& message);
	
	template<typename... Args> static void info(const std::string& formatStr, Args&&... args);
	template<typename... Args> static void debug(const std::string& formatStr, Args&&... args);
	template<typename... Args> static void warn(const std::string& formatStr, Args&&... args);
	template<typename... Args> static void error(const std::string& formatStr, Args&&... args);

private:
	static LogLevel currentLogLevel;
	static std::mutex logMutex;
	static std::string getTimestamp();
	static std::string logLevelToString(LogLevel level);
	static std::string getLogColor(LogLevel level);

	template<typename... Args>
	static std::string formatString(const std::string& formatStr, Args&&... args);

};

#include "Logger.inl"