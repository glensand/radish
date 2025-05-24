#pragma once 

#include "hope_logger/log_helper.h"

#undef ERROR
#define INFO hope::log::log_level::info
#define ERROR hope::log::log_level::error
#define LOG(Prior) HOPE_INTERIOR_LOG(Prior, *glob_logger)
#define CLOG(Prior, Cnd) if ((Cnd)) HOPE_INTERIOR_LOG(Prior, *glob_logger)

extern hope::log::logger* glob_logger;

#ifdef WITH_PROFILER
#include "easy/profiler.h"
#define __STR__(s) #s
#define THREAD_SCOPE(ThreadName) EASY_THREAD_SCOPE(__STR__(ThreadName));
#define NAMED_SCOPE(Name) EASY_BLOCK(__STR__(Name))
#define PROFILER_ENABLE EASY_PROFILER_ENABLE
#else
#define THREAD_SCOPE(ThreadName)
#define NAMED_SCOPE(Name)
#define PROFILER_ENABLE
#endif

namespace radish {
    void init();
}