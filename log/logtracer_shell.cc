

#include "logtracer_shell.h"
#include "log.h"
#include <iostream>

namespace logging {

void LogTracerShell::Output(const LogMessage::RawMessage& message)
{
    switch (message.type_) {
    case LOGTYPE_DEBUG:
    case LOGTYPE_FATAL:
        std::cout   << message.timestamp_ 
                    << " [DEBUG] "
                    << "[" 
                    << message.file_ 
                    << ':' 
                    << message.func_
                    << ':'
                    << message.line_
                    << "] "
                    << message.text_;
        break;
    case LOGTYPE_STORY:
        std::cout << TERMC_YELLOW  << message.timestamp_ << " [STORY] " << message.text_ << TERMC_NONE; 
        break;
    case LOGTYPE_BATTLE:
        std::cout << TERMC_PINK << message.timestamp_ << " [BATTLE] " << message.text_ << TERMC_NONE; 
        break;
    case LOGTYPE_ITEM:
        std::cout << TERMC_GREEN << message.timestamp_ << " [ITEM] " << message.text_ << TERMC_NONE; 
        break;
    case LOGTYPE_INFO:
        std::cout << TERMC_BLUE << message.timestamp_ << " [INFO] "  << message.text_ << TERMC_NONE; 
        break;
    case LOGTYPE_OPER:
        std::cout << message.timestamp_ << " [OPERATION] " << message.text_;
        break;
    default:
        break;
    }
}

}

