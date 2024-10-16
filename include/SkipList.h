/**
 * @file SkipList.h
 * @author chenyinjie
 * @date 2024-10-16
 */

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <shared_mutex>
#include <fstream>
#include <memory>
#include <random>
#include <exception>
#include <filesystem>

constexpr const char* delimiter = ":";
const std::filesystem::path STORE_FILE_PATH = std::filesystem::current_path();

// SkipList Node Template.
template <typename K, typename V>
class SkipNode {
public:
    SkipNode() = default;
    SkipNode(K key, V val, int level);
    ~SkipNode() = default;

    K GetKey() const;
    V GetValue() const;
    int GetLevel() const;

    void SetKey(K key);
    void SetValue(V val);
    void SetLevel(int level);

    int GetMaxLevel() const;
    void SetForward(int level, std::shared_ptr<SkipNode<K, V>> node);
    std::shared_ptr<SkipNode<K, V>> GetForward(int level) const;

private:
    K key_;
    V value_;
    int max_level_; // maximum level of SkipNode
    std::vector<std::shared_ptr<SkipNode<K, V>>> forward_; // SkipNode skip vector
};

// SkipList Class Template
template <typename K, typename V>
class SkipList {
public:
    SkipList(int max_level);
    ~SkipList();

    int GenerateRandomLevel();
    int GetListSize() const;

    bool SearchNode(K key) const;
    void DeleteNode(K key);
    void InsertNode(K key, V val);

    void DumpFile();
    void LoadFile();

    void DisplayList() const;
    void Clear(std::shared_ptr<SkipNode<K, V>> header);

    std::shared_ptr<SkipNode<K, V>> CreateNode(K key, V val, int level);

private:
    int max_level_;                                     // Maximum level of the skip list
    int cur_level_;                                     // Current highest level of the skip list
    int node_cnt_;                                      // SkipList current SkipNode numbers
    std::shared_ptr<SkipNode<K, V>> header_;            // Pointer to head node
    mutable std::shared_mutex mtx_;                     // mutex, can be modified in const function
    mutable std::fstream file_stream_;                  // file read write steeam

    bool IsValidStr(const std::string& str);
    void GetKeyValFromStr(const std::string& str, std::string& key, std::string& value);
};

// SkipNode implement
template <typename K, typename V>
SkipNode<K, V>::SkipNode(K key, V val, int level): key_(key), value_(val), max_level_(max_level_) {
    forward_.resize(level + 1, nullptr);
}

template <typename K, typename V>
K SkipNode<K, V>::GetKey() const {
    return key_;
}

template <typename K, typename V>
V SkipNode<K, V>::GetValue() const {
    return value_;
}

template <typename K, typename V>
int SkipNode<K, V>::GetLevel() const {
    return max_level_;
}

template <typename K, typename V>
void SkipNode<K, V>::SetKey(K key) {
    key_ = key;
}

template <typename K, typename V>
void SkipNode<K, V>::SetValue(V val) {
    value_ = val;
}

template <typename K, typename V>
void SkipNode<K, V>::SetLevel(int level) {
    max_level_ = level;
}

template <typename K, typename V>
int SkipNode<K, V>::GetMaxLevel() const {
    return forward_.size() - 1;
}

template <typename K, typename V>
void SkipNode<K, V>::SetForward(int level, std::shared_ptr<SkipNode<K, V>> node) {
    if (level >= 0 && level < forward_.size()) {
        forward_[level] = node;
    }
}

template <typename K, typename V>
std::shared_ptr<SkipNode<K, V>> SkipNode<K, V>::GetForward(int level) const {
    if (level >= 0 && level < forward_.size()) {
        return forward_[level];
    }
    return nullptr;
}

// SkipList implement
template <typename K, typename V>
bool SkipList<K, V>::IsValidStr(const std::string& str) {
    if (str.empty()) return false;
    if (str.find(delimiter) == std::string::npos) return false;
    return true;
}

template <typename K, typename V>
void SkipList<K, V>::GetKeyValFromStr(const std::string& str, std::string& key, std::string& value) {
    // IsValidStr call before
    key = str.substr(0, str.find(delimiter));
    value = str.substr(str.find(delimiter) + 1, str.length());
}

template <typename K, typename V>
SkipList<K, V>::SkipList(int max_level) {
    cur_level_ = 0;
    node_cnt_ = 0;
    max_level_ = max_level;
    header_ = std::make_shared<SkipNode<K, V>>(K{}, V{}, max_level);
}

template <typename K, typename V>
SkipList<K, V>::~SkipList() {
    if (file_stream_.is_open()) file_stream_.close();
}

template <typename K, typename V>
int SkipList<K, V>::GenerateRandomLevel() {
    static std::default_random_engine generator;
    static std::bernoulli_distribution distribution(0.5);
    int level = 0;
    while (level < max_level_ && distribution(generator)) ++level;
    return level;
}

template <typename K, typename V>
std::shared_ptr<SkipNode<K, V>> SkipList<K, V>::CreateNode(K key, V val, int level) {
    return std::make_shared<SkipNode<K, V>>(key, val, level);
}

template <typename K, typename V>
bool SkipList<K, V>::SearchNode(K key) const {
    // search node by key
    std::shared_lock<std::shared_mutex> locker(mtx_);
    auto cur = header_;
    for (int level = cur_level_; level >= 0; --level) {
        while (cur->GetForward(level) != nullptr && cur->GetForward(level)->GetKey() < key) {
            cur = cur->GetForward(level);
        }
    }
    cur = cur->GetForward(0);
    return cur != nullptr && cur->GetKey() == key;
}

template <typename K, typename V>
void SkipList<K, V>::DeleteNode(K key) {
    std::unique_lock<std::shared_mutex> locker(mtx_);
    auto cur = header_;
    std::vector<std::shared_ptr<SkipNode<K, V>>> update(max_level_ + 1);
    for (int level = cur_level_; level >= 0; level--) {
        while (cur->GetForward(level) != nullptr && cur->GetForward(level)->GetKey() < key) {
            cur = cur->GetForward(level);
        }
        update[level] = cur;
    }
    // delete node
    cur = cur->GetForward(0);
    if (cur == nullptr || cur->GetKey() != key) {
        std::cerr << "Key: " << key << " does not exists!" << std::endl;
        return ;
    }

    for (int level = 0; level <= cur_level_; ++level) {
        if (update[level]->GetForward(level) != cur) {
            break;
        }
        update[level]->SetForward(level, cur->GetForward(level));
    }

    while (cur_level_ > 0 && header_->GetForward(cur_level_) == nullptr) {
        --cur_level_;
    }
    --node_cnt_;
    std::cout << "Delete Key: " << key << " success!\n";
}

template <typename K, typename V>
void SkipList<K, V>::InsertNode(K key, V val) {
    std::unique_lock<std::shared_mutex> locker(mtx_);
    auto cur = header_;
    std::vector<std::shared_ptr<SkipNode<K, V>>> update(max_level_ + 1);
    // search insert position
    for (int level = cur_level_; level >= 0; level--) {
        while (cur->GetForward(level) != nullptr && cur->GetForward(level)->GetKey() < key) {
            cur = cur->GetForward(level);
        }
        update[level] = cur;
    }

    cur = cur->GetForward(0);
    if (cur != nullptr && cur->GetKey() == key) {
        std::cerr << "Key: " << key << " already exists!" << std::endl;
        return ;
    }

    // create node
    int glevel = GenerateRandomLevel();
    if (glevel > cur_level_) {
        for (int level = cur_level_ + 1; level <= glevel; level++) {
            update[level] = header_;
        }
        cur_level_ = glevel;
    }
    std::shared_ptr<SkipNode<K, V>> node = CreateNode(key, val, glevel);

    // insert node
    for (int level = 0; level <= glevel; ++level) {
        node->SetForward(level, update[level]->GetForward(level));
        update[level]->SetForward(level, node);
    }
    ++node_cnt_;
    std::cout << "insert key: " << key << " success!\n";
}

template <typename K, typename V>
int SkipList<K, V>::GetListSize() const {
    return node_cnt_;
}

template <typename K, typename V>
void SkipList<K, V>::DisplayList() const {
    std::cout << "\n========== SkipList Display ==========\n";
    for (int level = 0; level <= cur_level_; ++level) {
        auto cur = header_->GetForward(level);
        std::cout << "LEVEL: " << level << "  ";
        while (cur != nullptr) {
            std::cout << "[" << cur->GetKey() << ", " << cur->GetValue() << "] -> "; 
            cur = cur->GetForward(level);
        }
        std::cout << "nullptr" << std::endl;
    }
}

template <typename K, typename V>
void SkipList<K, V>::DumpFile() {
    
    try {
        file_stream_.open(STORE_FILE_PATH, std::ios::out | std::ios::trunc);
        if (!file_stream_.is_open()) {
            throw std::runtime_error("Unable to open file for writing.\n");
        }
        auto cur = header_->GetForward(0);
        while (cur != nullptr) {
            file_stream_ << "[" << cur->GetKey() << delimiter << cur->GetValue() << "]\n";
            cur = cur->GetForward(0);
        }
        file_stream_.flush();
        file_stream_.close();
    } catch (std::exception& e) {
        std::cerr << "Exception occured: " << e.what() << std::endl;
    }
}

template <typename K, typename V>
void SkipList<K, V>::LoadFile() {
    try {
        std::unique_lock<std::shared_mutex> locker(mtx_);
        file_stream_.open(STORE_FILE_PATH, std::ios::in);
        if (!file_stream_.is_open()) {
            throw std::runtime_error("Unable to open file for reading.");
        }
        std::cout << "\n========== Load File ==========\n";
        std::string line;
        while (getline(file_stream_, line)) {
            std::string key_str, val_str;
            GetKeyValFromStr(line, key_str, val_str);
            if (key_str.empty() || val_str.empty()) continue;
            InsertNode(static_cast<K>(key_str), static_cast<V>(val_str));
        }
        file_stream_.close();
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }
}

template <typename K, typename V>
void SkipList<K, V>::Clear(std::shared_ptr<SkipNode<K, V>> header) {
    std::unique_lock<std::shared_mutex> locker(mtx_);
    header_->forward_.assign(max_level_ + 1, nullptr);
    cur_level_ = 0;
    node_cnt_ = 0;
    std::cout << "SkipList cleared successfully!\n";
}

#endif