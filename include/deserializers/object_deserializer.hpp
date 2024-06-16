/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef OBJECT_DESERIALIZER_HPP
#define OBJECT_DESERIALIZER_HPP

#include "deserializers/base_deserializer.hpp"
#include <unordered_map>

using ObjKTMap = std::unordered_map<std::string, PrimitiveType>; // Object Key-name to PrimitiveType

class ObjectDeserializer final: public BaseDeserializer
{
public:
    explicit ObjectDeserializer(const std::string& param_name, char start, bool skip_name, char kv_separator,
                                char vk_separator, bool is_deep_obj, const ObjKTMap& kt_map);

    std::string Deserialize(const char* beg, const char* const end) override;
    ~ObjectDeserializer() override = default;

private:
    const char kv_separator_; // key-value separator
    const char vk_separator_; // value-key separator
    const bool is_deep_obj_;
    const ObjKTMap kt_map_; // key-type map

    inline void DeserializeKey(const char*& cursor, const char* const end, const char terminator,
                               std::string& key) const
    {
        key.push_back('"');
        while (cursor < end && *cursor != terminator) {
            key.push_back(*cursor);
            ++cursor;
        }
        if (cursor < end && *cursor == terminator) {
            ++cursor;
        }
        key.push_back('"');
    }
};

#endif // OBJECT_DESERIALIZER_HPP
