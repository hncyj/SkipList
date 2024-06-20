#include "SkipList.h"

// SkipNode implemention

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



