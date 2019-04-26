#include "Debug.h"

void Debug::logInfo(const std::string logMessage){
   if (m_currentLevel>=LOG_INFO && m_currentLevel !=LOG_DISABLED){
      std::cout << "INFO: " << logMessage << std::endl;
   }
}

void Debug::logInfo(const std::string prefix, const std::string logMessage){
   if (m_currentLevel>=LOG_INFO && m_currentLevel !=LOG_DISABLED){
      std::cout << "[" << prefix << ":INFO] " << logMessage << std::endl;
   }
}

void Debug::logInfo(const std::string prefix, const std::stringstream &logMessage){
   if (m_currentLevel>=LOG_INFO && m_currentLevel !=LOG_DISABLED){
      std::cout << "INFO: " << logMessage.str() << std::endl;
   }
}

void Debug::logInfo(const std::stringstream &logMessage){
   if (m_currentLevel>=LOG_INFO && m_currentLevel !=LOG_DISABLED){
      std::cout << "INFO: " << logMessage.str() << std::endl;
   }
}

void Debug::logError(const std::string logMessage){
   if (m_currentLevel>=LOG_ERROR && m_currentLevel !=LOG_DISABLED){
      std::cerr << "ERROR: " << logMessage << std::endl;
   }
}

void Debug::logError(const std::string prefix, const std::string logMessage){
   if (m_currentLevel>=LOG_ERROR && m_currentLevel !=LOG_DISABLED){
      std::cerr << "[" << prefix << ":ERROR] " << logMessage << std::endl;
   }
}

void Debug::logVerbose(const std::string logMessage){
   if (m_currentLevel>=LOG_VERBOSE && m_currentLevel !=LOG_DISABLED){
      std::cout << logMessage << std::endl;
   }
}

void Debug::logVerbose(const std::string prefix, const std::string logMessage){
   if (m_currentLevel>=LOG_VERBOSE && m_currentLevel !=LOG_DISABLED){
      std::cout << "[" << prefix << ":VERBOSE] " << logMessage << std::endl;
   }
}
