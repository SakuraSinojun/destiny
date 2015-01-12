
#pragma once

#include "log_dispatcher.h"

namespace logging {

class LogTracerShell : public LogDispatcher::Tracer {
public:
    LogTracerShell() {}
    virtual ~LogTracerShell() {}
    virtual void Output(const LogMessage::RawMessage& message);
};

}


