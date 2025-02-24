#pragma once

#include <sstream>
#include <vector>


template<typename T>
std::string toString(const T& value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

template<typename... Args>
std::string Logger::formatString(const std::string& formatStr, Args&&... args) {
    std::ostringstream stream;
    std::vector<std::string> argList = { toString(std::forward<Args>(args))... };

    size_t argIndex = 0;
    for (size_t i = 0; i < formatStr.size(); i++) {
        if (formatStr[i] == '{' && i + 1 < formatStr.size() && formatStr[i + 1] == '}') {
            if (argIndex < argList.size()) {
                stream << argList[argIndex++];
            }
            i++;
        }
        else {
            stream << formatStr[i];
        }
    }
    return stream.str();
}

template<typename... Args>
void Logger::log(LogLevel level, const std::string& formatStr, Args&&... args) {
    if (level < currentLogLevel) return;

    std::string logMessage = "[" + getTimestamp() + "] [" + logLevelToString(level) + "] " + formatString(formatStr, std::forward<Args>(args)...);

    std::cout << getLogColor(level) << logMessage << LogColors::RESET << std::endl;
}

template<typename... Args> void Logger::debug(const std::string& formatStr, Args&&... args) { log(LogLevel::DEBUG, formatStr, std::forward<Args>(args)...); }
template<typename... Args> void Logger::info(const std::string& formatStr, Args&&... args) { log(LogLevel::INFO, formatStr, std::forward<Args>(args)...); }
template<typename... Args> void Logger::warn(const std::string& formatStr, Args&&... args) { log(LogLevel::WARN, formatStr, std::forward<Args>(args)...); }
template<typename... Args> void Logger::error(const std::string& formatStr, Args&&... args) { log(LogLevel::ERR, formatStr, std::forward<Args>(args)...); }
