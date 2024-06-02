#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <mutex>
#include <fstream>
#include <memory>

std::mutex mtx;

// SkipList Node Template.
template <typename K, typename V>
class SkipNode {
private:
    K _key;
    V _value;
    int _level;
    std::vector<std::unique_ptr<SkipNode<K, V>>> _forward;
    
public:
    // default constructor.
    SkipNode() = default;
    // constructor.
    SkipNode(K k, V v, int l): _key(k), _value(v), _level(l), _forward(l + 1) {};
    // default destructor.
    ~SkipNode() = default;
    // getter.
    K getKey() const {return _key;};
    V getValue() const {return _value;};
};

// SkipList Class Template
template <typename K, typename V>
class SkipList {
private:
    // Maximum level of the skip list
    int _max_level;
    // Current highest level of the skip list
    int _cur_level;
    // SkipList current element count
    int _element_cnt;
    // Pointer to head node
    std::unique_ptr<SkipNode<K, V>> _header;
    // File reader and writer
    std::istream _file_reader;
    std::ostream _file_writer;
    

    /**
     * @brief Get the key value from string object.
     * This function is used to parse a key-value pair from a string.
     * It is intended for use when loading data from a file,
     * where each line contains a key-value pair separated by a delimiter.
     * The function extracts and assigns the key and value to the provided pointers. 
     * @param str raw string
     * @param key parsed key
     * @param value parsed value
     */
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);

    void is_valid_string(const std::string& str);

public:
    /**
     * @brief Construct a new Skip List object.
     * Constructor that initializes a new SkipList instance. 
     * It sets up the maximum level (depth) of the list, 
     * the current level (initially set to 0), and creates the header node. 
     * This function also initializes counters and file stream objects if needed.
     * @param max_size
     */
    SkipList(int max_size);

    ~SkipList();

    /**
     * @brief Get the random level object
     * Generates a random level for a new node being inserted into the skip list.
     * This helps determine how many forward pointers a new node will have. 
     * The randomness ensures that the skip list maintains a balanced structure.
     * @return int 
     */
    int get_random_level();

    std::unique_ptr<SkipNode<K, V>> create_node(K k, V v, int level);

    bool search_element(K k) const;
    void delete_element(K k);
    int insert_node(K k, V v);

    void display_list() const;
    void dump_file();
    void load_file();
    void clear(std::unique_ptr<SkipNode<K, V>>& node);
    int size() const;
};

#endif