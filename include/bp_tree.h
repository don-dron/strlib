#pragma once

#include <stdlib.h>

/**
 * Basic node for item. You must derivative this struct for insert.
 *
 * For example:
 *
 * struct test_node {
 *     struct bp_tree_node basic;
 *     int value;
 * }
 *
 * bp_tree_insert(map, &test_node.basic);
 */
struct bp_tree_node
{
};

/**
 * Inner node of B+ tree.
 */
struct bp_tree_struct_node;

struct bp_tree_struct_node
{
    int leaf;
    int size;

    struct bp_tree_struct_node *left;
    struct bp_tree_struct_node *right;
    struct bp_tree_struct_node *parent;
    struct bp_tree_node **keys;
};

struct bp_tree_non_leaf_node
{
    struct bp_tree_struct_node core;
    struct bp_tree_struct_node **children;
};

struct bp_tree_leaf_node
{
    struct bp_tree_struct_node core;
};

struct bp_tree_batch
{
    struct bp_tree_node **nodes;
    int size;
};

struct bp_tree
{
    struct bp_tree_struct_node *root;
    int degree;
    int size;
    int (*comparator)(struct bp_tree_node *, struct bp_tree_node *);

    int split_leaf;
    int split_non_leaf;

    int rebalance_right_leaf;
    int rebalance_right_non_leaf;

    int rebalance_left_leaf;
    int rebalance_left_non_leaf;

    int merge_left_leaf;
    int merge_left_non_leaf;

    int merge_right_leaf;
    int merge_right_non_leaf;
};

/**
 * Init B+ tree and add first leaf node - root.
 * Degree must be more or equals 4.
 */
int bp_tree_init(struct bp_tree *tree, int degree, int (*bp_tree_key_comparator)(struct bp_tree_node *, struct bp_tree_node *));

/**
 * Find key which equals lookup key. If key not found returns NULL.
 */
struct bp_tree_node *bp_tree_lookup(struct bp_tree *tree, struct bp_tree_node *key);

/**
 * Insert key which equals insert key.
 *
 * If key not found returns NULL, otherwise returns previous key (replaced with inserted key).
 */
struct bp_tree_node *bp_tree_insert(struct bp_tree *tree, struct bp_tree_node *key);

/**
 * Delete key which equals delete key.
 *
 * If key not found returns NULL, otherwise returns previous key.
 */
struct bp_tree_node *bp_tree_delete(struct bp_tree *tree, struct bp_tree_node *key);

/**
 * Find minimal key in B+ tree. If tree size = 0 returns NULL.
 */
struct bp_tree_node *bp_tree_min_key(struct bp_tree *tree);

/**
 * Find maximal key in B+ tree. If tree size = 0 returns NULL.
 */
struct bp_tree_node *bp_tree_max_key(struct bp_tree *tree);

/**
 * Find most left leaf.
 */
struct bp_tree_leaf_node *bp_tree_min_leaf(struct bp_tree *tree);

/**
 * Find most right leaf.
 */
struct bp_tree_leaf_node *bp_tree_max_leaf(struct bp_tree *tree);

/**
 * Find collection of keys.
 */
struct bp_tree_batch *bp_tree_insert_batch(struct bp_tree *tree, struct bp_tree_batch *node);

/**
 * Insert collection of keys.
 */
struct bp_tree_batch *bp_tree_lookup_batch(struct bp_tree *tree, struct bp_tree_batch *node);

/**
 * Delete collection of keys.
 */
struct bp_tree_batch *bp_tree_delete_batch(struct bp_tree *tree, struct bp_tree_batch *node);

/**
 * B+ debug print.
 */
void bp_tree_print(struct bp_tree *tree, void (*print_node)(struct bp_tree_node *));

/**
 * Free allocated memory. Before delete all items.
 */
int bp_tree_free(struct bp_tree *tree, void (*free_callback)(struct bp_tree_node *));
