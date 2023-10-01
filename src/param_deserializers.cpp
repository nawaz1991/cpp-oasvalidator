/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#include "param_validators.hpp"

#define INVALID_COMBINATION \
    [](const char*, const char* const, const std::unordered_map<std::string, ParamType>&) -> std::string { throw std::invalid_argument("Combination is not applicable"); }

const Deserializer ParamValidator::kDeserializers[STYLE_COUNT][TYPE_COUNT][2] = {
    // ===================== STYLE_SIMPLE =====================
    // STYLE_SIMPLE, TYPE_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) -> std::string {
        return {beg, end};
    },
    // STYLE_SIMPLE, TYPE_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) -> std::string {
        return {beg, end};
    },
    // STYLE_SIMPLE, TYPE_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 3);
        ret.push_back('"');
        ret.append(beg, end);
        ret.push_back('"');
        return ret;
    },
    // STYLE_SIMPLE, TYPE_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 3);
        ret.push_back('"');
        ret.append(beg, end);
        ret.push_back('"');
        return ret;
    },
    // STYLE_SIMPLE, TYPE_ARRAY_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 3);
        ret.push_back('[');
        ret.append(beg, end);
        ret.push_back(']');
        return ret;
    },
    // STYLE_SIMPLE, TYPE_ARRAY_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 3);
        ret.push_back('[');
        ret.append(beg, end);
        ret.push_back(']');
        return ret;
    },
    // STYLE_SIMPLE, TYPE_ARRAY_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        ret.push_back('[');
        const char* cursor = beg;
        const char* val_beg = nullptr;
        for (; cursor < end; cursor++) {
            ret.push_back('"');
            for (val_beg = cursor; ',' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_SIMPLE, TYPE_ARRAY_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        ret.push_back('[');
        const char* cursor = beg;
        const char* val_beg = nullptr;
        for (; cursor < end; cursor++) {
            ret.push_back('"');
            for (val_beg = cursor; ',' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_SIMPLE, TYPE_OBJECT, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        ret.push_back('{');
        const char* kv_beg = nullptr;
        for (const char* cursor = beg; cursor < end; cursor++) {
            for (kv_beg = cursor; *cursor != ',' && cursor < end; cursor++)
                ;
            ret.push_back('"'); // start key
            ret.append(kv_beg, cursor); // store key
            ret.append(R"(":)"); // end key
            auto mapper_itr = mapper.find({kv_beg, cursor});

            if (mapper.end() != mapper_itr) // found the key
            {
                switch (mapper_itr->second) {
                case TYPE_PRIMITIVE: // single value
                {
                    for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                        ;
                    ret.append(kv_beg, cursor);
                    break;
                }
                case TYPE_STRING: {
                    for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                        ;
                    ret.push_back('"');
                    ret.append(kv_beg, cursor);
                    ret.push_back('"');
                    break;
                }
                case TYPE_ARRAY_PRIMITIVE: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                            ;
                        mapper_itr = mapper.find({kv_beg, cursor});
                        if (mapper.end() != mapper_itr) {
                            cursor = kv_beg - 1;
                            break;
                        }
                        ret.append(kv_beg, cursor);
                        ret.push_back(',');
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_ARRAY_STRING: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                            ;
                        mapper_itr = mapper.find({kv_beg, cursor});
                        if (mapper.end() != mapper_itr) {
                            cursor = kv_beg - 1;
                            break;
                        }
                        ret.push_back('"');
                        ret.append(kv_beg, cursor);
                        ret.append(R"(",)");
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_OBJECT:
                case TYPE_COUNT:
                default:
                    break;
                }
                ret.push_back(','); // end of value
            }
        }
        if (',' == ret.back())
            ret.back() = '}'; // replace trailing ',' with '}'
        else
            ret.push_back('}');
        return ret;
    },
    // STYLE_SIMPLE, TYPE_OBJECT, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        ret.push_back('{');
        const char* kv_beg = nullptr;
        for (const char* cursor = beg; cursor < end; cursor++) {
            for (kv_beg = cursor; *cursor != '=' && cursor < end; cursor++)
                ;
            auto mapper_itr = mapper.find({kv_beg, cursor});
            ret.push_back('"'); // start key
            ret.append(kv_beg, cursor);
            ret.append(R"(":)"); // end key
            if (mapper.end() != mapper_itr) // found the key
            {
                switch (mapper_itr->second) {
                case TYPE_PRIMITIVE: // single value
                {
                    for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                        ;
                    ret.append(kv_beg, cursor);
                    break;
                }
                case TYPE_STRING: {
                    for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                        ;
                    ret.push_back('"');
                    ret.append(kv_beg, cursor);
                    ret.push_back('"');
                    break;
                }
                case TYPE_ARRAY_PRIMITIVE: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && *cursor != '=' && cursor < end; cursor++)
                            ;
                        if ('=' == *cursor) // new key
                        {
                            cursor = kv_beg - 1;
                            break;
                        }

                        ret.append(kv_beg, cursor);
                        ret.push_back(',');
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_ARRAY_STRING: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && *cursor != '=' && cursor < end; cursor++)
                            ;
                        if ('=' == *cursor) // new key
                        {
                            cursor = kv_beg - 1;
                            break;
                        }
                        ret.push_back('"');
                        ret.append(kv_beg, cursor);
                        ret.append(R"(",)");
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_OBJECT:
                case TYPE_COUNT:
                default:
                    break;
                }
                ret.push_back(','); // end of value
            }
        }
        if (',' == ret.back())
            ret.back() = '}'; // replace trailing ',' with '}'
        else
            ret.push_back('}');
        return ret;
    },
    // ===================== STYLE_LABEL =====================
    // STYLE_LABEL, TYPE_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) -> std::string {
        if (end > beg)
            return {beg + 1, end};
        return "";
    },
    // STYLE_LABEL, TYPE_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) -> std::string {
        if (end > beg)
            return {beg + 1, end};
        return "";
    },
    // STYLE_LABEL, TYPE_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 2);
        ret.push_back('"');
        if (end > beg)
            ret.append(beg + 1, end);
        ret.push_back('"');
        return ret;
    },
    // STYLE_LABEL, TYPE_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 2);
        ret.push_back('"');
        if (end > beg)
            ret.append(beg + 1, end);
        ret.push_back('"');
        return ret;
    },
    // STYLE_LABEL, TYPE_ARRAY_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 3);
        ret.push_back('[');
        const char* val_beg = nullptr;
        for (const char* cursor = beg + 1; cursor < end; cursor++) {
            for (val_beg = cursor; '.' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.push_back(',');
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_LABEL, TYPE_ARRAY_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 3);
        ret.push_back('[');
        const char* val_beg = nullptr;
        for (const char* cursor = beg + 1; cursor < end; cursor++) {
            for (val_beg = cursor; '.' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.push_back(',');
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_LABEL, TYPE_ARRAY_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 3);
        ret.push_back('[');
        const char* val_beg = nullptr;
        for (const char* cursor = beg + 1; cursor < end; cursor++) {
            ret.push_back('"');
            for (val_beg = cursor; '.' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_LABEL, TYPE_ARRAY_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 3);
        ret.push_back('[');
        const char* val_beg = nullptr;
        for (const char* cursor = beg + 1; cursor < end; cursor++) {
            ret.push_back('"');
            for (val_beg = cursor; '.' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_LABEL, TYPE_OBJECT, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        ret.push_back('{');
        const char* kv_beg = nullptr;
        // std::string kv;
        for (const char* cursor = beg + 1; cursor < end; cursor++) {
            for (kv_beg = cursor; *cursor != '.' && cursor < end; cursor++)
                ;
            auto mapper_itr = mapper.find({kv_beg, cursor});
            ret.push_back('"'); // start key
            ret.append(kv_beg, cursor);
            ret.append(R"(":)"); // end key
            if (mapper.end() != mapper_itr) // found the key
            {
                switch (mapper_itr->second) {
                case TYPE_PRIMITIVE: // single value
                {
                    for (kv_beg = ++cursor; *cursor != '.' && cursor < end; cursor++)
                        ;
                    ret.append(kv_beg, cursor);
                    break;
                }
                case TYPE_STRING: {
                    for (kv_beg = ++cursor; *cursor != '.' && cursor < end; cursor++)
                        ;
                    ret.push_back('"');
                    ret.append(kv_beg, cursor);
                    ret.push_back('"');
                    break;
                }
                case TYPE_ARRAY_PRIMITIVE: {
                    ret.push_back('[');
                    for (kv_beg = ++cursor; *cursor != '.' && cursor < end; cursor++)
                        ; // till next key or end
                    ret.append(kv_beg, cursor);
                    ret.push_back(']');
                    break;
                }
                case TYPE_ARRAY_STRING: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && *cursor != '.' && cursor < end; cursor++)
                            ;

                        ret.push_back('"');
                        ret.append(kv_beg, cursor);
                        ret.append(R"(",)");
                        if ('.' == *cursor) // next key found
                        {
                            break;
                        }
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_OBJECT:
                case TYPE_COUNT:
                default:
                    break;
                }
                ret.push_back(','); // end of value
            }
        }
        if (',' == ret.back())
            ret.back() = '}'; // replace trailing ',' with '}'
        else
            ret.push_back('}');
        return ret;
    },
    // STYLE_LABEL, TYPE_OBJECT, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        const char* kv_beg = nullptr;
        ret.push_back('{');
        for (const char* cursor = beg + 1; cursor < end; cursor++) {
            for (kv_beg = cursor; *cursor != '=' && cursor < end; cursor++)
                ;
            auto mapper_itr = mapper.find({kv_beg, cursor});
            ret.push_back('"'); // start key
            ret.append(kv_beg, cursor);
            ret.append(R"(":)"); // end key
            if (mapper.end() != mapper_itr) // found the key
            {
                switch (mapper_itr->second) {
                case TYPE_PRIMITIVE: // single value
                {
                    for (kv_beg = ++cursor; *cursor != '.' && cursor < end; cursor++)
                        ;
                    ret.append(kv_beg, cursor);
                    break;
                }
                case TYPE_STRING: {
                    for (kv_beg = ++cursor; *cursor != '.' && cursor < end; cursor++)
                        ;
                    ret.push_back('"');
                    ret.append(kv_beg, cursor);
                    ret.push_back('"');
                    break;
                }
                case TYPE_ARRAY_PRIMITIVE: {
                    ret.push_back('[');
                    for (kv_beg = ++cursor; *cursor != '.' && cursor < end; cursor++)
                        ; // till next key or end
                    ret.append(kv_beg, cursor);
                    ret.push_back(']');
                    break;
                }
                case TYPE_ARRAY_STRING: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && *cursor != '.' && cursor < end; cursor++)
                            ;

                        ret.push_back('"');
                        ret.append(kv_beg, cursor);
                        ret.append(R"(",)");
                        if ('.' == *cursor) // next key found
                        {
                            break;
                        }
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_OBJECT:
                case TYPE_COUNT:
                default:
                    break;
                }
                ret.push_back(','); // end of value
            }
        }
        if (',' == ret.back())
            ret.back() = '}'; // replace trailing ',' with '}'
        else
            ret.push_back('}');
        return ret;
    },
    // ===================== STYLE_MATRIX =====================
    // STYLE_MATRIX, TYPE_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) -> std::string {
        const char* cursor = beg + 1; // skip ';'
        for (; '=' != *(cursor - 1) && cursor < end; cursor++)
            ;
        return {cursor, end};
    },
    // STYLE_MATRIX, TYPE_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) -> std::string {
        const char* cursor = beg + 1; // skip ';'
        for (; '=' != *(cursor - 1) && cursor < end; cursor++)
            ;
        return {cursor, end};
    },
    // STYLE_MATRIX, TYPE_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg - 1);
        const char* cursor = beg + 1; // skip ';'
        for (; '=' != *(cursor - 1) && cursor < end; cursor++)
            ;
        ret.push_back('"');
        ret.append(cursor, end);
        ret.push_back('"');
        return ret;
    },
    // STYLE_MATRIX, TYPE_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg - 1);
        const char* cursor = beg + 1; // skip ';'
        for (; '=' != *(cursor - 1) && cursor < end; cursor++)
            ;
        ret.push_back('"');
        ret.append(cursor, end);
        ret.push_back('"');
        return ret;
    },
    // STYLE_MATRIX, TYPE_ARRAY_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg + 1; // skip ';'
        for (; '=' != *(cursor - 1) && cursor < end; cursor++)
            ;
        ret.push_back('[');
        ret.append(cursor, end);
        ret.push_back(']');
        return ret;
    },
    // STYLE_MATRIX, TYPE_ARRAY_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg + 1; // skip ';'
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (; '=' != *cursor && cursor < end; cursor++)
                ;
            for (val_beg = ++cursor; ';' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.push_back(',');
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_MATRIX, TYPE_ARRAY_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 20);
        const char* cursor = beg + 1; // skip ';'
        ret.push_back('[');
        const char* val_beg;
        for (; '=' != *cursor && cursor < end; cursor++)
            ;
        for (; cursor < end; beg++) {
            for (val_beg = ++cursor; ',' != *cursor && cursor < end; cursor++)
                ;
            ret.push_back('"');
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_MATRIX, TYPE_ARRAY_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg + 1; // skip ';'
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (; '=' != *cursor && cursor < end; cursor++)
                ;
            for (val_beg = ++cursor; ';' != *cursor && cursor < end; cursor++)
                ;
            ret.push_back('"');
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_MATRIX, TYPE_OBJECT, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        const char* kv_beg = nullptr;
        std::string kv;
        ret.push_back('{');
        const char* cursor = beg + 1; // skip ';'
        while ('=' != *cursor++ && cursor < end)
            ;
        for (; cursor < end; cursor++) {
            for (kv_beg = cursor; *cursor != ',' && cursor < end; cursor++)
                ;
            kv = {kv_beg, cursor};
            auto mapper_itr = mapper.find(kv);
            ret.push_back('"'); // start key
            ret.append(kv_beg, cursor);
            ret.append(R"(":)"); // end key
            if (mapper.end() != mapper_itr) // found the key
            {
                switch (mapper_itr->second) {
                case TYPE_PRIMITIVE: // single value
                {
                    for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                        ;
                    ret.append(kv_beg, cursor);
                    break;
                }
                case TYPE_STRING: {
                    for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                        ;
                    ret.push_back('"');
                    ret.append(kv_beg, cursor);
                    ret.push_back('"');
                    break;
                }
                case TYPE_ARRAY_PRIMITIVE: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                            ;
                        kv = {kv_beg, cursor};
                        mapper_itr = mapper.find(kv);
                        if (mapper.end() != mapper_itr) {
                            cursor = kv_beg - 1;
                            break;
                        }
                        ret.append(kv_beg, cursor);
                        ret.push_back(',');
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_ARRAY_STRING: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                            ;
                        kv = {kv_beg, cursor};
                        mapper_itr = mapper.find(kv);
                        if (mapper.end() != mapper_itr) {
                            cursor = kv_beg - 1;
                            break;
                        }
                        ret.push_back('"');
                        ret.append(kv_beg, cursor);
                        ret.append(R"(",)");
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_OBJECT:
                case TYPE_COUNT:
                default:
                    break;
                }
                ret.push_back(','); // end of value
            }
        }
        if (',' == ret.back())
            ret.back() = '}'; // replace trailing ',' with '}'
        else
            ret.push_back('}');
        return ret;
    },
    // STYLE_MATRIX, TYPE_OBJECT, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        const char* kv_beg = nullptr;
        std::string kv;
        ret.push_back('{');
        const char* cursor = beg + 1; // skip ';'

        for (; cursor < end; cursor++) {
            for (kv_beg = cursor; *cursor != '=' && cursor < end; cursor++)
                ;
            kv = {kv_beg, cursor};
            ret.push_back('"'); // start key
            ret.append(kv_beg, cursor);
            ret.append(R"(":)"); // end key
            auto mapper_itr = mapper.find(kv);
            if (mapper.end() != mapper_itr) // found the key
            {
                switch (mapper_itr->second) {
                case TYPE_PRIMITIVE: // single value
                {
                    for (kv_beg = ++cursor; *cursor != ';' && cursor < end; cursor++)
                        ;
                    ret.append(kv_beg, cursor);
                    break;
                }
                case TYPE_STRING: {
                    for (kv_beg = ++cursor; *cursor != ';' && cursor < end; cursor++)
                        ;
                    ret.push_back('"');
                    ret.append(kv_beg, cursor);
                    ret.push_back('"');
                    break;
                }
                case TYPE_ARRAY_PRIMITIVE: {
                    for (kv_beg = ++cursor; *cursor != ';' && cursor < end; cursor++)
                        ;
                    ret.push_back('[');
                    ret.append(kv_beg, cursor);
                    ret.push_back(']');
                    break;
                }
                case TYPE_ARRAY_STRING: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && ';' != *cursor && cursor < end; cursor++)
                            ;

                        ret.push_back('"');
                        ret.append(kv_beg, cursor);
                        ret.append(R"(",)");
                        if (';' == *cursor) // start of next key
                        {
                            break;
                        }
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_OBJECT:
                case TYPE_COUNT:
                default:
                    break;
                }
                ret.push_back(','); // end of value
            }
        }
        if (',' == ret.back())
            ret.back() = '}'; // replace trailing ',' with '}'
        else
            ret.push_back('}');
        return ret;
    },
    // ===================== STYLE_FORM =====================
    // STYLE_FORM, TYPE_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) -> std::string {
        const char* cursor = beg;
        while ('=' != *cursor++ && cursor < end)
            ;
        return {cursor, end};
    },
    // STYLE_FORM, TYPE_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) -> std::string {
        const char* cursor = beg;
        while ('=' != *cursor++ && cursor < end)
            ;
        return {cursor, end};
    },
    // STYLE_FORM, TYPE_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        const char* cursor = beg;
        while ('=' != *cursor++ && cursor < end)
            ;
        std::string ret;
        ret.reserve(end - cursor + 3);
        ret.push_back('"');
        ret.append(cursor, end);
        ret.push_back('"');
        return ret;
    },
    // STYLE_FORM, TYPE_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        const char* cursor = beg;
        while ('=' != *cursor++ && cursor < end)
            ;
        std::string ret;
        ret.reserve(end - cursor + 3);
        ret.push_back('"');
        ret.append(cursor, end);
        ret.push_back('"');
        return ret;
    },
    // STYLE_FORM, TYPE_ARRAY_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        const char* cursor = beg;
        while ('=' != *cursor++ && cursor < end)
            ;
        std::string ret;
        ret.reserve(end - cursor + 3);
        ret.push_back('[');
        ret.append(cursor, end);
        ret.push_back(']');
        return ret;
    },
    // STYLE_FORM, TYPE_ARRAY_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg + 1; // skip ';'
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (; '=' != *cursor && cursor < end; cursor++)
                ;
            for (val_beg = ++cursor; '&' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.push_back(',');
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_FORM, TYPE_ARRAY_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 20);
        ret.push_back('[');
        const char* cursor = beg;
        const char* val_beg;
        for (; '=' != *cursor && cursor < end; cursor++)
            ;
        for (; cursor < end; beg++) {
            for (val_beg = ++cursor; ',' != *cursor && cursor < end; cursor++)
                ;
            ret.push_back('"');
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_FORM, TYPE_ARRAY_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg;
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (; '=' != *cursor && cursor < end; cursor++)
                ;
            for (val_beg = ++cursor; '&' != *cursor && cursor < end; cursor++)
                ;
            ret.push_back('"');
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_FORM, TYPE_OBJECT, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        const char* kv_beg = nullptr;
        ret.push_back('{');
        const char* cursor = beg + 1; // skip ';'
        while ('=' != *cursor++ && cursor < end)
            ;
        for (; cursor < end; cursor++) {
            for (kv_beg = cursor; *cursor != ',' && cursor < end; cursor++)
                ;
            auto mapper_itr = mapper.find({kv_beg, cursor});
            ret.push_back('"'); // start key
            ret.append(kv_beg, cursor);
            ret.append(R"(":)"); // end key
            if (mapper.end() != mapper_itr) // found the key
            {
                switch (mapper_itr->second) {
                case TYPE_PRIMITIVE: // single value
                {
                    for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                        ;
                    ret.append(kv_beg, cursor);
                    break;
                }
                case TYPE_STRING: {
                    for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                        ;
                    ret.push_back('"');
                    ret.append(kv_beg, cursor);
                    ret.push_back('"');
                    break;
                }
                case TYPE_ARRAY_PRIMITIVE: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                            ;
                        mapper_itr = mapper.find({kv_beg, cursor});
                        if (mapper.end() != mapper_itr) {
                            cursor = kv_beg - 1;
                            break;
                        }
                        ret.append(kv_beg, cursor);
                        ret.push_back(',');
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_ARRAY_STRING: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != ',' && cursor < end; cursor++)
                            ;
                        mapper_itr = mapper.find({kv_beg, cursor});
                        if (mapper.end() != mapper_itr) {
                            cursor = kv_beg - 1;
                            break;
                        }
                        ret.push_back('"');
                        ret.append(kv_beg, cursor);
                        ret.append(R"(",)");
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_OBJECT:
                case TYPE_COUNT:
                default:
                    break;
                }
                ret.push_back(','); // end of value
            }
        }
        if (',' == ret.back())
            ret.back() = '}'; // replace trailing ',' with '}'
        else
            ret.push_back('}');
        return ret;
    },
    // STYLE_FORM, TYPE_OBJECT, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) {
        std::string ret;
        ret.reserve(end - beg + 100); // more is better
        const char* kv_beg = nullptr;
        std::string kv;
        std::string kv_inner;
        ret.push_back('{');
        const char* cursor = beg; // skip ';'
        if ('?' == *cursor)
            cursor++;

        for (; cursor < end; cursor++) {
            for (kv_beg = cursor; *cursor != '=' && cursor < end; cursor++)
                ;
            if (cursor == end)
                break; // Malformed input
            kv = {kv_beg, cursor};
            ret.push_back('"'); // start key
            ret.append(kv_beg, cursor);
            ret.append(R"(":)"); // end key
            auto mapper_itr = mapper.find(kv);
            if (mapper.end() != mapper_itr) // found the key
            {
                cursor++; // Skip '='
                if (cursor == end)
                    break; // Malformed input
                switch (mapper_itr->second) {
                case TYPE_PRIMITIVE: // single value
                {
                    for (kv_beg = cursor; *cursor != '&' && cursor < end; cursor++)
                        ;
                    ret.append(kv_beg, cursor);
                    break;
                }
                case TYPE_STRING: {
                    for (kv_beg = cursor; *cursor != '&' && cursor < end; cursor++)
                        ;
                    ret.push_back('"');
                    ret.append(kv_beg, cursor);
                    ret.push_back('"');
                    break;
                }
                case TYPE_ARRAY_PRIMITIVE: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = cursor; *cursor != '&' && cursor < end; cursor++)
                            ; // find value
                        ret.append(kv_beg, cursor);
                        ret.push_back(',');
                        for (kv_beg = ++cursor; *cursor != '=' && cursor < end; cursor++)
                            ; // find key
                        kv_inner = {kv_beg, cursor};
                        if (kv != kv_inner) { // end of array
                            cursor = kv_beg - 1;
                            break;
                        }
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_ARRAY_STRING: {
                    ret.push_back('[');
                    while (cursor < end) {
                        for (kv_beg = ++cursor; *cursor != '&' && cursor < end; cursor++)
                            ; // find value
                        if (cursor < end) {
                            ret.push_back('"');
                            ret.append(kv_beg, cursor);
                            ret.append(R"(",)");
                            for (kv_beg = ++cursor; *cursor != '=' && cursor < end; cursor++)
                                ; // find key
                            kv_inner = {kv_beg, cursor};
                            if (kv != kv_inner) { // end of array
                                cursor = kv_beg - 1;
                                break;
                            }
                        }
                    }
                    if (',' == ret.back())
                        ret.back() = ']'; // replace trailing ',' with ']'
                    else
                        ret.push_back(']');
                    break;
                }
                case TYPE_OBJECT: // TODO object in object
                case TYPE_COUNT:
                default:
                    break;
                }
                ret.push_back(','); // end of value
            }
        }
        if (',' == ret.back())
            ret.back() = '}'; // replace trailing ',' with '}'
        else
            ret.push_back('}');
        return ret;
    },
    // ===================== STYLE_SPACE_DELIM =====================
    // STYLE_SPACE_DELIM, TYPE_PRIMITIVE, false
    INVALID_COMBINATION,
    // STYLE_SPACE_DELIM, TYPE_PRIMITIVE, true
    INVALID_COMBINATION,
    // STYLE_SPACE_DELIM, TYPE_STRING, false
    INVALID_COMBINATION,
    // STYLE_SPACE_DELIM, TYPE_STRING, true
    INVALID_COMBINATION,
    // STYLE_SPACE_DELIM, TYPE_ARRAY_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg;
        ret.push_back('[');
        const char* val_beg;
        for (; '=' != *cursor && cursor < end; cursor++)
            ;
        if ('=' == *cursor)
            cursor++;
        while (cursor < end) {
            for (val_beg = cursor; '%' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.push_back(',');
            if (cursor + 3 < end && '%' == *cursor && '2' == *(cursor + 1) && '0' == *(cursor + 2)) {
                cursor += 3; // skip "%20"
            }
        }
        if (',' == ret.back()) {
            ret.back() = ']'; // replace trailing ',' with ']'
        } else {
            ret.push_back(']');
        }
        return ret;
    },
    // STYLE_SPACE_DELIM, TYPE_ARRAY_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg + 1; // skip ';'
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (; '=' != *cursor && cursor < end; cursor++) {
                // move till start of value
            }
            for (val_beg = ++cursor; '&' != *cursor && cursor < end; cursor++) {
            }
            ret.append(val_beg, cursor);
            ret.push_back(',');
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_SPACE_DELIM, TYPE_ARRAY_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 20);
        const char* cursor = beg + 1; // skip ';'
        ret.push_back('[');
        const char* val_beg;
        for (; '=' != *cursor && cursor < end; cursor++)
            ;
        if ('=' == *cursor)
            cursor++;
        for (; cursor < end; beg++) {
            for (val_beg = cursor; '%' != *cursor && cursor < end; cursor++)
                ;
            ret.push_back('"');
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
            if (cursor + 3 < end && '%' == *cursor && '2' == *(cursor + 1) && '0' == *(cursor + 2))
                cursor += 3; // skip "%20"
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_SPACE_DELIM, TYPE_ARRAY_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg;
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (; '=' != *cursor && cursor < end; cursor++)
                ;
            for (val_beg = ++cursor; '&' != *cursor && cursor < end; cursor++)
                ;
            ret.push_back('"');
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_SPACE_DELIM, TYPE_OBJECT, false
    INVALID_COMBINATION,
    // STYLE_SPACE_DELIM, TYPE_OBJECT, true
    INVALID_COMBINATION,
    // ===================== STYLE_PIPE_DELIM =====================
    // STYLE_PIPE_DELIM, TYPE_PRIMITIVE, false
    INVALID_COMBINATION,
    // STYLE_PIPE_DELIM, TYPE_PRIMITIVE, true
    INVALID_COMBINATION,
    // STYLE_PIPE_DELIM, TYPE_STRING, false
    INVALID_COMBINATION,
    // STYLE_PIPE_DELIM, TYPE_STRING, true
    INVALID_COMBINATION,
    // STYLE_PIPE_DELIM, TYPE_ARRAY_PRIMITIVE, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg;
        for (; '=' != *cursor && cursor < end; cursor++)
            ;
        if ('=' == *cursor)
            cursor++;
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (val_beg = cursor; '|' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor++);
            ret.push_back(',');
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_PIPE_DELIM, TYPE_ARRAY_PRIMITIVE, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg + 1; // skip ';'
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (; '=' != *cursor && cursor < end; cursor++)
                ;
            for (val_beg = ++cursor; '&' != *cursor && cursor < end; cursor++)
                ;
            ret.append(val_beg, cursor);
            ret.push_back(',');
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_PIPE_DELIM, TYPE_ARRAY_STRING, false
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg + 20);
        const char* cursor = beg + 1; // skip ';'
        ret.push_back('[');
        const char* val_beg;
        for (; '=' != *cursor && cursor < end; cursor++)
            ;
        for (; cursor < end; beg++) {
            for (val_beg = ++cursor; '|' != *cursor && cursor < end; cursor++)
                ;
            ret.push_back('"');
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            ret.back() = ']'; // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_PIPE_DELIM, TYPE_ARRAY_STRING, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>&) {
        std::string ret;
        ret.reserve(end - beg);
        const char* cursor = beg;
        ret.push_back('[');
        const char* val_beg;
        while (cursor < end) {
            for (; '=' != *cursor && cursor < end; cursor++)
                ;
            for (val_beg = ++cursor; '&' != *cursor && cursor < end; cursor++)
                ;
            ret.push_back('"');
            ret.append(val_beg, cursor);
            ret.append(R"(",)");
        }
        if (',' == ret.back())
            if (',' == ret.back())
                ret.back() = ']'; // replace trailing ',' with ']'
            else
                ret.push_back(']'); // replace trailing ',' with ']'
        else
            ret.push_back(']');
        return ret;
    },
    // STYLE_PIPE_DELIM, TYPE_OBJECT, false
    INVALID_COMBINATION,
    // STYLE_PIPE_DELIM, TYPE_OBJECT, true
    INVALID_COMBINATION,
    // ===================== STYLE_DEEP_OBJ =====================
    // STYLE_DEEP_OBJ, TYPE_PRIMITIVE, false
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_PRIMITIVE, true
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_STRING, false
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_STRING, true
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_ARRAY_PRIMITIVE, false
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_ARRAY_PRIMITIVE, true
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_ARRAY_STRING, false
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_ARRAY_STRING, true
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_OBJECT, false
    INVALID_COMBINATION,
    // STYLE_DEEP_OBJ, TYPE_OBJECT, true
    [](const char* beg, const char* const end, const std::unordered_map<std::string, ParamType>& mapper) -> std::string {
        return {beg, end};
    } // TODO
};
