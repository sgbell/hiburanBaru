#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#ifndef _DEBUG_H
#define _DEBUG_H

#define debug_info(x) Debug::logInfo(std::string((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__))+":"+std::to_string(__LINE__),x);
#define debug_error(x) Debug::logError(std::string((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__))+":"+std::to_string(__LINE__),x);
#define debug_verbose(x) Debug::logVerbose(std::string((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__))+":"+std::to_string(__LINE__),x);

class Debug {
  public:
    static void logInfo(const std::string prefix, const std::string logMessage);
    static void logInfo(const std::string prefix, const std::stringstream &logMessage);
    static void logError(const std::string prefix, const std::string logMessage);
    static void logVerbose(const std::string prefix, const std::string logMessage);
    static void logInfo(const std::string logMessage);
    static void logInfo(const std::stringstream &logMessage);
    static void logError(const std::string logMessage);
    static void logVerbose(const std::string logMessage);
    enum LogLevel{
       LOG_DISABLED,
       LOG_ERROR,
       LOG_INFO,
       LOG_VERBOSE
    };
    static LogLevel m_currentLevel;
    static bool m_debugOn;
};

#endif
