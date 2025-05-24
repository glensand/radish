#include "stream_wrapper.h"
#include "foundation.h"
#include "hope_logger/logger.h"
#include "hope_logger/ostream.h"

hope::log::logger* glob_logger = nullptr;
    
namespace radish {
    void init(){
        // TODO:: introduce console arguments, log parameters etc
        glob_logger = new hope::log::logger(
            *hope::log::create_multy_stream({
                hope::log::create_file_stream("Radish.txt")
                // hope::log::create_console_stream() put it under console variable
            })
        );
    }
}