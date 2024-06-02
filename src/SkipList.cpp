#include "SkipList.h"

template <typename K, typename V>
SkipList<K, V>::SkipList(int max_level): 
    _max_level(max_level), _cur_level(0), _element_cnt(0) {
    // 创建一个头节点
    K key{};
    V value{};
    this->_header = std::make_unique<SkipNode<K, V>>(key, value, _max_level);
}

// template <typename K, typename V>
// SkipList<K, V>::~SkipList() {
//     if (_file_reader.is_open()) {
//         _file_reader.close();
//     }
//     if (_file_writer.is_open()) {
//         _file_writer.close();
//     }
//     // recusive delete
//     if (this->_header->_forward[0] != nullptr) {
//         this->clear(_header->forward[0]);
//     }
//     delete(this->_header);
// }

// template <typename K, typename V>
// void SkipList<K, V>::clear(SkipNode<K, V>* node) {
//     if (node->_forward[0] != nullptr) {
//         clear(node->_forward[0]);
//     }
//     delete(node);
// }

// template <typename K, typename V>
// int SkipList<K, V>::get_random_level() {
//     int level = 1;
//     while (rand() % 2) {
//         level++;
//     }
//     level = (level < _max_level) ? level : _max_level;
//     return level;
// }

// template <typename K, typename V>
// SkipNode<K, V>* SkipList<K, V>::create_node(K key, V value, int level) {
//     SkipNode<K, V>* node = new SkipNode<K, V>(key, value, level);
//     return node;
// }

// template <typename K, typename V>
// bool SkipList<K, V>::search_element(K key) const{
//     // search through list by key
//     SkipNode<K, V>* cur = this->_header;
//     // start from top level
//     for (int level = _cur_level; level >= 0; level--) {
//         while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
//             cur = cur->_forward[level];
//         }
//     }
//     cur = cur->_forward[0];
//     if (cur != nullptr && cur->getKey() == key) {
//         return true;
//     }
//     return false;
// }

// template <typename K, typename V>
// void SkipList<K, V>::delete_element(K key) {
//     // lock current thread
//     std::lock_guard<std::mutex> lock(mtx);

//     SkipNode<K, V>* cur = this->_header;
//     // record the node which will be modify
//     std::vector<SkipNode<K, V>*> update(_max_level + 1, nullptr);

//     for (int level = _cur_level; level >= 0; level--) {
//         while (cur->_forward[level] != nullptr && cur->_forward[level]->getKey() < key) {
//             cur = cur->_forward[level];
//         }
//         update[level] = cur;
//     }

//     cur = cur->_forward[0];
//     if (cur != nullptr && cur->getKey() == key) {
//         // delete node
//         for (int level = 0; level <= _cur_level; level++) {
//             if (update[level]->_forward[level] != cur) {
//                 break;
//             }
//             update[level]->_forward[level] = cur->_forward[level];
//         }
//         delete cur;
//         _element_cnt--;
        
//         while (_cur_level > 0 && this->_header->_forward[_cur_level] == nullptr) {
//             _cur_level--;
//         }
//     }
// }

 


