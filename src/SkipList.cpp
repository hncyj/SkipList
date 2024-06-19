#include "SkipList.h"

template <typename K, typename V>
SkipList<K, V>::~SkipList() {
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    if (_file_writer.is_open()) {
        _file_writer.close();
    }
}

template <typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int level = 1;
    while (rand() % 2 && level < _max_level) {
        level++;
    }
    return level;
}

template <typename K, typename V>
std::unique_ptr<SkipNode<K, V>> SkipList<K, V>::create_node(K key, V value, int level) {
    return std::make_unique<SkipNode<K, V>>(key, value, level);
}

template <typename K, typename V>
bool SkipList<K, V>::search_node(K key) const {
    std::lock_guard<std::mutex> lock(mtx);
    // search by key
    SkipNode<K, V>* cur = _header.get();
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
    std::vector<SkipNode<K, V>*> update(_max_level + 1);
    SkipNode<K, V>* cur = _header.get();
    // find out nodes
    for (int level = _cur_level; level >= 0; level--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
        update[level] = cur;
    }
    // delete nodes
    cur = cur->_forward[0].get();
    if (cur != nullptr && cur->getKey() == key) {
        for (int level = 0; level <= _cur_level; level++) {
            // higher level has far more skip distance
            // so we can break earlyer
            if (update[level]->_forward[level].get() != cur) {
                break;
            }
            update[level]->_forward[level] = std::move(cur->_forward[level]);
        }
        // update _cur_level
        while (_cur_level > 0 && _header->_forward[_cur_level] == nullptr) {
            _cur_level--;
        }
        --_node_cnt;
    }
}


template <typename K, typename V>
int SkipList<K, V>::insert_node(K key, V val) {
    std::lock_guard<std::mutex> lock(mtx);
    // search key pos
    std::vector<SkipNode<K, V>*> update(_max_level + 1);
    SkipNode<K, V>* cur = _header.get();

    for (int level = _cur_level; level >= 0; level--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
        update[level] = cur;
    }

    // insert node
    cur = cur->_forward[0].get();
    if (cur != nullptr && cur->getKey() == key) {
        std::cout << "Key: " << key << " already exists!" << std::endl;
        return 1;
    }
    
    if (cur == nullptr || cur->getKey() != key) {
        int lvl = get_random_level();
        // if lvl greater than _cur_level, modify update[]
        if (lvl > _cur_level) {
            for (int level = _cur_level + 1; i <= lvl; level++) {
                update[level] = _header; 
            }
            _cur_level = lvl;
        }
        std::unique_ptr<SkipNode<K, V>> node_to_be_insert = create_node(key, val, lvl);

        for (int level = 0; level <= lvl ; level++) {
            node_to_be_insert->_forward[level] = std::move(update[level]->_forward[level]);
            update[level]->_forward[level] = node_to_be_insert.get();
        }
        std::cout << "Insert success. Key: " << key << ", val: " << val << std::endl;
        ++_node_cnt;
    }
    return 0;
}


