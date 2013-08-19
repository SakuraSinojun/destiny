

#pragma once


#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string>

namespace logging {

class LogMessage {
public:
    LogMessage(const char* file, const char* func, int line, int type);
    LogMessage();
    ~LogMessage();
    std::ostream&   stream() { return stream_; }
    class RawMessage {
    public:
        RawMessage(const char* file, const char* func, int line, int type)
            : file_(file)
            , func_(func)
            , text_("")
            , timestamp_("")
            , line_(line)
            , type_(type)
        {
        }
        std::string file_;
        std::string func_;
        std::string text_;
        std::string timestamp_;
        int         line_;
        int         type_;
    };
private:
    RawMessage          raw_message_;
    std::stringstream   stream_;
};

int log_trace(const char * file, const char * func, int line, int type, const char * fmt, ...);
}


