/**
 * @file main.cpp
 * @author chenyinjie
 * @version 0.1
 * @date 2024-10-16
 */

#include "SkipList.h"

int main() {
    SkipList<int, std::string> skiplist(6);
    skiplist.InsertNode(0, "chen");
    skiplist.InsertNode(1, "yin");
    skiplist.InsertNode(2, "jie");

    std::cout << "SkipList size:" << skiplist.GetListSize() << std::endl;
    skiplist.DisplayList();

    skiplist.DumpFile();

    skiplist.SearchNode(0);
    skiplist.SearchNode(9);

    skiplist.DeleteNode(0);
    skiplist.DeleteNode(6);
    skiplist.DisplayList();
   
    return 0;
}