#include "SkipList.h"

// SkipNode implement

template <typename K, typename V>
SkipNode<K, V>::SkipNode(K key, V val, int level) {
    _key = key;
    _val = val;
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
SkipList<K, V>::SkipList(int max_level) {
    _max_level = max_level;
    _cur_level = 0;
    _node_cnt = 0;
    _header = std::make_shared<SkipNode<K, V>>(K{}, V{}, max_level);
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
std::shared_ptr<SkipNode<K, V>> SkipList<K, V>::create_node(K key, V val, int lvl) {
    return std::make_shared<SkipNode<K, V>>(key, val, level);
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
    return cur && cur->getKey() == key;
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
        return;
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
    --_node_cnt;
}

template <typename K, typename V>
void SkipList<K, V>::insert_node(K key, V val) {
    std::lock_guard<std::mutex> lock(mtx);
    if (search_node(key)) {
        std::cerr << "Key: " << key << " already exists!" << std::endl;
        return;
    }
    auto cur = _header.get();
    std::vector<SkipNode<K, V>*> update(_max_level + 1);
    // create node
    int lvl = get_random_level();
    std::shared_ptr<SkipNode<K, V>> node = std::make_shared<SkipNode<K, V>>(key, val, lvl);
    for (int level = _cur_level; level >= 0; level--) {
        while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
            cur = cur->_forward[level].get();
        }
        update[level] = cur;
    }
    cur = cur->_forward[0].get();
    


}


