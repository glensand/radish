/* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/radish
 */

#pragma once

#include "serialization.h"
#include <cstdint>

#if 0
#include "easy/profiler.h"
#define __STR__(s) #s
#define NAMED_SCOPE(Name) EASY_BLOCK(__STR__(Name))
#else
#define __STR__(s) #s
#define NAMED_SCOPE(Name)
#endif

// Profiling macros for performance monitoring
// Only enabled when profiling is turned on via #if 1

// Defines the version and request type for all messages
#define RADISH_MAJOR_VERSION 0
#define RADISH_MINOR_VERSION 0 
#define RADISH_PATCH_VERSION 0

namespace radish {

    // Request types supported by the radish key-value service:
    // - get: Retrieves value for a given key
    // - set: Stores a key-value pair
    // - dump: Dumps entire key-value store (not implemented)

    enum class e_request : uint8_t {
        get, 
        set, 
        dump,
    };

    // Message header structure containing:
    // - Version information (Major.Minor.Patch)
    // - Request type (get/set/dump)
    // Used to identify and route incoming messages

    struct header_t final {
        int8_t MajorVersion;
        int8_t MinorVersion;
        int8_t Patch;
        e_request request;
    };

    namespace get {

        // Request message for retrieving a value by key
        struct request final {
            request(const std::string& in_key = "")
                : key(in_key) {}

            constexpr static header_t get_header() {
                return { RADISH_MAJOR_VERSION, RADISH_MINOR_VERSION, RADISH_PATCH_VERSION, e_request::get };
            }

            template<typename TStream>
            void write(TStream& stream) {
                NAMED_SCOPE(Radish_Get_Request_Write);
                write_impl(get_header(), stream);
                write_impl(key, stream);
            }

            template<typename TStream>
            void read(TStream& stream) {
                read_impl(key, stream);
            }

            const auto& get_key() const noexcept { return key; }
        private:
            std::string key;
        };

        struct response final {
            explicit response(std::string in_key = {}) 
                : key(std::move(in_key)) {}

            template<typename TStream>
            void write(TStream& stream, const std::vector<uint8_t>& in_value) {
                NAMED_SCOPE(Radish_Get_Response_Write);
                write_impl(key, stream);
                // notify client that value is present
                write_impl(!in_value.empty(), stream);
                if (!in_value.empty()) {
                    stream.write(in_value.data(), in_value.size());
                }
            }

            /// Reads a response from the stream and returns a tuple containing the value and success flag
            /// @tparam TStream The stream type to read from
            /// @tparam TValue The type of value being read
            /// @param stream The stream to read the response from
            /// @return A tuple containing the read value and a boolean indicating if the value was present
            template<typename TValue, typename TStream>
            std::tuple<TValue, bool> read(TStream& stream) {
                NAMED_SCOPE(Radish_Get_Response_Read);
                TValue value;
                read_impl(key, stream);
                bool ok;
                read_impl(ok, stream);
                // if value is not present, client will not read it
                if (ok) {
                    read_impl(value, stream);
                }
                return std::make_tuple(std::move(value), ok);
            }

        private:    
            std::string key;
        };

    }

    namespace set {

        struct request final {
            constexpr static header_t get_header() {
                return { RADISH_MAJOR_VERSION, RADISH_MINOR_VERSION, RADISH_PATCH_VERSION, e_request::set };
            }

            template<typename TValue, typename TStream>
            void write(TStream& stream, const std::string& key, TValue&& val) {
                NAMED_SCOPE(Radish_Set_Request_Write);
                write_impl(get_header(), stream);
                write_impl(key, stream);
                write_impl(val, stream);
            }

            template<typename TStream>
            std::tuple<std::string, std::vector<uint8_t>> read(TStream& stream) {
                NAMED_SCOPE(Radish_Set_Request_Write);
                std::string key;;
                read_impl(key, stream);
                auto payload_size = stream.count();
                std::vector<uint8_t> payload(payload_size);
                stream.read(payload.data(), payload_size);
                return std::make_tuple(std::move(key), std::move(payload));
            }
        };

        struct response final {
            template<typename TStream>
            void write(TStream& stream) {
                NAMED_SCOPE(Radish_Set_Response_Write);
                write_impl(true, stream);
            }

            template<typename TStream>
            void read(TStream& stream) {
                NAMED_SCOPE(Radish_Set_Response_Read);
                read_impl(m_ok, stream);
            }

            bool ok() const noexcept { return m_ok; }
        private:
            bool m_ok = false;
        };
    }

    namespace dump {

        struct request final {

        };

        struct response final {

        };

    }

    namespace stat {
        
    }
}
