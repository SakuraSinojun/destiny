
#pragma once


#define LOGTYPE_DEBUG   0
#define LOGTYPE_OPER    1
#define LOGTYPE_STORY   11
#define LOGTYPE_BATTLE  12
#define LOGTYPE_INFO    13
#define LOGTYPE_ITEM    14
#define LOGTYPE_FATAL   99

#define TERMC_BLACK   "\033[1;30;30m"
#define TERMC_RED     "\033[1;31;31m"
#define TERMC_GREEN   "\033[1;32;32m"
#define TERMC_YELLOW  "\033[1;33;33m"
#define TERMC_BLUE    "\033[1;34;34m"
#define TERMC_PINK    "\033[1;35;35m"
#define TERMC_LGREEN  "\033[1;36;36m"
#define TERMC_WHITE   "\033[1;37;37m"
#define TERMC_NONE    "\033[0m"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "logging.h"

namespace logging {

class LogMessageVoidify {
public:
    LogMessageVoidify() {}
    void operator&(std::ostream&) {}
};

class ScopeTracer {
public:
    ScopeTracer(const char* func, const char* name, int line) : name_(name), func_(func), line_(line) {
        ::logging::LogMessage().stream() << TERMC_RED << "Into scope: " << name << " (" << func << ":" << line << ")" << TERMC_NONE;
    }
    ~ScopeTracer() {
        ::logging::LogMessage().stream() << TERMC_GREEN << "Leave scope: " << name_ << " (" << func_ << ":" << line_ << ")" << TERMC_NONE;
    }
private:
    const char * name_;
    const char * func_;
    int          line_;
};

}

#if defined (__GNUC__)
#  define __FUNC__     ((const char*) (__PRETTY_FUNCTION__))
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 19901L
#  define __FUNC__     ((const char*) (__func__))
#else
#  define __FUNC__     ((const char*) (__FUNCTION__))
#endif

#define LOG(type) ::logging::LogMessageVoidify() & ::logging::LogMessage(__FILE__, __FUNC__, __LINE__, LOGTYPE_ ## type).stream()
#define Debug() ::logging::LogMessageVoidify() & ::logging::LogMessage(__FILE__, __FUNC__, __LINE__, LOGTYPE_DEBUG).stream()
#define Fatal() ::logging::LogMessageVoidify() & ::logging::LogMessage(__FILE__, __FUNC__, __LINE__, LOGTYPE_FATAL).stream()
#define LStory() ::logging::LogMessageVoidify() & ::logging::LogMessage(__FILE__, __FUNC__, __LINE__, LOGTYPE_STORY).stream()
#define LBattle() ::logging::LogMessageVoidify() & ::logging::LogMessage(__FILE__, __FUNC__, __LINE__, LOGTYPE_BATTLE).stream()
#define LItem() ::logging::LogMessageVoidify() & ::logging::LogMessage(__FILE__, __FUNC__, __LINE__, LOGTYPE_ITEM).stream()
#define LInfo() ::logging::LogMessageVoidify() & ::logging::LogMessage(__FILE__, __FUNC__, __LINE__, LOGTYPE_INFO).stream()
#define DCHECK(condition) if (!(condition)) Fatal() << "Check fail: " << #condition

#define PERROR(str)  Debug() << str << ": " << strerror(errno)
#define ScopeTrace(x)   logging::ScopeTracer x ## __LINE__(__FUNC__, #x, __LINE__)

#define RUN_HERE() LOG(DEBUG) << TERMC_YELLOW << "Run here! " << TERMC_NONE

#define PRINTF(type, X, ...) ::logging::log_trace(__FILE__, __FUNC__, __LINE__, LOGTYPE_ ## type, X, ##__VA_ARGS__)



