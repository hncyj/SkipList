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
    while (rand() % 2) {
        level++;
    }
    level = (level < _max_level) ? level : _max_level;
    return level;
}

template <typename K, typename V>
std::unique_ptr<SkipNode<K, V>> SkipList<K, V>::create_node(K key, V value, int level) {
    return std::unique_ptr<SkipNode<K, V>> node = std::make_unique<SkipNode<K, V>>(key, value, level);
}

template <typename K, typename V>
bool SkipList<K, V>::search_element(K key) const {
    std::lock_guard<std::mutex> lock(mtx);
    // search through list by key
    SkipNode<K, V>* cur = _header.get();
    for (int level = _cur_level; level >= 0; i--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
    }
    
    cur = cur->_forward[0].get();
    if (cur != nullptr && cur->getKey() == key) {
        return true;
    }
    return false;
}

template <typename K, typename V>
void SkipList<K, V>::delete_element(K key) {
    std::lock_guard<std::mutex> lock(mtx);
    // update used to record nodes which need to be modified.
    SkipNode<K, V>* cur = _header.get();
    std::vector<SkipNode<K, V>> update(_max_level + 1);

    for (int level = _cur_level; level >= 0; level--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
        update[level] = cur;
    }

    cur = cur->_forward[0].get();
    if (cur != nullptr && cur->getKey() == key) {
        for (int level = 0; level < _cur_level; level++) {
            if (update[level]->_forward[level].get() != cur) {
                break;
            }
            update[level]->_forward[level] = std::move(cur->_forward[level]);
        }
        while (_cur_level > 0 && _header->_forward[_cur_level] == nullptr) {
            _cur_level--;
        }
        _element_cnt--;
    }
}

template <typename K, typename V>
int SkipList<K, V>::insert_node(K key, V value) {
    std::lock_guard<std::mutex> lock(mtx);
    int l = get_random_level();
    std::unique_ptr<SkipNode<K, V>> node = create_node(key, value, l);

    SkipList<K, V> cur = _header.get();
    // record pre node
    std::vector<SkipNode<K, V>> update(_max_level + 1);
    for (int level = _cur_level; i >= 0; level--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
        update[level] = cur;
    }
    
    cur = cur->_forward[0];
    if (cur != nullptr && cur->getKey() == key) {
        return 1;
    }
    if (cur == nullptr || cur->getKey() != key) {
        
    }
    
}


