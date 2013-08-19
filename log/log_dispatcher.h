

#pragma once

#include <string>
#include <vector>
#include "logging.h"

namespace logging {

class LogDispatcher {
public:
    ~LogDispatcher();
    static LogDispatcher* GetInstance(void);

    class Tracer {
    public:
        Tracer() {}
        virtual ~Tracer() {}
        virtual void Output(const LogMessage::RawMessage& message) = 0;
    };
    void AddTracer(Tracer* tracer);
    void Output(const LogMessage::RawMessage& message);

private:
    LogDispatcher();
    static LogDispatcher* m_instance;
    std::vector<Tracer*>    m_tracers;

};
}



