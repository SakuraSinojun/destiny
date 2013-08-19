

#include "logtracer_udp.h"
#include "log.h"
#include <iostream>
#include <sstream>
#include "tools/socket.h"
#include <stdlib.h>

namespace logging {

static tools::CSocket *  sockU = NULL;

LogTracerUdp::LogTracerUdp()
{
    if (!sockU)
        sockU = new tools::CSocket(tools::CSocket::eSocketType_UDP);
    sockU->Create();
    sockU->SetRemoteIP("127.0.0.1");
    sockU->SetRemotePort(10234);
}

LogTracerUdp::~LogTracerUdp()
{
}

void LogTracerUdp::Output(const LogMessage::RawMessage& message)
{
    std::stringstream    oss;
    switch (message.type_) {
    case LOGTYPE_DEBUG:
    case LOGTYPE_FATAL:
        oss << message.timestamp_ 
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
        oss << TERMC_YELLOW  << message.timestamp_ << " [STORY] " << message.text_ << TERMC_NONE; 
        break;
    case LOGTYPE_BATTLE:
        oss << TERMC_PINK << message.timestamp_ << " [BATTLE] " << message.text_ << TERMC_NONE; 
        break;
    case LOGTYPE_ITEM:
        oss << TERMC_GREEN << message.timestamp_ << " [ITEM] " << message.text_ << TERMC_NONE; 
        break;
    case LOGTYPE_INFO:
        oss << TERMC_BLUE << message.timestamp_ << " [INFO] "  << message.text_ << TERMC_NONE; 
        break;
    case LOGTYPE_OPER:
        oss << message.timestamp_ << " [OPERATION] " << message.text_;
        break;
    default:
        break;
    }
    if (sockU)
        sockU->Send(oss.str().c_str(), oss.str().length());
}


}

