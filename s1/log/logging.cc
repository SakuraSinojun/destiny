

#include "logging.h"
#include "log.h"
#include "log_dispatcher.h"
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <stdarg.h>


namespace logging {

LogMessage::LogMessage(const char* file, const char* func, int line, int type)
    : raw_message_(file, func, line, type)
{
}

LogMessage::LogMessage()
    : raw_message_("", "", 0, LOGTYPE_DEBUG)
{
}

LogMessage::~LogMessage()
{
    size_t  pos = raw_message_.file_.find_last_of("/\\");
    if (pos != std::string::npos) {
        raw_message_.file_ = raw_message_.file_.substr(pos + 1);
    }

    std::ostringstream   oss;
    oss << '[';
    time_t  t = time(NULL);
    struct tm* local_time = localtime(&t);
    oss << std::setfill('0')
        << std::setw(2) << local_time->tm_hour
        << ':'
        << std::setw(2) << local_time->tm_min
        << ':'
        << std::setw(2) << local_time->tm_sec
        << ']';
    raw_message_.timestamp_ = oss.str();

    std::string st = stream_.str();

    pos = st.find_last_not_of("\r\n ");
    if (pos != std::string::npos)
        st = st.substr(0, pos + 1);
    oss.str("");
    oss << st << std::endl;

    raw_message_.text_ = oss.str();

    LogDispatcher* dispatcher = LogDispatcher::GetInstance();

    dispatcher->Output(raw_message_);
    if (raw_message_.type_ == LOGTYPE_FATAL) {
        abort();
    }
}

// extern for CStyle..
int log_trace(const char * file, const char * func, int line, int type, const char * fmt, ...)
{
    va_list arglist;
    char    temp[1024];

    va_start(arglist, fmt);
    int     ret = ::vsnprintf(temp, sizeof(temp), fmt, arglist);
    va_end(arglist);
    if (ret <= 0) {
        ::printf("vsnprintf failed.\n");
    }
    else if (ret < (int)sizeof(temp)) {
        LogMessage(file, func, line, type).stream() << temp;
    }
    else {
        char * buffer = NULL;
        try {
            buffer = new char[ret + 1];
        } catch (std::bad_alloc) {
            buffer = NULL;
        }
        if(buffer) {
            va_start(arglist, fmt);
            ret = ::vsnprintf(buffer, ret + 1, fmt, arglist);
            va_end(arglist);
            LogMessage(file, func, line, type).stream() << buffer;
            delete [] buffer;
        }
    }
    return ret;
}


}


