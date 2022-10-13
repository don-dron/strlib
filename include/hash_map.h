#pragma once

#include <stdlib.h>

/**
 * Basic node for item. You must derivative this struct for insert.
 *
 * For example:
 *
 * struct test_node {
 *     struct hash_map_node basic;
 *     int value;
 * }
 *
 * hash_map_insert(map, &test_node.basic);
 */
struct hash_map_node
{
    struct hash_map_node *next;
};

/**
 * Simple hash map with fixed size of buckets.
 */
struct hash_map
{
    /** Current size of hash map */
    int size;

    /** Items hash function, must return non-negative value */
    int (*hash_function)(struct hash_map_node *node);

    /** Comparator for items */
    int (*comparator)(struct hash_map_node *first, struct hash_map_node *second);

    struct hash_map_node *buckets[16];
};

/**
 * Fill struct hash_map by pointer.
 * 
 * For example:
 * 
 * struct hash_map map;
 * hash_map_init(&map, comparator, hash_function);
 *  
 */
int hash_map_init(
    struct hash_map *map,
    int (*comparator)(struct hash_map_node *a, struct hash_map_node *b),
    int (*hash_function)(struct hash_map_node *node));

/**
 * Insert node to hash map. If map contains node with same key,
 * return previous item, otherwise return NULL.
 */
struct hash_map_node *hash_map_insert(struct hash_map *map, struct hash_map_node *node);

/**
 * Find item by key.
 */
struct hash_map_node *hash_map_find(struct hash_map *map, struct hash_map_node *node);

/**
 * Delete item by key. Return deleted item. If item not found, return NULL.
 */
struct hash_map_node *hash_map_delete(struct hash_map *map, struct hash_map_node *node);

/**
 * Debug print for hash map. 
 */
void hash_map_print(struct hash_map *map, void (*print_node)(struct hash_map_node *node));

/**
 * Free allocated memory. Before delete all items.
 */
void hash_map_free(struct hash_map *map, void (*free_callback)(struct hash_map_node *));
