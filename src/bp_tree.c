#include <bp_tree.h>

/**
 * Returns true if node is leaf.
 */
static inline int bp_tree_node_is_leaf(struct bp_tree_struct_node *node);

/**
 * Create non leaf node;
 */
static struct bp_tree_non_leaf_node *bp_tree_init_non_leaf(struct bp_tree *tree);

/**
 * Create leaf node;
 */
static struct bp_tree_leaf_node *bp_tree_init_leaf(struct bp_tree *tree);

/**
 * Find minimal key in node.
 */
static struct bp_tree_node *bp_tree_min_node_key(struct bp_tree *tree,
                                                 struct bp_tree_struct_node *node,
                                                 int ignore_size);

/**
 * Find maximal key in node.
 */
static struct bp_tree_node *bp_tree_max_node_key(struct bp_tree *tree,
                                                 struct bp_tree_struct_node *node);

/**
 * Update each key in node by minimal keys in children.
 */
static void bp_tree_update_node(struct bp_tree *tree, struct bp_tree_struct_node *node);

/**
 * Find leaf node which contains key.
 */
static struct bp_tree_struct_node *bp_tree_lookup_leaf(struct bp_tree *tree,
                                                       struct bp_tree_node *key);

/**
 * Find key in leaf node.
 */
static struct bp_tree_node *bp_tree_lookup_leaf_child(struct bp_tree *tree,
                                                      struct bp_tree_node *key);

/**
 * Insert key to leaf node.
 */
static struct bp_tree_node *bp_tree_insert_leaf_child(struct bp_tree *tree, struct bp_tree_node *key);
/**
 * Split struct node (leaf or non-leaf).
 */
static void bp_tree_split(struct bp_tree *tree,
                          struct bp_tree_struct_node *for_split);

/**
 * Delete key in leaf or non-leaf node.
 */
static struct bp_tree_node *bp_tree_delete_child(struct bp_tree *tree, struct bp_tree_struct_node *found, struct bp_tree_node *key);
/**
 * Move key from left node to right node.
 */
static void bp_tree_transfer_from_left_to_right(struct bp_tree *tree,
                                                struct bp_tree_struct_node *left,
                                                struct bp_tree_struct_node *right);
/**
 * Move key from right node to left node.
 */
static void bp_tree_transfer_from_right_to_left(struct bp_tree *tree,
                                                struct bp_tree_struct_node *left,
                                                struct bp_tree_struct_node *right);
/**
 * Merge left and right nodes.
 */
static void bp_tree_merge_nodes(struct bp_tree *tree,
                                struct bp_tree_struct_node *left,
                                struct bp_tree_struct_node *right);

/**
 * Free non leaf node;
 */
static struct bp_tree_non_leaf_node *bp_tree_free_non_leaf(struct bp_tree_non_leaf_node *node);

/**
 * Free leaf node;
 */
static struct bp_tree_leaf_node *bp_tree_free_leaf(struct bp_tree_leaf_node *node);

int bp_tree_init(struct bp_tree *tree,
                 int size,
                 int (*node_comparator)(struct bp_tree_node *, struct bp_tree_node *))
{
    tree->degree = size;
    tree->size = 0;
    tree->comparator = node_comparator;
    tree->root = &bp_tree_init_leaf(tree)->core;
    tree->split_leaf = 0;
    tree->split_non_leaf = 0;
    tree->rebalance_left_leaf = 0;
    tree->rebalance_left_non_leaf = 0;
    tree->rebalance_right_leaf = 0;
    tree->rebalance_right_non_leaf = 0;
    tree->merge_left_leaf = 0;
    tree->merge_left_non_leaf = 0;
    tree->merge_right_leaf = 0;
    tree->merge_right_non_leaf = 0;
    return 0;
}

int bp_tree_free(struct bp_tree *tree,
                 void (*free_callback)(struct bp_tree_node *))
{
    while (1)
    {
        struct bp_tree_node *min_key = bp_tree_min_node_key(tree, tree->root, 0);

        if (min_key == NULL)
        {
            break;
        }
        else
        {
            free_callback(bp_tree_delete(tree, min_key));
        }
    }
    bp_tree_free_leaf((struct bp_tree_leaf_node *)tree->root);
    return 0;
}

struct bp_tree_node *bp_tree_min_key(struct bp_tree *tree)
{
    return bp_tree_min_node_key(tree, tree->root, 0);
}

struct bp_tree_node *bp_tree_max_key(struct bp_tree *tree)
{
    return bp_tree_max_node_key(tree, tree->root);
}

struct bp_tree_node *bp_tree_lookup(struct bp_tree *tree, struct bp_tree_node *node)
{
    return bp_tree_lookup_leaf_child(tree, node);
}

struct bp_tree_node *bp_tree_insert(struct bp_tree *tree, struct bp_tree_node *node)
{
    return bp_tree_insert_leaf_child(tree, node);
}

struct bp_tree_node *bp_tree_delete(struct bp_tree *tree, struct bp_tree_node *node)
{
    return bp_tree_delete_child(tree, bp_tree_lookup_leaf(tree, node), node);
}

struct bp_tree_batch *bp_tree_lookup_batch(struct bp_tree *tree, struct bp_tree_batch *node)
{
    struct bp_tree_batch *result = (struct bp_tree_batch *)malloc(sizeof(struct bp_tree_batch));
    result->nodes = (struct bp_tree_node **)malloc(sizeof(struct bp_tree_batch));

    int result_size = 0;

    for (int i = 0; i < node->size; i++)
    {
        struct bp_tree_node *found = bp_tree_lookup_leaf_child(tree, node->nodes[i]);

        if (found != NULL)
        {
            result->nodes[result_size++] = found;
        }
    }

    result->size = result_size;

    return result;
}

struct bp_tree_batch *bp_tree_insert_batch(struct bp_tree *tree, struct bp_tree_batch *node)
{
    struct bp_tree_batch *result = (struct bp_tree_batch *)malloc(sizeof(struct bp_tree_batch));
    result->nodes = (struct bp_tree_node **)malloc(sizeof(struct bp_tree_batch));

    int result_size = 0;

    for (int i = 0; i < node->size; i++)
    {
        struct bp_tree_node *found = bp_tree_insert_leaf_child(tree, node->nodes[i]);

        if (found != NULL)
        {
            result->nodes[result_size++] = found;
        }
    }

    result->size = result_size;

    return result;
}

struct bp_tree_batch *bp_tree_delete_batch(struct bp_tree *tree, struct bp_tree_batch *node)
{
    struct bp_tree_batch *result = (struct bp_tree_batch *)malloc(sizeof(struct bp_tree_batch));
    result->nodes = (struct bp_tree_node **)malloc(sizeof(struct bp_tree_batch));

    int result_size = 0;

    for (int i = 0; i < node->size; i++)
    {
        struct bp_tree_leaf_node *found = (struct bp_tree_leaf_node *)bp_tree_lookup_leaf(tree, node->nodes[i]);

        struct bp_tree_node *deleted = bp_tree_delete_child(tree, &found->core, node->nodes[i]);

        if (deleted != NULL)
        {
            result->nodes[result_size++] = deleted;
        }
    }

    result->size = result_size;

    return result;
}

void bp_tree_print(struct bp_tree *tree,
                   void (*print_node)(struct bp_tree_node *))
{
    struct bp_tree_struct_node *current = (struct bp_tree_struct_node *)tree->root;
    int level = tree->size / tree->degree;
    while (current)
    {
        struct bp_tree_struct_node *step = current;

        for (int i = 0; i < level; i++)
        {
            printf(" ");
        }

        while (step)
        {
            printf("[");
            for (int i = 0; i < step->size; i++)
            {
                print_node(step->keys[i]);
            }
            printf("]");

            step = step->right;

            if (step != NULL)
            {
                for (int i = 0; i < level; i++)
                {
                    printf(" ");
                }
            }
        }

        printf("\n\n");

        if (current->leaf == 1 || current->size == 0)
        {
            current = NULL;
        }
        else
        {
            current = (struct bp_tree_struct_node *)((struct bp_tree_non_leaf_node *)current)->children[0];
        }

        level--;
    }
}

struct bp_tree_leaf_node *bp_tree_min_leaf(struct bp_tree *tree)
{
    struct bp_tree_struct_node *current = tree->root;
    while (1)
    {
        if (bp_tree_node_is_leaf(current))
        {
            return (struct bp_tree_leaf_node *)current;
        }

        current = ((struct bp_tree_non_leaf_node *)current)->children[0];
    }
}

struct bp_tree_leaf_node *bp_tree_max_leaf(struct bp_tree *tree)
{
    struct bp_tree_struct_node *current = tree->root;
    while (1)
    {
        if (bp_tree_node_is_leaf(current))
        {
            return (struct bp_tree_leaf_node *)current;
        }

        current = ((struct bp_tree_non_leaf_node *)current)->children[current->size];
    }
}

static struct bp_tree_non_leaf_node *bp_tree_init_non_leaf(struct bp_tree *tree)
{
    struct bp_tree_non_leaf_node *node = (struct bp_tree_non_leaf_node *)malloc(sizeof(struct bp_tree_non_leaf_node));
    node->core.keys = (struct bp_tree_node **)calloc(tree->degree, sizeof(struct bp_tree_node *));
    node->children = (struct bp_tree_struct_node **)calloc(tree->degree + 1, sizeof(struct bp_tree_struct_node *));
    node->core.leaf = 0;
    node->core.size = 0;
    node->core.left = NULL;
    node->core.right = NULL;
    node->core.parent = NULL;
    return node;
}

static struct bp_tree_leaf_node *bp_tree_init_leaf(struct bp_tree *tree)
{
    struct bp_tree_leaf_node *node = (struct bp_tree_leaf_node *)malloc(sizeof(struct bp_tree_leaf_node));
    node->core.keys = (struct bp_tree_node **)calloc(tree->degree, sizeof(struct bp_tree_node *));
    node->core.parent = NULL;
    node->core.leaf = 1;
    node->core.size = 0;
    node->index = 0;
    node->core.left = NULL;
    node->core.right = NULL;
    node->core.parent = NULL;
    return node;
}

static inline int bp_tree_node_is_leaf(struct bp_tree_struct_node *node)
{
    return node->leaf == 1;
}

static struct bp_tree_struct_node *bp_tree_lookup_leaf(struct bp_tree *tree, struct bp_tree_node *key)
{
    struct bp_tree_struct_node *current = (struct bp_tree_struct_node *)tree->root;

    while (!bp_tree_node_is_leaf(current))
    {
        struct bp_tree_non_leaf_node *as_non_leaf = (struct bp_tree_non_leaf_node *)current;
        current = NULL;

        for (int i = 0; i < as_non_leaf->core.size; i++)
        {
            int cmp = tree->comparator(key, as_non_leaf->core.keys[i]);
            if (cmp == -1)
            {
                current = (struct bp_tree_struct_node *)as_non_leaf->children[i];
                break;
            }

            if (i == as_non_leaf->core.size - 1)
            {
                current = (struct bp_tree_struct_node *)as_non_leaf->children[as_non_leaf->core.size];
                break;
            }
        }
    }

    return current;
}

static struct bp_tree_node *bp_tree_lookup_leaf_child(struct bp_tree *tree, struct bp_tree_node *key)
{
    struct bp_tree_leaf_node *found = (struct bp_tree_leaf_node *)bp_tree_lookup_leaf(tree, key);

    for (int i = 0; i < found->core.size; i++)
    {
        int cmp = tree->comparator(found->core.keys[i], key);
        if (cmp == 0)
        {
            return found->core.keys[i];
        }
        else if (cmp == 1)
        {
            break;
        }
    }

    return NULL;
}

static void bp_tree_split(struct bp_tree *tree, struct bp_tree_struct_node *for_split)
{

    struct bp_tree_struct_node *new_node = NULL;
    struct bp_tree_node *mid = NULL;

    if (bp_tree_node_is_leaf(for_split))
    {
        tree->split_leaf++;

        struct bp_tree_leaf_node *node = (struct bp_tree_leaf_node *)for_split;
        struct bp_tree_leaf_node *leaf = bp_tree_init_leaf(tree);
        new_node = &leaf->core;
        leaf->core.right = node->core.right;

        if (node->core.right != NULL)
        {
            node->core.right->left = &leaf->core;
        }

        node->core.right = &leaf->core;
        leaf->core.left = &node->core;
        leaf->core.parent = node->core.parent;

        int t = tree->degree / 2;
        mid = node->core.keys[t];
        leaf->core.size = node->core.size - t;
        node->core.size = t;

        for (int i = 0; i < leaf->core.size; i++)
        {
            leaf->core.keys[i] = node->core.keys[i + node->core.size];
        }
    }
    else
    {
        tree->split_non_leaf++;

        struct bp_tree_non_leaf_node *node = (struct bp_tree_non_leaf_node *)for_split;
        struct bp_tree_non_leaf_node *non_leaf = bp_tree_init_non_leaf(tree);
        new_node = &non_leaf->core;
        non_leaf->core.right = node->core.right;
        non_leaf->core.parent = node->core.parent;

        if (node->core.right != NULL)
        {
            node->core.right->left = &non_leaf->core;
        }

        node->core.right = &non_leaf->core;
        non_leaf->core.left = &node->core;

        int t = tree->degree / 2;
        mid = node->core.keys[t];
        non_leaf->core.size = node->core.size - t - 1;
        node->core.size = t;

        for (int i = 0; i < non_leaf->core.size; i++)
        {
            non_leaf->core.keys[i] = node->core.keys[i + node->core.size + 1];
        }

        for (int i = 0; i <= non_leaf->core.size; i++)
        {
            non_leaf->children[i] = node->children[i + node->core.size + 1];
            ((struct bp_tree_struct_node *)non_leaf->children[i])->parent = (struct bp_tree_struct_node *)non_leaf;
        }
    }

    if (for_split == tree->root)
    {
        tree->root = &bp_tree_init_non_leaf(tree)->core;
        ((struct bp_tree_struct_node *)tree->root)->keys[0] = mid;
        ((struct bp_tree_non_leaf_node *)tree->root)->children[0] = for_split;
        ((struct bp_tree_non_leaf_node *)tree->root)->children[1] = new_node;
        ((struct bp_tree_struct_node *)tree->root)->size = 1;
        for_split->parent = tree->root;
        new_node->parent = tree->root;
    }
    else
    {
        new_node->parent = for_split->parent;
        struct bp_tree_struct_node *parent = for_split->parent;

        int position = 0;

        while (position < parent->size && tree->comparator(mid, parent->keys[position]) != -1)
        {
            position++;
        }

        for (int i = parent->size; i > position; i--)
        {
            parent->keys[i] = parent->keys[i - 1];
        }

        struct bp_tree_non_leaf_node *parent_as_non_leaf = ((struct bp_tree_non_leaf_node *)parent);
        for (int i = parent->size + 1; i > position + 1; i--)
        {
            parent_as_non_leaf->children[i] = parent_as_non_leaf->children[i - 1];
        }

        parent_as_non_leaf->core.keys[position] = mid;
        parent_as_non_leaf->children[position + 1] = new_node;
        parent_as_non_leaf->children[position + 1]->parent = &parent_as_non_leaf->core;
        parent_as_non_leaf->core.size++;

        if (parent->size >= tree->degree)
        {
            bp_tree_split(tree, parent);
        }
    }
}

static struct bp_tree_node *bp_tree_insert_leaf_child(struct bp_tree *tree, struct bp_tree_node *key)
{
    struct bp_tree_leaf_node *found = (struct bp_tree_leaf_node *)bp_tree_lookup_leaf(tree, key);
    struct bp_tree_node *prev = NULL;
    int position = 0;

    for (int i = 0; i < found->core.size; i++)
    {
        int cmp = tree->comparator(found->core.keys[i], key);

        if (cmp == 0)
        {
            prev = found->core.keys[i];
            break;
        }
        else if (cmp == -1)
        {
            position++;
        }
        else
        {
            break;
        }
    }

    if (prev == NULL)
    {
        for (int i = found->core.size; i > position; i--)
        {
            found->core.keys[i] = found->core.keys[i - 1];
        }

        found->core.size++;
        tree->size++;
    }

    found->core.keys[position] = key;
    bp_tree_update_node(tree, &found->core);

    if (found->core.size >= tree->degree)
    {
        bp_tree_split(tree, &found->core);
    }

    return prev;
}

static struct bp_tree_node *bp_tree_min_node_key(struct bp_tree *tree,
                                                 struct bp_tree_struct_node *node,
                                                 int ignore_size)
{
    struct bp_tree_struct_node *current = node;
    while (1)
    {
        if (bp_tree_node_is_leaf(current))
        {
            if (ignore_size)
            {
                return current->keys[0];
            }
            else
            {
                return current->size == 0 ? NULL : current->keys[0];
            }
        }

        current = ((struct bp_tree_non_leaf_node *)current)->children[0];
    }
}

static struct bp_tree_node *bp_tree_max_node_key(struct bp_tree *tree,
                                                 struct bp_tree_struct_node *node)
{
    struct bp_tree_struct_node *current = node;
    while (1)
    {
        if (bp_tree_node_is_leaf(current))
        {
            return current->size == 0 ? NULL : current->keys[current->size - 1];
        }

        current = ((struct bp_tree_non_leaf_node *)current)->children[current->size];
    }
}

static void bp_tree_update_node(struct bp_tree *tree, struct bp_tree_struct_node *node)
{
    struct bp_tree_struct_node *current = node;

    while (current != NULL)
    {
        if (current->leaf != 1)
        {
            struct bp_tree_non_leaf_node *non_leaf = (struct bp_tree_non_leaf_node *)current;

            for (int i = 0; i < non_leaf->core.size; i++)
            {
                struct bp_tree_node *min_node = bp_tree_min_node_key(tree, non_leaf->children[i + 1], 1);
                non_leaf->core.keys[i] = min_node;
            }
        }

        current = current->parent;
    }
}

static void bp_tree_transfer_from_left_to_right(struct bp_tree *tree,
                                                struct bp_tree_struct_node *left,
                                                struct bp_tree_struct_node *right)
{
    struct bp_tree_node *shift = right->keys[0];

    for (int i = right->size; i > 0; i--)
    {
        right->keys[i] = right->keys[i - 1];
    }
    right->keys[0] = left->keys[left->size - 1];

    if (right->leaf == 1)
    {
        tree->rebalance_left_leaf++;
    }
    else
    {
        tree->rebalance_left_non_leaf++;

        struct bp_tree_non_leaf_node *right_non_leaf = (struct bp_tree_non_leaf_node *)right;
        for (int i = right->size + 1; i > 0; i--)
        {
            right_non_leaf->children[i] = right_non_leaf->children[i - 1];
        }

        right_non_leaf->children[0] = ((struct bp_tree_non_leaf_node *)left)->children[left->size];
        right_non_leaf->children[0]->parent = &right_non_leaf->core;
    }

    left->size--;
    right->size++;
    bp_tree_update_node(tree, right->parent);
}

static void bp_tree_transfer_from_right_to_left(struct bp_tree *tree,
                                                struct bp_tree_struct_node *left,
                                                struct bp_tree_struct_node *right)
{
    left->keys[left->size] = right->keys[0];

    for (int i = 0; i < right->size - 1; i++)
    {
        right->keys[i] = right->keys[i + 1];
    }

    if (bp_tree_node_is_leaf(left))
    {
        tree->rebalance_right_leaf++;
    }
    else
    {
        tree->rebalance_right_non_leaf++;
        ((struct bp_tree_non_leaf_node *)left)->children[left->size + 1] = ((struct bp_tree_non_leaf_node *)right)->children[0];
        ((struct bp_tree_non_leaf_node *)left)->children[left->size + 1]->parent = left;

        for (int i = 0; i < right->size; i++)
        {
            ((struct bp_tree_non_leaf_node *)right)->children[i] = ((struct bp_tree_non_leaf_node *)right)->children[i + 1];
            ((struct bp_tree_non_leaf_node *)right)->children[i]->parent = right;
        }
    }

    right->size--;
    left->size++;

    bp_tree_update_node(tree, left->parent);
}

static void bp_tree_merge_nodes(struct bp_tree *tree,
                                struct bp_tree_struct_node *left,
                                struct bp_tree_struct_node *right)
{
    struct bp_tree_node *mid = bp_tree_min_node_key(tree, right, 1);

    struct bp_tree_struct_node *parent = right->parent;

    if (bp_tree_node_is_leaf(left))
    {
        for (int i = 0; i < right->size; i++)
        {
            left->keys[left->size + i] = right->keys[i];
        }
        left->size += right->size;
    }
    else
    {
        for (int i = 0; i <= right->size; i++)
        {
            ((struct bp_tree_non_leaf_node *)left)->children[left->size + i + 1] = ((struct bp_tree_non_leaf_node *)right)->children[i];
            ((struct bp_tree_non_leaf_node *)left)->children[left->size + i + 1]->parent = left;
        }

        for (int i = 0; i < right->size; i++)
        {
            left->keys[left->size + i + 1] = right->keys[i];
        }

        left->keys[left->size] = ((struct bp_tree_non_leaf_node *)left)->children[left->size + 1]->keys[0];
        left->size += right->size + 1;
    }

    left->right = right->right;

    if (left->right)
    {
        left->right->left = left;
    }

    bp_tree_update_node(tree, left);
    bp_tree_delete_child(tree, right->parent, mid);
    bp_tree_update_node(tree, left->parent);

    if (bp_tree_node_is_leaf(right))
    {
        bp_tree_free_leaf((struct bp_tree_leaf_node *)right);
    }
    else
    {
        bp_tree_free_non_leaf((struct bp_tree_non_leaf_node *)right);
    }
}

static struct bp_tree_non_leaf_node *bp_tree_free_non_leaf(struct bp_tree_non_leaf_node *node)
{
    free(node->children);
    free(node->core.keys);
    free(node);
}

static struct bp_tree_leaf_node *bp_tree_free_leaf(struct bp_tree_leaf_node *node)
{
    free(node->core.keys);
    free(node);
}

static struct bp_tree_node *bp_tree_delete_child(struct bp_tree *tree, struct bp_tree_struct_node *node, struct bp_tree_node *key)
{
    struct bp_tree_node *result_key = NULL;
    int position = 0;

    for (int i = 0; i < node->size; i++)
    {
        int cmp = tree->comparator(node->keys[i], key);

        if (cmp == 0)
        {
            result_key = node->keys[i];
            break;
        }
        else if (cmp == -1)
        {
            position++;
        }
        else
        {
            break;
        }
    }

    if (result_key == NULL)
    {
        return NULL;
    }

    for (int i = position; i < node->size - 1; i++)
    {
        node->keys[i] = node->keys[i + 1];
    }

    if (node->leaf != 1)
    {
        struct bp_tree_struct_node *child = ((struct bp_tree_non_leaf_node *)node)->children[position + 1];
        for (int i = position + 1; i < node->size; i++)
        {
            ((struct bp_tree_non_leaf_node *)node)->children[i] = ((struct bp_tree_non_leaf_node *)node)->children[i + 1];
        }

        if (child->right != NULL)
        {
            child->right->left = child->left;
        }

        if (child->left != NULL)
        {
            child->left->right = child->right;
        }
    }
    else
    {
        tree->size--;
    }

    bp_tree_update_node(tree, node);
    node->size--;

    if (node->size < tree->degree / 2)
    {
        struct bp_tree_struct_node *left = (struct bp_tree_struct_node *)node->left;
        struct bp_tree_struct_node *right = (struct bp_tree_struct_node *)node->right;

        int degree = tree->degree / 2;

        if (node->left != NULL && left->size > degree - 1)
        {
            bp_tree_transfer_from_left_to_right(tree, left, node);
        }
        else if (node->right != NULL && right->size > degree - 1)
        {
            bp_tree_transfer_from_right_to_left(tree, node, right);
        }
        else if (left != NULL && node->parent == left->parent)
        {
            if (left->leaf == 1)
            {
                tree->merge_left_leaf++;
            }
            else
            {
                tree->merge_left_non_leaf++;
            }
            bp_tree_merge_nodes(tree, left, node);
        }
        else if (right != NULL && node->parent == right->parent)
        {
            if (right->leaf == 1)
            {
                tree->merge_right_leaf++;
            }
            else
            {
                tree->merge_right_non_leaf++;
            }
            bp_tree_merge_nodes(tree, node, right);
        }
    }

    if (tree->root->size == 0 && tree->root->leaf != 1)
    {
        struct bp_tree_non_leaf_node *last_root = (struct bp_tree_non_leaf_node *)tree->root;
        tree->root = last_root->children[0];
        tree->root->parent = NULL;

        bp_tree_free_non_leaf(last_root);
    }
    return result_key;
}
