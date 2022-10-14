#include <hash_map.h>

struct test_hash_node
{
    struct hash_map_node core;
    int value;
};

static int hash_node_hash(struct hash_map_node *node)
{
    int value = ((struct test_hash_node *)node)->value;
    return value < 0 ? -value : value;
}

static int hash_node_cmp(struct hash_map_node *first, struct hash_map_node *second)
{
    int first_value = ((struct test_hash_node *)first)->value;
    int second_value = ((struct test_hash_node *)second)->value;

    if (first_value < second_value)
    {
        return -1;
    }
    else if (first_value > second_value)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void free_hash_map_node(struct hash_map_node *node)
{
    free(node);
}

int lookup_int_hash_map(struct hash_map *map, int val)
{
    struct test_hash_node *node = (struct test_hash_node *)malloc(sizeof(struct test_hash_node));
    node->value = val;

    struct hash_map_node *result = hash_map_lookup(map, &node->core);
    free(node);

    if (result == NULL)
    {
        return -1;
    }
    else
    {
        int res = ((struct test_hash_node *)result)->value;
        return res;
    }
}

int insert_int_hash_map(struct hash_map *map, int val)
{
    struct test_hash_node *node = (struct test_hash_node *)malloc(sizeof(struct test_hash_node));
    node->value = val;

    struct hash_map_node *result = hash_map_insert(map, &node->core);

    if (result == NULL)
    {
        return -1;
    }
    else
    {
        int res = ((struct test_hash_node *)result)->value;
        free(result);
        return res;
    }
}

int delete_int_hash_map(struct hash_map *map, int val)
{
    struct test_hash_node *node = (struct test_hash_node *)malloc(sizeof(struct test_hash_node));
    node->value = val;

    struct hash_map_node *result = hash_map_delete(map, &node->core);
    free(node);

    if (result == NULL)
    {
        return -1;
    }
    else
    {
        int res = ((struct test_hash_node *)result)->value;
        free(result);
        return res;
    }
}

int hash_map_test_1(void *unused)
{
    struct hash_map *map = (struct hash_map *)malloc(sizeof(struct hash_map));
    hash_map_init(map, hash_node_cmp, hash_node_hash);

    assert(-1 == lookup_int_hash_map(map, 1));
    insert_int_hash_map(map, 1);
    assert(-1 == lookup_int_hash_map(map, -5));
    assert(1 == lookup_int_hash_map(map, 1));

    assert(-1 == lookup_int_hash_map(map, 2));
    insert_int_hash_map(map, 2);
    assert(-1 == lookup_int_hash_map(map, -5));
    assert(2 == lookup_int_hash_map(map, 2));

    assert(-1 == lookup_int_hash_map(map, 3));
    insert_int_hash_map(map, 3);
    assert(-1 == lookup_int_hash_map(map, -5));
    assert(3 == lookup_int_hash_map(map, 3));

    assert(-1 == lookup_int_hash_map(map, 4));
    insert_int_hash_map(map, 4);
    assert(-1 == lookup_int_hash_map(map, -5));
    assert(4 == lookup_int_hash_map(map, 4));

    assert(-1 == lookup_int_hash_map(map, 5));
    insert_int_hash_map(map, 5);
    assert(-1 == lookup_int_hash_map(map, -5));
    assert(5 == lookup_int_hash_map(map, 5));

    assert(-1 == lookup_int_hash_map(map, 6));
    insert_int_hash_map(map, 6);
    assert(-1 == lookup_int_hash_map(map, -5));
    assert(6 == lookup_int_hash_map(map, 6));

    assert(-1 == lookup_int_hash_map(map, 7));
    insert_int_hash_map(map, 7);
    assert(-1 == lookup_int_hash_map(map, -5));
    assert(7 == lookup_int_hash_map(map, 7));

    assert(-1 == lookup_int_hash_map(map, 8));
    insert_int_hash_map(map, 8);
    assert(-1 == lookup_int_hash_map(map, -5));
    assert(8 == lookup_int_hash_map(map, 8));

    assert(-1 == lookup_int_hash_map(map, -2));
    assert(1 == lookup_int_hash_map(map, 1));
    assert(2 == lookup_int_hash_map(map, 2));
    assert(3 == lookup_int_hash_map(map, 3));
    assert(4 == lookup_int_hash_map(map, 4));
    assert(5 == lookup_int_hash_map(map, 5));
    assert(6 == lookup_int_hash_map(map, 6));
    assert(7 == lookup_int_hash_map(map, 7));
    assert(8 == lookup_int_hash_map(map, 8));
    assert(-1 == lookup_int_hash_map(map, 9));
    assert(-1 == lookup_int_hash_map(map, 10));

    hash_map_free(map, free_hash_map_node);
    free(map);
    return 0;
}

int main()
{
    run_test(hash_map_test_1, (void *)NULL);
    return 1;
}
