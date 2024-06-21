#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <mutex>
#include <fstream>
#include <memory>
#include <random>

#define STORE_FILE_PATH "/Users/yinjiechen/GitHub/SkipList/test/"

std::string delimiter = ":";

// SkipList Node Template.
template <typename K, typename V>
class SkipNode {
private:
    K _key;
    V _value;
    int _level; // maximum level of SkipNode
    std::vector<std::shared_ptr<SkipNode<K, V>>> _forward; // SkipNode skip vector
    
public:
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
    mutable static std::mutex mtx; // mutex, can be modified in const function
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
    _forward.resize(level + 1, nullptr);
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
    auto cur = _header.get();
    for (int level = _cur_level; level >= 0; level--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
    }
    cur = cur->_forward[0].get();
    return cur != nullptr && cur->getKey() == key;
}

template <typename K, typename V>
void SkipList<K, V>::delete_node(K key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto cur = _header.get();
    std::vector<SkipNode<K, V>*> update(_max_level + 1);
    for (int level = _cur_level; level >= 0; level--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
        update[level] = cur;
    }
    // delete node
    cur = cur->_forward[0].get();
    if (!cur || cur->getKey() != key) {
        std::cerr << "Key: " << key << " does not exists!" << std::endl;
        return ;
    }
    for (int level = 0; level <= _cur_level; level++) {
        if (update[level]->_forward[level] != cur) {
            break;
        }
        update[level]->_forward[level] = cur->_forward[level];
    }
    while (_cur_level > 0 && _header->_forward[_cur_level] == nullptr) {
        --_cur_level;
    }
    _node_cnt--;
}

template <typename K, typename V>
void SkipList<K, V>::insert_node(K key, V val) {
    std::lock_guard<std::mutex> lock(mtx);
    auto cur = _header.get();
    std::vector<SkipNode<K, V>*> update(_max_level + 1);
    // search insert position
    for (int level = _cur_level; level >= 0; level--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
        update[level] = cur;
    }
    cur = cur->_forward[0].get();
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
            node->_forward[level] = update[level]->_forward[level];
            update[level]->_forward[level] = node;
        }
        std::cout << "Insert success." << std::endl;
        _node_cnt++;
}

template <typename K, typename V>
int SkipList<K, V>::get_list_size() const {
    return _node_cnt;
}

template <typename K, typename V>
void SkipList<K, V>::display_list() const {
    std::cout << "\n*******SkipList*******\n";
    for (int level = 0; level <= _cur_level; level++) {
        auto cur = _header->_forward[level].get();
        std::cout << "level: " << level << std::endl;
        while (cur != nullptr) {
            std::cout << "[" << cur->getKey() << ", " << cur->getValue() << "] -> "; 
            cur = cur->_forward[level].get();
        }
        std::cout << std::endl;
    }
}

template <typename K, typename V>
void SkipList<K, V>::dump_file() {
    try {
        _file_writer.open(STORE_FILE_PATH);
        if (!_file_writer.is_open()) {
            thorw std::runtime_error("Unable to open file for writing.\n");
        }
        auto cur = _header->_forward[0].get();
        while (cur != nullptr) {
            _file_writer << "[" << cur->getKey() << ": " << cur->getValue() << "]\n";
            cur = cur->_forward[0].get();
        }
        _file_writer.flush();
        _file_writer.close();
    } catch (std::exception& e) {
        // handle the exception
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
        clear(header->_forward[level]);
    }
    header.reset();
}

#endif