#pragma once 

#include "hope_logger/log_helper.h"
#include "hope_thread/runtime/threadpool.h"
#include "easy/profiler.h"

#undef ERROR

#define INFO hope::log::log_level::info
#define ERROR hope::log::log_level::error
#define LOG(Prior) HOPE_INTERIOR_LOG(Prior, *glob_logger)
#define CLOG(Prior, Cnd) if ((Cnd)) HOPE_INTERIOR_LOG(Prior, *glob_logger)

extern hope::log::logger* glob_logger;

#define __STR__(s) #s
#define THREAD_SCOPE(ThreadName) EASY_THREAD_SCOPE(__STR__(ThreadName));
#define NAMED_SCOPE(Name) EASY_BLOCK(__STR__(Name))