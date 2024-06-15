#include "utils/path_trie.hpp"
#include "utils/common.hpp"

PathTrie::PathTrie()
    : root_(new Node())
{
}

PathTrie::PathTrie(const PathTrie& other)
    : root_(nullptr)
{
    if (other.root_) {
        root_ = new Node(*other.root_);
        CopyNode(root_, other.root_);
    }
}

PathTrie& PathTrie::operator=(const PathTrie& other)
{
    if (this == &other)
        return *this;

    DeleteNode(root_);
    if (other.root_) {
        root_ = new Node(*other.root_);
        CopyNode(root_, other.root_);
    } else {
        root_ = nullptr;
    }

    return *this;
}

void PathTrie::CopyNode(Node*& thisNode, Node* otherNode)
{
    if (otherNode) {
        thisNode = new Node(*otherNode);
        for (const auto& child : otherNode->children) {
            CopyNode(thisNode->children[child.first], child.second);
        }
    }
}

void PathTrie::Insert(const std::string& path)
{
    auto* node = root_;
    const char* dir_start = path.data();
    const char* const path_end = dir_start + path.length();
    const char* dir_end;
    int fragment_index = 0;

    while (dir_start < path_end) {
        dir_end = Seek(dir_start, path_end, '/');
        std::string dir(dir_start, dir_end);
        fragment_index++;

        if (dir[0] == '{') {
            node->is_param = true;
            node->dir = dir;
        }

        if (node->children.find(dir) == node->children.end()) {
            auto* new_node = new Node();
            new_node->frag_idx = fragment_index; // Set the fragmentIndex for the new node
            node->children[dir] = new_node;
        }
        node = node->children[dir];

        dir_start = dir_end + 1; // skip '/'
    }
}

bool PathTrie::Search(const char* beg, const char* const end, std::string& oas_path)
{
    auto* node = root_;
    const char* dir_end;
    oas_path.clear();

    while (beg < end) {
        dir_end = Seek(beg, end, '/');
        std::string dir(beg, dir_end);
        auto it = node->children.find(dir);

        if (it == node->children.end()) {
            if (node->is_param) {
                oas_path += node->dir + "/";
                node = node->children.begin()->second;
            } else {
                oas_path.clear();
                return false;
            }
        } else {
            oas_path += dir + "/";
            node = it->second;
        }

        beg = dir_end + 1; // skip '/'
    }

    if (!oas_path.empty()) {
        oas_path.pop_back(); // remove trailing '/'
    }

    return true;
}

bool PathTrie::Search(const char* beg, const char* const end, std::string& oas_path,
                      std::unordered_map<size_t, ParamRange>& param_idxs)
{
    auto* node = root_;
    const char* dir_end;
    oas_path.clear();

    while (beg < end) {
        dir_end = Seek(beg, end, '/');
        std::string dir(beg, dir_end);
        auto it = node->children.find(dir);

        if (it == node->children.end()) {
            if (node->is_param) {
                oas_path += node->dir + "/";
                param_idxs.emplace(node->frag_idx, ParamRange{beg, dir_end});
                node = node->children.begin()->second;
            } else {
                oas_path.clear();
                return false;
            }
        } else {
            oas_path += dir + "/";
            node = it->second;
        }

        beg = dir_end + 1; // skip '/'
    }

    if (!oas_path.empty()) {
        oas_path.pop_back(); // remove trailing '/'
    }

    return true;
}

PathTrie::~PathTrie()
{
#ifndef LUA_OAS_VALIDATOR // LUA manages garbage collection itself
    DeleteNode(root_);
#endif // LUA_OAS_VALIDATOR
}

void PathTrie::DeleteNode(Node* node)
{
#ifndef LUA_OAS_VALIDATOR // LUA manages garbage collection itself
    for (auto& pair : node->children) {
        DeleteNode(pair.second);
    }
    delete node;
#endif // LUA_OAS_VALIDATOR
}
