#include "Logger.h"

LogLevel Logger::currentLogLevel = LogLevel::INFO;
std::mutex Logger::logMutex;

void Logger::setLogLevel(LogLevel level) {
	currentLogLevel = level;
}

std::string Logger::getTimestamp() {
	std::time_t now = std::time(nullptr);
	std::tm timeStruct;

#ifdef _WIN32
	localtime_s(&timeStruct, &now);
#else
	localtime_r(&now, &timeStruct);
#endif

	std::ostringstream oss;
	oss << std::put_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) {
	switch (level){
		case LogLevel::INFO: return "INFO";
		case LogLevel::DEBUG: return "DEBUG";
		case LogLevel::WARN: return "WARN";
		case LogLevel::ERR: return "ERROR";
		default: return "UNKNOW";
	}
}

std::string Logger::getLogColor(LogLevel level) {
	switch (level)
	{
		case LogLevel::DEBUG: return std::string(LogColors::BLUE);
		case LogLevel::INFO: return std::string(LogColors::GREEN);
		case LogLevel::WARN: return std::string(LogColors::YELLOW);
		case LogLevel::ERR: return std::string(LogColors::RED);
		default: return std::string(LogColors::RESET);
	}
}


void Logger::log(LogLevel level, const std::string& message) {
	if (level < currentLogLevel) return;

	std::lock_guard<std::mutex> lock(logMutex);
	std::string logMessage = "[" + getTimestamp() + "] [" + logLevelToString(level) + "] " + message;
	std::cout << getLogColor(level) << logMessage << LogColors::RESET << std::endl;
}


void Logger::debug(const std::string& message) { log(LogLevel::DEBUG, message); }
void Logger::info(const std::string& message) { log(LogLevel::INFO, message); }
void Logger::warn(const std::string& message) { log(LogLevel::WARN, message); }
void Logger::error(const std::string& message) { log(LogLevel::ERR, message); }
