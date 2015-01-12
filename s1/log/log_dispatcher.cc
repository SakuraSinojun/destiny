

#include "log_dispatcher.h"
#include "logging.h"

namespace logging {

LogDispatcher* LogDispatcher::m_instance = NULL;
LogDispatcher* LogDispatcher::GetInstance(void)
{
    if (!m_instance) {
        m_instance = new LogDispatcher();
    }
    return m_instance;
}

LogDispatcher::LogDispatcher()
{
}

LogDispatcher::~LogDispatcher()
{
}

void LogDispatcher::AddTracer(Tracer* tracer)
{
    m_tracers.push_back(tracer);
}

void LogDispatcher::Output(const LogMessage::RawMessage& message)
{
    std::vector<Tracer*>::iterator iter;
    for (iter = m_tracers.begin();
        iter != m_tracers.end();
        iter++) {
        (*iter)->Output(message);
    }
}


}



