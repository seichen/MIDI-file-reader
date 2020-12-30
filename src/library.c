/* Name, library.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */

#include "library.h"
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <ftw.h>

int helper(const char *, const struct stat *, int);

tree_node_t *g_song_library;

/* This function finds the parent
 * pointer of the specified node
 */

tree_node_t **find_parent_pointer(tree_node_t **root,
    const char *song) {
  assert(root != NULL && song != NULL);

  tree_node_t *temp = *root;
  if (strcmp(temp->song_name, song) == 0) {
    return root;
  }

  while (temp != NULL) {
    if (strcmp(temp->song_name, song) < 0) {
      if (temp->right_child == NULL) {
        break;
      }
      if (strcmp(temp->right_child->song_name, song) == 0) {
        return &(temp->right_child);
      }
      temp = temp->right_child;
    }
    else {
      if (temp->left_child == NULL) {
        break;
      }
      if (strcmp(temp->left_child->song_name, song) == 0) {
        return &(temp->left_child);
      }
      temp = temp->left_child;
    }
  }

  return NULL;
} /* find_parent_pointer() */

/* This function inserts a node
 * into the tree
 */

int tree_insert(tree_node_t **root, tree_node_t *insert) {
  assert(root);
  assert(insert);

  tree_node_t *temp = *root;

  while (temp != NULL) {
    if (strcmp(temp->song_name, insert->song_name) == 0) {
      return DUPLICATE_SONG;
    }
    if (strcmp(temp->song_name, insert->song_name) < 0) {
      if (temp->right_child == NULL) {
        temp->right_child = insert;
        return INSERT_SUCCESS;
      }
      temp = temp->right_child;
    }
    else {
      if (temp->left_child == NULL) {
        temp->left_child = insert;
        return INSERT_SUCCESS;
      }
      temp = temp->left_child;
    }
  }

  return 0;
} /* tree_insert() */

/* This function removes the node
 * from the tree
 */

int remove_song_from_tree(tree_node_t **root,
    const char *song) {
  assert(root != NULL && song != NULL);

  tree_node_t *remove = NULL;
  tree_node_t *left = NULL;
  tree_node_t *right = NULL;

  tree_node_t *temp = *root;

  if (strcmp(temp->song_name, song) == 0) {
    right = temp->right_child;
    left = temp->left_child;
    free_node(temp);
    if (left == NULL) {
      *root = right;
      return DELETE_SUCCESS;
    }
    else if (right == NULL) {
      *root = left;
      return DELETE_SUCCESS;
    }
    *root = right;
    while (right != NULL) {
      if (right->left_child == NULL) {
        right->left_child = left;
        return DELETE_SUCCESS;
      }
      right = right->left_child;
    }

  }

  while (temp != NULL) {

    if (strcmp(temp->song_name, song) < 0) {

      if (temp->right_child == NULL) {
        break;
      }
      if (strcmp(temp->right_child->song_name, song) == 0) {

        remove = temp->right_child;
        temp->right_child = remove->right_child;
        left = remove->left_child;

        free_node(remove);

        if (temp->right_child == NULL) {

          temp->right_child = left;
          return DELETE_SUCCESS;
        }
        if (left == NULL) {

          return DELETE_SUCCESS;
        }

        temp = temp->right_child;
        while (temp != NULL) {
          if (strcmp(temp->song_name, left->song_name) < 0) {
            if (temp->right_child == NULL) {
              temp->right_child = left;
              return DELETE_SUCCESS;
            }
            temp = temp->right_child;
          }
          else {
            if (temp->left_child == NULL) {
              temp->left_child = left;
              return DELETE_SUCCESS;
            }
            temp = temp->left_child;
          }

        }
      }
      temp = temp->right_child;
    }
    else {

      if (temp->left_child == NULL) {
        break;
      }
      if (strcmp(temp->left_child->song_name, song) == 0) {

        remove = temp->left_child;
        temp->left_child = remove->left_child;
        right = remove->right_child;

        free_node(remove);

        if (temp->left_child == NULL) {

          temp->left_child = right;
          return DELETE_SUCCESS;
        }
        if (right == NULL) {

          return DELETE_SUCCESS;
        }

        temp = temp->left_child;
        while (temp != NULL) {
          if (strcmp(temp->song_name, right->song_name) < 0) {
            if (temp->right_child == NULL) {
              temp->right_child = right;
              return DELETE_SUCCESS;
            }
            temp = temp->right_child;
          }
          else {
            if (temp->left_child == NULL) {
              temp->left_child = right;
              return DELETE_SUCCESS;
            }
            temp = temp->left_child;
          }

        }
      }

      temp = temp->left_child;
    }
  }

  return SONG_NOT_FOUND;
} /* remove_song_from_tree() */

/* This function frees a node
 * and all its memory
 */

void free_node(tree_node_t *delete) {
  if (delete != NULL) {
    free_song(delete->song);
    //free(delete->song_name);
    free(delete);
  }
} /* free_node() */

/* This function prints
 * a specified node
 */

void print_node(tree_node_t *node, FILE *out) {
  assert(node);
  assert(out);

  fprintf(out, "%s\n", node->song_name);
} /* print_node() */

/* This function traverses
 * the tree in pre order
 */

void traverse_pre_order(tree_node_t *node, void *n,
    traversal_func_t func) {
  if (node == NULL) {
    return;
  }

  func(node, n);

  traverse_pre_order(node->left_child, n, func);

  traverse_pre_order(node->right_child, n, func);

} /* traverse_pre_order() */

/* This function traverses
 * the tree in order
 */

void traverse_in_order(tree_node_t *node, void *n,
    traversal_func_t func) {
  if (node == NULL) {
    return;
  }

  traverse_in_order(node->left_child, n, func);

  func(node, n);

  traverse_in_order(node->right_child, n, func);

} /* traverse_in_order() */

/* This function traverses
 * the tree in post order
 */

void traverse_post_order(tree_node_t *node, void *n,
    traversal_func_t func) {
  if (node == NULL) {
    return;
  }

  traverse_post_order(node->left_child, n, func);

  traverse_post_order(node->right_child, n, func);

  func(node, n);

} /* traverse_post_order() */

/* This function frees the whole
 * library
 */

void free_library(tree_node_t *root) {
  if (root == NULL) {
    return;
  }

  if (root->left_child != NULL) {
    free_library(root->left_child);
  }

  if (root->right_child != NULL) {
    free_library(root->right_child);
  }

  free_node(root);
} /* free_library() */

/* This function writes the
 * library in order
 */

void write_song_list(FILE *fp, tree_node_t *root) {
  assert(fp);
  assert(root);

  traverse_in_order(root, fp, (traversal_func_t) print_node);

} /* write_song_list() */

/* This function takes
 * a directory and makes
 * the library
 */

void make_library(const char *name) {
  assert(name != NULL);

  g_song_library = NULL;

  ftw(name, helper, 20);

} /* make_library() */

int helper(const char *path, const struct stat *sb, int typeflag) {

  if (strstr((char*)path, ".mid") != NULL) {
    tree_node_t *node = malloc(sizeof(tree_node_t));

    int index = -1;

    node->song = parse_file((char*)path);

    for (int i = 0; i < strlen((char*)node->song->path); i++) {
      if (path[i] == '/') {
        index = i;
      }
    }

    node->song_name = &node->song->path[index + 1];

    node->left_child = NULL;
    node->right_child = NULL;

    if (g_song_library == NULL) {
      g_song_library = node;
    }
    else {
      int check = tree_insert(&g_song_library, node);
      assert(check != DUPLICATE_SONG);
    }

  }
  return 0;
}
