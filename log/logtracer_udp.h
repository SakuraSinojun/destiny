
#pragma once

#include "log_dispatcher.h"

namespace logging {

class LogTracerUdp : public LogDispatcher::Tracer {
public:
    LogTracerUdp();
    virtual ~LogTracerUdp();
    virtual void Output(const LogMessage::RawMessage& message);
};

}


