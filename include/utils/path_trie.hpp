/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE 7982e23aed1dc8eda1fdc979fee617354ae998dc ]

#ifndef PATH_TRIE_HPP
#define PATH_TRIE_HPP
#include "utils/common.hpp"

#include <string>
#include <unordered_map>

class PathTrie
{
public:
    PathTrie()
        : root_(new Node())
    {
    }
    void Insert(const std::string& path);
    bool Search(const char* beg, const char* end, std::string& oas_path);
    bool Search(const char* beg, const char* end, std::string& oas_path, std::unordered_map<size_t, ParamRange>& param_idxs);
    ~PathTrie();

private:
    struct Node
    {
        std::string dir;
        bool is_param{false};
        int frag_idx{0};
        std::unordered_map<std::string, Node*> children;
    };

    void DeleteNode(Node* node);
    Node* root_;
};

#endif // PATH_TRIE_HPP
