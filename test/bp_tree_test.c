#include <hash_map.h>
#include <bp_tree.h>

struct test_node
{
    struct bp_tree_node core;
    int value;
};

struct test_hash_node
{
    struct hash_map_node core;
    int value;
};

static void free_bp_tree_node(struct bp_tree_node *node)
{
    free(node);
}

static void free_hash_map_node(struct hash_map_node *node)
{
    free(node);
}

static void node_print(struct bp_tree_node *node)
{
    int first_value = ((struct test_node *)node)->value;
    printf(" %d ", first_value);
}

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

static int node_cmp(struct bp_tree_node *first, struct bp_tree_node *second)
{
    int first_value = ((struct test_node *)first)->value;
    int second_value = ((struct test_node *)second)->value;

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

static void assert_tree(struct bp_tree *tree)
{
    struct bp_tree_struct_node *current = (struct bp_tree_struct_node *)tree->root;
    int level = tree->size / tree->degree;
    while (current)
    {
        for (int i = 0; i < current->size - 1; i++)
        {
            assert(tree->comparator(current->keys[i], current->keys[i + 1]) == -1);
        }

        if (!current->leaf)
        {
            for (int i = 0; i < current->size; i++)
            {
                assert(tree->comparator(current->keys[i], (((struct bp_tree_non_leaf_node *)current)->children[i])->keys[0]) >= 0);
            }

            for (int i = 1; i < current->size; i++)
            {
                struct bp_tree_struct_node *left_child = ((struct bp_tree_non_leaf_node *)current)->children[i - 1];
                if (tree->comparator(current->keys[i], left_child->keys[left_child->size - 1]) != 1)
                {
                    bp_tree_print(tree, node_print);
                    assert(tree->comparator(current->keys[i], left_child->keys[left_child->size - 1]) == 1);
                }
            }

            for (int i = 0; i < current->size; i++)
            {
                struct bp_tree_struct_node *left_child = ((struct bp_tree_non_leaf_node *)current)->children[i];
                struct bp_tree_struct_node *right_child = ((struct bp_tree_non_leaf_node *)current)->children[i + 1];

                if (tree->comparator(left_child->keys[left_child->size - 1], right_child->keys[0]) != -1)
                {
                    bp_tree_print(tree, node_print);
                    assert(tree->comparator(left_child->keys[left_child->size - 1], right_child->keys[0]) == -1);
                }
            }
        }

        current = (struct bp_tree_struct_node *)current->right;

        if (!current)
        {
            break;
        }

        if (current->leaf == 1 || current->size == 0)
        {
            current = NULL;
        }
        else
        {
            current = (struct bp_tree_struct_node *)((struct bp_tree_non_leaf_node *)current)->children[0];
        }
    }
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

int insert_int_bp_tree(struct bp_tree *tree, int val)
{
    struct test_node *node = (struct test_node *)malloc(sizeof(struct test_node));
    node->value = val;

    struct bp_tree_batch *batch = (struct bp_tree_batch *)malloc(sizeof(struct bp_tree_batch));
    batch->nodes = (struct bp_tree_node **)malloc(sizeof(struct bp_tree_node *));
    batch->size = 1;
    batch->nodes[0] = &node->core;

    struct bp_tree_batch *result = bp_tree_insert_batch(tree, batch);
    free(batch->nodes);
    free(batch);

    if (result->size == 0)
    {
        free(result->nodes);
        free(result);
        return -1;
    }
    else
    {
        int res = ((struct test_node *)result->nodes[0])->value;
        assert_tree(tree);
        free(result->nodes[0]);
        free(result->nodes);
        free(result);
        return res;
    }
}

int delete_int_bp_tree(struct bp_tree *tree, int val)
{
    struct test_node *node = (struct test_node *)malloc(sizeof(struct test_node));
    node->value = val;

    struct bp_tree_batch *batch = (struct bp_tree_batch *)malloc(sizeof(struct bp_tree_batch));
    batch->nodes = (struct bp_tree_node **)malloc(sizeof(struct bp_tree_node *));
    batch->size = 1;
    batch->nodes[0] = &node->core;

    struct bp_tree_batch *result = bp_tree_delete_batch(tree, batch);
    free(node);
    free(batch->nodes);
    free(batch);

    if (result->size == 0)
    {
        free(result->nodes);
        free(result);
        return -1;
    }
    else
    {
        struct test_node *test_node = (struct test_node *)result->nodes[0];
        int res = test_node->value;
        assert_tree(tree);
        free(result->nodes[0]);
        free(result->nodes);
        free(result);
        return res;
    }
}

int lookup_int_bp_tree(struct bp_tree *tree, int val)
{
    struct test_node *node = (struct test_node *)malloc(sizeof(struct test_node));
    node->value = val;

    struct bp_tree_batch *batch = (struct bp_tree_batch *)malloc(sizeof(struct bp_tree_batch));
    batch->nodes = (struct bp_tree_node **)malloc(sizeof(struct bp_tree_node *));
    batch->size = 1;
    batch->nodes[0] = &node->core;

    struct bp_tree_batch *result = bp_tree_lookup_batch(tree, batch);
    free(node);
    free(batch->nodes);
    free(batch);

    if (result->size == 0)
    {
        free(result->nodes);
        free(result);
        return -1;
    }
    else
    {
        int res = ((struct test_node *)result->nodes[0])->value;
        free(result->nodes);
        free(result);
        return res;
    }
}

static void print_stat(struct bp_tree *tree)
{
    printf("Split leaf: %d\n", tree->split_leaf);
    printf("Split non_leaf: %d\n", tree->split_non_leaf);

    printf("Rebalance left leaf: %d\n", tree->rebalance_left_leaf);
    printf("Rebalance right leaf: %d\n", tree->rebalance_right_leaf);
    printf("Rebalance left non leaf: %d\n", tree->rebalance_left_non_leaf);
    printf("Rebalance right non leaf: %d\n", tree->rebalance_right_non_leaf);

    printf("Merge left leaf: %d\n", tree->merge_left_leaf);
    printf("Merge right leaf: %d\n", tree->merge_right_leaf);
    printf("Merge left non leaf: %d\n", tree->merge_left_non_leaf);
    printf("Merge right non leaf: %d\n", tree->merge_right_non_leaf);
}

int bp_tree_test_1(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 4, node_cmp);

    assert(-1 == lookup_int_bp_tree(tree, 1));
    insert_int_bp_tree(tree, 1);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(1 == lookup_int_bp_tree(tree, 1));

    assert(-1 == lookup_int_bp_tree(tree, 2));
    insert_int_bp_tree(tree, 2);

    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(2 == lookup_int_bp_tree(tree, 2));

    assert(-1 == lookup_int_bp_tree(tree, 3));
    insert_int_bp_tree(tree, 3);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(3 == lookup_int_bp_tree(tree, 3));

    assert(-1 == lookup_int_bp_tree(tree, 4));
    insert_int_bp_tree(tree, 4);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(4 == lookup_int_bp_tree(tree, 4));

    assert(-1 == lookup_int_bp_tree(tree, 5));
    insert_int_bp_tree(tree, 5);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(5 == lookup_int_bp_tree(tree, 5));

    assert(-1 == lookup_int_bp_tree(tree, 6));
    insert_int_bp_tree(tree, 6);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(6 == lookup_int_bp_tree(tree, 6));

    assert(-1 == lookup_int_bp_tree(tree, 7));
    insert_int_bp_tree(tree, 7);
    assert(-1 == lookup_int_bp_tree(tree, -5));

    assert(7 == lookup_int_bp_tree(tree, 7));

    assert(-1 == lookup_int_bp_tree(tree, 8));
    insert_int_bp_tree(tree, 8);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(8 == lookup_int_bp_tree(tree, 8));

    assert(-1 == lookup_int_bp_tree(tree, -2));
    assert(1 == lookup_int_bp_tree(tree, 1));
    assert(2 == lookup_int_bp_tree(tree, 2));
    assert(3 == lookup_int_bp_tree(tree, 3));
    assert(4 == lookup_int_bp_tree(tree, 4));
    assert(5 == lookup_int_bp_tree(tree, 5));
    assert(6 == lookup_int_bp_tree(tree, 6));
    assert(7 == lookup_int_bp_tree(tree, 7));
    assert(8 == lookup_int_bp_tree(tree, 8));
    assert(-1 == lookup_int_bp_tree(tree, 9));
    assert(-1 == lookup_int_bp_tree(tree, 10));

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_2(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 4, node_cmp);

    assert(-1 == lookup_int_bp_tree(tree, 2));
    insert_int_bp_tree(tree, 2);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(2 == lookup_int_bp_tree(tree, 2));

    assert(-1 == lookup_int_bp_tree(tree, 1));
    insert_int_bp_tree(tree, 1);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(1 == lookup_int_bp_tree(tree, 1));

    assert(-1 == lookup_int_bp_tree(tree, 5));
    insert_int_bp_tree(tree, 5);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(5 == lookup_int_bp_tree(tree, 5));

    assert(-1 == lookup_int_bp_tree(tree, 4));
    insert_int_bp_tree(tree, 4);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(4 == lookup_int_bp_tree(tree, 4));

    assert(-1 == lookup_int_bp_tree(tree, 7));
    insert_int_bp_tree(tree, 7);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(7 == lookup_int_bp_tree(tree, 7));

    assert(-1 == lookup_int_bp_tree(tree, 8));
    insert_int_bp_tree(tree, 8);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(8 == lookup_int_bp_tree(tree, 8));

    assert(-1 == lookup_int_bp_tree(tree, 6));
    insert_int_bp_tree(tree, 6);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(6 == lookup_int_bp_tree(tree, 6));

    assert(-1 == lookup_int_bp_tree(tree, 3));
    insert_int_bp_tree(tree, 3);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(3 == lookup_int_bp_tree(tree, 3));

    assert(-1 == lookup_int_bp_tree(tree, -2));
    assert(1 == lookup_int_bp_tree(tree, 1));
    assert(2 == lookup_int_bp_tree(tree, 2));
    assert(3 == lookup_int_bp_tree(tree, 3));
    assert(4 == lookup_int_bp_tree(tree, 4));
    assert(5 == lookup_int_bp_tree(tree, 5));
    assert(6 == lookup_int_bp_tree(tree, 6));
    assert(7 == lookup_int_bp_tree(tree, 7));
    assert(8 == lookup_int_bp_tree(tree, 8));
    assert(-1 == lookup_int_bp_tree(tree, 9));
    assert(-1 == lookup_int_bp_tree(tree, 10));

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_3(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 4, node_cmp);

    assert(-1 == lookup_int_bp_tree(tree, 2));
    insert_int_bp_tree(tree, 2);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(2 == lookup_int_bp_tree(tree, 2));

    assert(-1 == lookup_int_bp_tree(tree, 1));
    insert_int_bp_tree(tree, 1);
    assert(-1 == lookup_int_bp_tree(tree, -5));
    assert(1 == lookup_int_bp_tree(tree, 1));

    assert(-1 == lookup_int_bp_tree(tree, -2));
    assert(1 == lookup_int_bp_tree(tree, 1));
    assert(2 == lookup_int_bp_tree(tree, 2));

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_4(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 4, node_cmp);

    for (int i = 0; i < 100; i++)
    {
        insert_int_bp_tree(tree, rand());
    }

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_5(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 7, node_cmp);

    for (int i = 0; i < 100000; i++)
    {
        insert_int_bp_tree(tree, rand() % 10000);
    }

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_6(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 5, node_cmp);

    for (int i = 0; i < 100; i++)
    {
        int value = rand() % 100;
        int result = insert_int_bp_tree(tree, value);
    }

    for (int i = 0; i < 100000; i++)
    {
        int value = rand() % 100;
        int result = delete_int_bp_tree(tree, value);
    }

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_7(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 4, node_cmp);

    insert_int_bp_tree(tree, 8);
    insert_int_bp_tree(tree, 20);
    insert_int_bp_tree(tree, 11);
    insert_int_bp_tree(tree, 63);
    insert_int_bp_tree(tree, 3);
    insert_int_bp_tree(tree, 14);
    insert_int_bp_tree(tree, 51);
    insert_int_bp_tree(tree, 88);
    insert_int_bp_tree(tree, 48);
    insert_int_bp_tree(tree, 18);
    insert_int_bp_tree(tree, 89);
    insert_int_bp_tree(tree, 57);
    insert_int_bp_tree(tree, 11);
    insert_int_bp_tree(tree, 84);
    insert_int_bp_tree(tree, 24);
    insert_int_bp_tree(tree, 42);
    insert_int_bp_tree(tree, 53);
    insert_int_bp_tree(tree, 39);
    insert_int_bp_tree(tree, 26);
    insert_int_bp_tree(tree, 7);
    insert_int_bp_tree(tree, 111);
    insert_int_bp_tree(tree, 834);
    insert_int_bp_tree(tree, 22);
    insert_int_bp_tree(tree, 41);
    insert_int_bp_tree(tree, 54);
    insert_int_bp_tree(tree, 40);
    insert_int_bp_tree(tree, 31);
    insert_int_bp_tree(tree, 35);

    delete_int_bp_tree(tree, 8);
    delete_int_bp_tree(tree, 24);
    delete_int_bp_tree(tree, 22);
    delete_int_bp_tree(tree, 20);
    delete_int_bp_tree(tree, 11);
    delete_int_bp_tree(tree, 63);
    delete_int_bp_tree(tree, 3);
    delete_int_bp_tree(tree, 14);
    delete_int_bp_tree(tree, 51);
    delete_int_bp_tree(tree, 88);
    delete_int_bp_tree(tree, 48);
    delete_int_bp_tree(tree, 18);
    delete_int_bp_tree(tree, 89);
    delete_int_bp_tree(tree, 57);
    delete_int_bp_tree(tree, 11);
    delete_int_bp_tree(tree, 84);
    delete_int_bp_tree(tree, 24);
    delete_int_bp_tree(tree, 42);
    delete_int_bp_tree(tree, 53);
    delete_int_bp_tree(tree, 39);
    delete_int_bp_tree(tree, 26);
    delete_int_bp_tree(tree, 7);
    delete_int_bp_tree(tree, 111);
    delete_int_bp_tree(tree, 834);
    delete_int_bp_tree(tree, 22);
    delete_int_bp_tree(tree, 41);
    delete_int_bp_tree(tree, 54);
    delete_int_bp_tree(tree, 40);
    delete_int_bp_tree(tree, 31);
    delete_int_bp_tree(tree, 35);
    delete_int_bp_tree(tree, 111);
    delete_int_bp_tree(tree, 834);

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_8(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 8, node_cmp);

    for (int i = 0; i < 10000; i++)
    {
        int t = rand() % 1000;
        insert_int_bp_tree(tree, t);
        insert_int_bp_tree(tree, rand() % 1000);

        for (int j = 0; j < 100; j++)
        {
            delete_int_bp_tree(tree, rand() % 1000);
        }
        delete_int_bp_tree(tree, t);
    }

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_9(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    bp_tree_init(tree, 5, node_cmp);

    insert_int_bp_tree(tree, 1);
    insert_int_bp_tree(tree, 4);
    insert_int_bp_tree(tree, 9);
    insert_int_bp_tree(tree, 10);
    insert_int_bp_tree(tree, 11);
    insert_int_bp_tree(tree, 12);
    insert_int_bp_tree(tree, 13);
    insert_int_bp_tree(tree, 15);
    insert_int_bp_tree(tree, 16);
    insert_int_bp_tree(tree, 20);
    insert_int_bp_tree(tree, 25);

    delete_int_bp_tree(tree, 48);

    print_stat(tree);
    bp_tree_free(tree, free_bp_tree_node);
    free(tree);
    return 0;
}

int bp_tree_test_10(void *unused)
{
    struct bp_tree *tree = (struct bp_tree *)malloc(sizeof(struct bp_tree));
    struct hash_map *map = (struct hash_map *)malloc(sizeof(struct hash_map));
    bp_tree_init(tree, 4, node_cmp);
    hash_map_init(map, hash_node_cmp, hash_node_hash);
    int n = 1000000;
    int m = 100;
    int hash = 0;

    int insert_count = 0;
    int delete_count = 0;

    for (int i = 0; i < n; i++)
    {
        int value = rand() % m;

        int op = rand() % 3;

        if (op == 0)
        {
            int bp_tree_result = lookup_int_bp_tree(tree, value);
            int hash_map_result = lookup_int_hash_map(map, value);
            assert(bp_tree_result == hash_map_result);
        }
        else if (op == 1)
        {
            int bp_tree_result = insert_int_bp_tree(tree, value);
            int hash_map_result = insert_int_hash_map(map, value);
            assert(bp_tree_result == hash_map_result);

            if (bp_tree_result == -1)
            {
                hash ^= value;
                insert_count++;
            }
        }
        else if (op == 2)
        {
            int bp_tree_result = delete_int_bp_tree(tree, value);
            int hash_map_result = delete_int_hash_map(map, value);
            assert(bp_tree_result == hash_map_result);

            if (bp_tree_result != -1)
            {
                delete_count++;
                hash ^= value;
            }
        }
    }

    struct test_node *previous = NULL;
    int index = 0;
    bp_tree_for_each(tree, var, struct test_node)
    {
        index++;

        if (previous != NULL)
        {
            assert(tree->comparator(&previous->core, &var->core) == -1);
        }

        previous = var;
    }

    assert(tree->size == index);

    for (int i = 0; i < m; i++)
    {
        int bp_tree_result = delete_int_bp_tree(tree, i);
        int hash_map_result = delete_int_hash_map(map, i);
        assert(bp_tree_result == hash_map_result);

        if (bp_tree_result != -1)
        {
            hash ^= i;
            delete_count++;
        }
    }

    assert(hash == 0);
    assert(insert_count == delete_count);

    print_stat(tree);

    bp_tree_free(tree, free_bp_tree_node);
    hash_map_free(map, free_hash_map_node);
    free(tree);
    free(map);
    return 0;
}

int main()
{
    run_test(bp_tree_test_1, (void *)NULL);
    run_test(bp_tree_test_2, (void *)NULL);
    run_test(bp_tree_test_3, (void *)NULL);
    run_test(bp_tree_test_4, (void *)NULL);
    run_test(bp_tree_test_5, (void *)NULL);
    run_test(bp_tree_test_6, (void *)NULL);
    run_test(bp_tree_test_7, (void *)NULL);
    run_test(bp_tree_test_8, (void *)NULL);
    run_test(bp_tree_test_9, (void *)NULL);
    run_test(bp_tree_test_10, (void *)NULL);
    return 0;
}
