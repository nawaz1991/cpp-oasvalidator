#include "utils/path_trie.hpp"
#include "utils/common.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

class PathTrieTest: public ::testing::Test
{
protected:
    PathTrie trie_;

    // Utility function to simplify the test cases
    bool InsertAndSearch(const std::string& insert_path, const std::string& search_path, std::string& out_oas_path)
    {
        trie_.Insert(insert_path);
        return trie_.Search(search_path.c_str(), search_path.c_str() + search_path.size(), out_oas_path);
    }

    // Helper function for parameterized path searches
    bool InsertAndSearchParam(const std::string& insert_path, const std::string& search_path, std::string& out_oas_path, std::unordered_map<size_t, ParamRange>& param_idxs)
    {
        trie_.Insert(insert_path);
        return trie_.Search(search_path.c_str(), search_path.c_str() + search_path.size(), out_oas_path, param_idxs);
    }
};

// Test inserting and searching for a simple path
TEST_F(PathTrieTest, InsertAndSearchSimplePath)
{
    std::string oas_path;
    EXPECT_TRUE(InsertAndSearch("/api/data", "/api/data", oas_path));
    EXPECT_EQ(oas_path, "/api/data");
}

// Test searching for a path that does not exist
TEST_F(PathTrieTest, SearchNonExistentPath)
{
    std::string oas_path;
    EXPECT_FALSE(InsertAndSearch("/api/data", "/api/none", oas_path));
    EXPECT_TRUE(oas_path.empty());
}

// Test inserting and searching for a path with a parameter
TEST_F(PathTrieTest, InsertAndSearchParameterizedPath)
{
    std::string oas_path;
    std::unordered_map<size_t, ParamRange> param_idxs;
    EXPECT_TRUE(InsertAndSearchParam("/api/data/{id}", "/api/data/123", oas_path, param_idxs));
    EXPECT_EQ(oas_path, "/api/data/{id}");
    ASSERT_TRUE(param_idxs.find(3) != param_idxs.end());
    EXPECT_EQ(std::string(param_idxs[3].beg, param_idxs[3].end), "123");
}

// Test parameterized path with multiple parameters
TEST_F(PathTrieTest, InsertAndSearchMultiParamPath)
{
    std::string oas_path;
    std::unordered_map<size_t, ParamRange> param_idxs;
    trie_.Insert("/api/data/{id}/edit/{action}");
    EXPECT_TRUE(trie_.Search("/api/data/123/edit/update", &"/api/data/123/edit/update"[25], oas_path, param_idxs));
    EXPECT_EQ(oas_path, "/api/data/{id}/edit/{action}");
    ASSERT_EQ(param_idxs.size(), 2u);
    EXPECT_EQ(std::string(param_idxs[3].beg, param_idxs[3].end), "123");
    EXPECT_EQ(std::string(param_idxs[5].beg, param_idxs[5].end), "update");
}
