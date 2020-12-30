#ifndef _LIBRARY_H
#define _LIBRARY_H

#include "parser.h"

#define DUPLICATE_SONG (-1)
#define INSERT_SUCCESS (0)

#define SONG_NOT_FOUND (-1)
#define DELETE_SUCCESS (0)

typedef struct tree_node_s {
  char *song_name;
  song_data_t *song;

  struct tree_node_s *left_child;
  struct tree_node_s *right_child;
} tree_node_t;

extern tree_node_t *g_song_library;

//  Type of the functions applied by traversals to each node
typedef void (*traversal_func_t)(tree_node_t *, void *);

//  Tree operations
tree_node_t **find_parent_pointer(tree_node_t **, const char *);
int tree_insert(tree_node_t **, tree_node_t *);
int remove_song_from_tree(tree_node_t **, const char *);
void free_node(tree_node_t *);
void print_node(tree_node_t *, FILE *);

//  Traversal functions
void traverse_pre_order(tree_node_t *, void *, traversal_func_t);
void traverse_in_order(tree_node_t *, void *, traversal_func_t);
void traverse_post_order(tree_node_t *, void *, traversal_func_t);

//  Wrapper functions
void free_library(tree_node_t *);
void write_song_list(FILE *fp, tree_node_t *);

//  Data type specific
void make_library(const char *);

#endif // _LIBRARY_H
