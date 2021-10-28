#pragma once

#include <stdlib.h>


// int_node struct code from stack demo from class
typedef struct int_node {
  int value;
  struct int_node* next;
}int_node_t;

// This struct represents the sorted list itself.
typedef struct sorted_list {
  int_node_t * top;
} sorted_list_t;

/// Initialize a sorted list.
void sorted_list_init(sorted_list_t* lst);

/// Destroy a sorted list.
void sorted_list_destroy(sorted_list_t* lst);

/// Add an element to a sorted list, maintaining the lowest-to-highest sorted value.
void sorted_list_insert(sorted_list_t* lst, int value);

/// Count how many times a value appears in a sorted list.
size_t sorted_list_count(sorted_list_t* lst, int value);

/// Print the values in a sorted list in ascending order, separated by spaces and followed by a
/// newline.
void sorted_list_print(sorted_list_t* lst);
