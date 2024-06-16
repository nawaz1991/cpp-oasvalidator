/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#ifndef PATH_TRIE_HPP
#define PATH_TRIE_HPP

#include "utils/common.hpp"
#include <string>
#include <unordered_map>

class PathTrie
{
public:
    PathTrie();
    PathTrie(const PathTrie& other); // Copy constructor
    PathTrie& operator=(const PathTrie& other); // Copy assignment operator
    ~PathTrie();

    void Insert(const std::string& path);
    bool Search(const char* beg, const char* end, std::string& oas_path);
    bool Search(const char* beg, const char* end, std::string& oas_path,
                std::unordered_map<size_t, ParamRange>& param_idxs);

private:
    struct Node
    {
        Node() = default;
        std::string dir{};
        bool is_param = false;
        int frag_idx = 0;
        std::unordered_map<std::string, Node*> children{};
    };

    void DeleteNode(Node* node);
    void CopyNode(Node*& this_node, Node* other_node);

    Node* root_;
};

#endif // PATH_TRIE_HPP
