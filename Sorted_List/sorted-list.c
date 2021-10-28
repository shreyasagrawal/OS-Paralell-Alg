/*A sorted list. 
* This data structure behaves like a normal list, except it maintains all of the values in sorted order (lowest to highest).
* Linked list approach
*/
#include "sorted-list.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Initialize a sorted list.
 *
 * \param lst This is a pointer to space that should be initialized as a sorted list. The caller of
 * this function retains ownership of the memory that lst points to (meaning the caller must free it
 * if the pointer was returned from malloc)
 */
void sorted_list_init(sorted_list_t* lst) {
  lst->top = NULL;
}

/**
 * Destroy a sorted list. Free any memory allocated to store the list, but not the list itself.
 *
 * \param lst This is a pointer to the space that holds the list being destroyred. This function
 * should free any memory used to represent the list, but the caller retains ownership of the lst
 * pointer itself.
 */
void sorted_list_destroy(sorted_list_t* lst) {

  int_node_t* tmp = lst->top;

  while (tmp != NULL) {
    int_node_t* nexttmp = tmp->next;
    free(tmp);
    tmp = nexttmp;
  }
}

/**
 * Add an element to a sorted list, maintaining the lowest-to-highest sorted order.
 *
 * \param lst   The list that is being inserted to
 * \param value The value being inserted
 */

void sorted_list_insert(sorted_list_t* lst, int value) {
  // Make space for a new node
  int_node_t* new_node = malloc(sizeof(int_node_t));

  // Fill in the new node
  new_node->value = value;
  //special case to check if this node is the first node being put on stack
  if (lst->top == NULL) {
    new_node->next = NULL;
    lst->top = new_node;
    return;
  }

  // temporary node to hold the node at the top of stack
  int_node_t* tmp = lst->top;

  //case where the newnode has a smaller value than top of stack, so we put it on top of stack
  if (lst->top->value > new_node->value) {
    lst->top = new_node;
    new_node->next = tmp;
    return;
  }

  //while loop to check where to insert newnode. Breaks if we reach the last node on stack
  // or the newnode has a smaller value than current node
  while (tmp->next != NULL && tmp->value < new_node->value) {
    tmp = tmp->next;
  }  // while

  //insert the new node after the node that breaks the loop
  new_node->next = tmp->next;
  tmp->next = new_node;

  //swapping values if the node inserted is smaller than prev node
  if (tmp->value > new_node->value) {
    int val = tmp->value;
    tmp->value = new_node->value;
    new_node->value = val;
  }
}  // insert

/**
 * Count how many times a value appears in a sorted list.
 *
 * \param lst The list being searched
 * \param value The value being counted
 * \returns the number of times value appears in lst
 */
size_t sorted_list_count(sorted_list_t* lst, int value) {
  int_node_t* tmp = lst->top;
  size_t count = 0;

  while (tmp != NULL) {
    if (tmp->value == value) {
      count++;
    }
    tmp = tmp->next;
  }
  return count;
}

/**
 * Print the values in a sorted list in ascending order, separated by spaces and followed by a
 * newline.
 *
 * \param lst The list to print
 */
void sorted_list_print(sorted_list_t* lst) {
  if (lst->top == NULL){
    printf("Empty stack. Nothing to print");
  }
  int_node_t* tmp = lst->top;

  while (tmp != NULL) {
    printf("%d ", tmp->value);
    tmp = tmp->next;
  }
  printf("\n");
}
