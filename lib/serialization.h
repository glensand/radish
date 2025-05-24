/* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/radish
 */

#pragma once

#include "hope_core/tuple/tuple_from_struct.h"
#include "hope_core/type_traits/user_defined_types.h"
#include "hope_core/type_traits/detector.h"

#include "hope-io/net/stream.h"

namespace radish {

    template<typename T, typename TStream>
    using write_function_t = decltype(radish_custom_write(std::declval<const T&>(), std::declval<TStream&>()));

    template<typename T, typename TStream>
    using read_function_t = decltype(radish_custom_read(std::declval<T&>(), std::declval<TStream&>()));

    // TODO:: do not split read and write function, we just can create load/save archive (like UE does it)
    template<typename TValue, typename TStream>
    void write_impl(const TValue& val, TStream& stream) {
        // custom serializer always have priority against other types
        if constexpr (hope::is_detected_v<write_function_t, TValue, TStream>) {
            static_assert(hope::is_detected_v<read_function_t, TValue, TStream>, 
                "radish/ both read and write functions need to be implemented");
            radish_custom_write(val, stream);
        } else if constexpr (
            std::is_floating_point_v<TValue> || 
                std::is_integral_v<TValue> || 
                    hope::is_string_v<TValue>) {
            stream.write(val);
        } else if constexpr (hope::is_vector_v<TValue>) {
            stream.write((uint16_t)val.size());
            for (auto&& e : val) {
                write_impl(e, stream);
            }
        } else if constexpr (hope::is_array_v<TValue>) {
            for (auto&& e : val) {
                write_impl(e, stream);
            }
        } else if constexpr (hope::is_user_defined_type_v<TValue>) {
            auto&& tuple = hope::tuple_from_struct(val, hope::field_policy::reference{});
            for_each(tuple, [&stream](const auto& field) {
                write_impl(field, stream);
            });
        }
    }

    template<typename TValue, typename TStream>
    void read_impl(TValue& val, TStream& stream) {
        // custom serializer always have priority against other types
        if constexpr (hope::is_detected_v<read_function_t, TValue, TStream>) {
            static_assert(hope::is_detected_v<write_function_t, TValue, TStream>, 
                "radish/ both read and write functions need to be implemented");
            radish_custom_read(val, stream);
        } else if constexpr (
            std::is_floating_point_v<TValue> || 
                std::is_integral_v<TValue> || 
                    hope::is_string_v<TValue>) {
            stream.read(val);
        } else if constexpr (hope::is_vector_v<TValue>) {
            const auto size = stream.template read<uint16_t>();
            val.resize(size);
            for (auto&& e : val) {
                read_impl(e, stream);
            }
        } else if constexpr (hope::is_array_v<TValue>) {
            for (auto&& e : val) {
                read_impl(e, stream);
            }
        } else if constexpr (hope::is_user_defined_type_v<TValue>) {
            auto&& tuple = hope::tuple_from_struct(val, hope::field_policy::reference{});
            for_each(tuple, [&stream](auto& field) {
                read_impl(field, stream);
            });
        }
    }
}