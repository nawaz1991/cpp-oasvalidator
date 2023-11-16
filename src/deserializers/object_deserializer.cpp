#include "deserializers/object_deserializer.hpp"

ObjectDeserializer::ObjectDeserializer(const std::string& param_name, char start, bool skip_name, char kv_separator, char vk_separator, bool is_deep_obj, const ObjKTMap& kt_map)
    : BaseDeserializer(param_name, start, skip_name)
    , kv_separator_(kv_separator)
    , vk_separator_(vk_separator)
    , is_deep_obj_(is_deep_obj)
    , kt_map_(kt_map)
{
}

std::string ObjectDeserializer::Deserialize(const char* beg, const char* const end)
{
    const char* cursor = beg;

    CheckNSkipStart(cursor);

    if (skip_name_ && !is_deep_obj_) {
        CheckNSkipName(cursor, end);
        CheckNSkipChar(cursor, end, '=');
    }

    CheckData(cursor, end);

    std::string ret;
    ret.reserve(static_cast<size_t>(end - beg + 128));

    ret.push_back('{');

    std::string key;
    key.reserve(128);

    if (is_deep_obj_) {
        while (cursor < end) {
            CheckNSkipName(cursor, end);
            CheckNSkipChar(cursor, end, '[');
            key.clear();
            DeserializeKey(cursor, end, ']', key);
            CheckNSkipChar(cursor, end, '=');
            try {
                auto param_type = kt_map_.at(key);
                ret.append(key);
                ret.push_back(':');
                switch (param_type) {
                case PrimitiveType::BOOLEAN:
                    DeserializeBoolean(cursor, end, ret);
                    break;

                case PrimitiveType::INTEGER:
                    DeserializeInteger(cursor, end, ret);
                    break;

                case PrimitiveType::NUMBER:
                    DeserializeNumber(cursor, end, ret);
                    break;

                case PrimitiveType::STRING:
                    DeserializeString(cursor, end, vk_separator_, ret);
                    break;
                }
                if (*cursor == vk_separator_) {
                    ret.push_back(',');
                    ++cursor;
                }
            } catch (const std::out_of_range&) {
                throw DeserializationException("Invalid format for '" + param_name_ + "'");
            }
        }
    } else {
        while (cursor < end) {
            key.clear();
            DeserializeKey(cursor, end, kv_separator_, key);
            try {
                auto param_type = kt_map_.at(key);
                ret.append(key);
                ret.push_back(':');
                switch (param_type) {
                case PrimitiveType::BOOLEAN:
                    DeserializeBoolean(cursor, end, ret);
                    break;

                case PrimitiveType::INTEGER:
                    DeserializeInteger(cursor, end, ret);
                    break;

                case PrimitiveType::NUMBER:
                    DeserializeNumber(cursor, end, ret);
                    break;

                case PrimitiveType::STRING:
                    DeserializeString(cursor, end, vk_separator_, ret);
                    break;
                }
                if (*cursor == vk_separator_) {
                    ret.push_back(',');
                    ++cursor;
                }
            } catch (const std::out_of_range&) {
                throw DeserializationException("Invalid format for '" + param_name_ + "'");
            }
        }
    }

    CheckEnd(cursor, end);

    ret.push_back('}');
    return ret;
}
