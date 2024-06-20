#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <mutex>
#include <fstream>
#include <memory>

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
    int _node_cnt; // SkipList current SkipNode count
    std::shared_ptr<SkipNode<K, V>> _header; // Pointer to head node
    mutable std::mutex mtx; // mutex, can be modified in const function
    std::ifstream _file_reader;
    std::ofstream _file_writer;

    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    void is_valid_string(const std::string& str);

public:
    SkipList(int max_level);
    ~SkipList();
    int get_random_level();
    std::shared_ptr<SkipNode<K, V>> create_node(K key, V val);
    bool search_node(K key) const;
    void delete_node(K key);
    int insert_node(K key, V val);
    int size() const;
    void display_list() const;
    void dump_file();
    void load_file();
    void clear(std::shared_ptr<SkipNode<K, V>> node);
};

#endif