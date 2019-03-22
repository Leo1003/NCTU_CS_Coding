#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

typedef struct node {
    struct node *left, *right;
    int count;
} node_t;

node_t *node_pool = NULL;
size_t next_free_node = 0;

node_t *node_new()
{
    node_t *new_node = &node_pool[next_free_node++];
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->count = 0;
    return new_node;
}

void insert(node_t *root, const char *str)
{
    node_t *current = root;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '0') {
            if (current->left == NULL) {
                current->left = node_new();
            }
            current = current->left;
        } else if (str[i] == '1') {
            if (current->right == NULL) {
                current->right = node_new();
            }
            current = current->right;
        } else {
            continue;
        }
        current->count++;
    }
}

int search(node_t *root, const char *str)
{
    node_t *current = root;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '0') {
            if (current->left == NULL) {
                return 0;
            }
            current = current->left;
        } else if (str[i] == '1') {
            if (current->right == NULL) {
                return 0;
            }
            current = current->right;
        }
    }
    return current->count;
}

int main() {
    node_pool = (node_t*)malloc(10485760 * sizeof(node_t));
    if (!node_pool) {
        abort();
    }
    node_t *root = node_new();
    
    int n;
    char *line = NULL;
    size_t line_s = 0;

    scanf("%d", &n);
    getline(&line, &line_s, stdin);

    for (int i = 0; i < n; i++) {
        getline(&line, &line_s, stdin);
        insert(root, line);
    }
    
    int q;
    scanf("%d", &q);
    getline(&line, &line_s, stdin);

    for (int i = 0; i < q; i++) {
        getline(&line, &line_s, stdin);
        int count = search(root, line);
        printf("%d\n", count);
    }

    return 0;
}
