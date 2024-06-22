#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <mutex>
#include <fstream>
#include <memory>
#include <random>
#include <exception>

#define STORE_FILE_PATH "/Users/yinjiechen/GitHub/SkipList/store/dumpfile"

std::string delimiter = ":";

// SkipList Node Template.
template <typename K, typename V>
class SkipNode {
private:
    K _key;
    V _value;
    int _level; // maximum level of SkipNode
    
public:
    std::vector<std::shared_ptr<SkipNode<K, V>>> forward; // SkipNode skip vector

    SkipNode() = default;
    SkipNode(K key, V val, int lvl);
    ~SkipNode() = default;

    K getKey() const;
    V getValue() const;
    int getLevel() const;

    void setKey(K key);
    void setValue(V val);
    void setLevel(int level);
};

// SkipList Class Template
template <typename K, typename V>
class SkipList {
private:
    int _max_level; // Maximum level of the skip list
    int _cur_level; // Current highest level of the skip list
    int _node_cnt; // SkipList current SkipNode numbers
    std::shared_ptr<SkipNode<K, V>> _header; // Pointer to head node
    mutable std::mutex mtx; // mutex, can be modified in const function
    mutable std::ifstream _file_reader;
    mutable std::ofstream _file_writer;

    bool is_valid_string(const std::string& str);
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);

public:
    SkipList(int max_level);
    ~SkipList();
    int get_random_level();
    std::shared_ptr<SkipNode<K, V>> create_node(K key, V val, int lvl);
    bool search_node(K key) const;
    void delete_node(K key);
    void insert_node(K key, V val);
    int get_list_size() const;
    void display_list() const;
    void dump_file();
    void load_file();
    void clear(std::shared_ptr<SkipNode<K, V>> header);
};

// SkipNode implement
template <typename K, typename V>
SkipNode<K, V>::SkipNode(K key, V val, int level) {
    _key = key;
    _value = val;
    _level = level;
    forward.resize(level + 1, nullptr);
}

template <typename K, typename V>
K SkipNode<K, V>::getKey() const {
    return _key;
}

template <typename K, typename V>
V SkipNode<K, V>::getValue() const {
    return _value;
}

template <typename K, typename V>
int SkipNode<K, V>::getLevel() const {
    return _level;
}

template <typename K, typename V>
void SkipNode<K, V>::setKey(K key) {
    _key = key;
}

template <typename K, typename V>
void SkipNode<K, V>::setValue(V val) {
    _value = val;
}

template <typename K, typename V>
void SkipNode<K, V>::setLevel(int level) {
    _level = level;
}

// SkipList implement
template <typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

template <typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {
  if (!is_valid_string(str)) {
    return;
  }
  *key = str.substr(0, str.find(delimiter));
  *value = str.substr(str.find(delimiter) + 1, str.length()); 
}

template <typename K, typename V>
SkipList<K, V>::SkipList(int max_level) {
  _max_level = max_level;
  _cur_level = 0;
  _node_cnt = 0;
  _header = std::make_shared<SkipNode<K, V>>(K{}, V{}, max_level);
}

template <typename K, typename V>
SkipList<K, V>::~SkipList() {
    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
}

template <typename K, typename V>
int SkipList<K, V>::get_random_level() {
    static std::default_random_engine generator;
    static std::geometric_distribution<int> distribution(0.5);
    int level = 0;
    while (level < _max_level && distribution(generator)) {
        level++;
    }
    return level;
}

template <typename K, typename V>
std::shared_ptr<SkipNode<K, V>> SkipList<K, V>::create_node(K key, V val, int lvl) {
    return std::make_shared<SkipNode<K, V>>(key, val, lvl);
}

template <typename K, typename V>
bool SkipList<K, V>::search_node(K key) const {
    // search node by key
    std::lock_guard<std::mutex> lock(mtx);
    auto cur = _header;
    for (int level = _cur_level; level >= 0; level--) {
        while (cur->forward[level] != nullptr && cur->forward[level]->getKey() < key) {
            cur = cur->forward[level];
        }
    }
    return cur != nullptr && cur->getKey() == key;
}

template <typename K, typename V>
void SkipList<K, V>::delete_node(K key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto cur = _header;
    std::vector<std::shared_ptr<SkipNode<K, V>>> update(_max_level + 1);
    for (int level = _cur_level; level >= 0; level--) {
        while (cur->forward[level] != nullptr && cur->forward[level]->getKey() < key) {
            cur = cur->forward[level];
        }
        update[level] = cur;
    }
    // delete node
    cur = cur->forward[0];
    if (!cur || cur->getKey() != key) {
        std::cerr << "Key: " << key << " does not exists!" << std::endl;
        return ;
    }
    for (int level = 0; level <= _cur_level; level++) {
        if (update[level]->forward[level] != cur) {
            break;
        }
        update[level]->forward[level] = cur->forward[level];
    }
    while (_cur_level > 0 && _header->forward[_cur_level] == nullptr) {
        --_cur_level;
    }
    _node_cnt--;
    std::cout << "Delete Key: " << key << " success!\n";
}

template <typename K, typename V>
void SkipList<K, V>::insert_node(K key, V val) {
    std::lock_guard<std::mutex> lock(mtx);
    auto cur = _header;
    std::vector<std::shared_ptr<SkipNode<K, V>>> update(_max_level + 1);
    // search insert position
    for (int level = _cur_level; level >= 0; level--) {
        while (cur->forward[level] != nullptr && cur->forward[level]->getKey() < key) {
            cur = cur->forward[level];
        }
        update[level] = cur;
    }
    cur = cur->forward[0];
    if (cur != nullptr && cur->getKey() == key) {
        std::cerr << "Key: " << key << " already exists!" << std::endl;
        return ;
    }
    // generate node
    int generate_level = get_random_level();
    if (generate_level > _cur_level) {
        for (int level = _cur_level + 1; level <= generate_level; level++) {
            update[level] = _header;
        }
        _cur_level = generate_level;
    }
    std::shared_ptr<SkipNode<K, V>> node = create_node(key, val, generate_level);
    // insert node
    for (int level = 0; level <= _cur_level; level++) {
        node->forward[level] = update[level]->forward[level];
        update[level]->forward[level] = node;
    }
    _node_cnt++;
    std::cout << "insert key: " << key << " success!\n";
}

template <typename K, typename V>
int SkipList<K, V>::get_list_size() const {
    return _node_cnt;
}

template <typename K, typename V>
void SkipList<K, V>::display_list() const {
    std::cout << "\n******* SkipList Display *******\n";
    for (int level = 0; level <= _cur_level; level++) {
        auto cur = _header->forward[level].get();
        std::cout << "level: " << level << ": ";
        while (cur != nullptr) {
            std::cout << "[" << cur->getKey() << ", " << cur->getValue() << "] -> "; 
            cur = cur->forward[level].get();
        }
        std::cout << std::endl;
    }
}

template <typename K, typename V>
void SkipList<K, V>::dump_file() {
    try {
        _file_writer.open(STORE_FILE_PATH);
        if (!_file_writer.is_open()) {
            throw std::runtime_error("Unable to open file for writing.\n");
        }
        auto cur = _header->forward[0].get();
        while (cur != nullptr) {
            _file_writer << "[" << cur->getKey() << ": " << cur->getValue() << "]\n";
            cur = cur->forward[0].get();
        }
        _file_writer.flush();
        _file_writer.close();
    } catch (std::exception& e) {
        std::cerr << "Exception occured: " << e.what() << std::endl;
    }
}

template <typename K, typename V>
void SkipList<K, V>::load_file() {
    _file_reader.open(STORE_FILE_PATH);
    std::cout << "\n*******load file *******\n";
    std::string key, val, line;
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, &key, &val);
        if (!key.empty() && !val.empty()) {
            continue;
        }
        insert_node(std::stoi(key), val);
    }
    _file_reader.close();
}

template <typename K, typename V>
void SkipList<K, V>::clear(std::shared_ptr<SkipNode<K, V>> header) {
    if (!header) {
        return ;
    }
    for (int level = 0; level <= header->getLevel(); level++) {
        clear(header->forward[level]);
    }
    header.reset();
    std::cout << "SkipList clear success!\n";
}

#endif