#include "SkipList.h"

int main() {
    SkipList<int, std::string> skiplist(6);
    skiplist.insert_node(0, "chen");
    skiplist.insert_node(1, "yin");
    skiplist.insert_node(2, "jie");

    std::cout << "SkipList size:" << skiplist.get_list_size() << std::endl;
    skiplist.display_list();

    skiplist.dump_file();

    skiplist.search_node(0);
    skiplist.search_node(9);

    skiplist.delete_node(0);
    skiplist.delete_node(6);
    skiplist.display_list();
   
    return 0;
}