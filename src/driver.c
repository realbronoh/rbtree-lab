#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    rbtree *tree = new_rbtree();

    key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
    for (int i=0; i<sizeof(entries)/sizeof(entries[0]); i++){
        rbtree_insert(tree, entries[i]);
    }

    print_rbtree(tree->root);

    printf("===========================\n");

    rbtree_erase(tree, rbtree_find(tree, 2));
    print_rbtree(tree->root);
    printf("========================\n");

    rbtree_erase(tree, rbtree_find(tree, 156));
    print_rbtree(tree->root);
    printf("========================\n");

    rbtree_erase(tree, rbtree_find(tree, 24));
    print_rbtree(tree->root);
    printf("========================\n");

    rbtree_erase(tree, rbtree_find(tree, 10));
    print_rbtree(tree->root);
    printf("========================\n");
    printf("%d\n", tree->root->right->right->key);

    return 0;
}