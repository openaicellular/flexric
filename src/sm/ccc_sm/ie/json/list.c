/**
 * @file      list.c
 * @brief     List library
 *
 * MIT License
 *
 * Copyright (c) 2021-2023 joelguittet and c-list contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"

/******************************************************************************/
/* Prototypes                                                                 */
/******************************************************************************/

/**
 * @brief Create a list element
 * @param list List instance
 * @param e Element to be added in the list
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
static list_element_t *list_create_element(list_t *list, void *e, size_t size);

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/**
 * @brief Function used to create list instance
 * @param alloc true if element should be allocated when they are added in the list, false for a copy only
 * @param sort Callback function invoked to sort elements of the list, NULL if not used
 * @return List instance if the function succeeded, NULL otherwise
 */
list_t *
list_create(bool alloc, bool (*sort)(list_t *, void *, void *)) {

  /* Create list instance */
  list_t *list = (list_t *)malloc(sizeof(list_t));
  if (NULL == list) {
    /* Unable to allocate memory */
    return NULL;
  }
  memset(list, 0, sizeof(list_t));

  /* Save alloc flag */
  list->alloc = alloc;

  /* Save sort callback */
  list->sort = sort;

  /* Initialize semaphore used to access the list */
  sem_init(&list->sem, 0, 1);

  return list;
}

/**
 * @brief Add element to the the list
 * @param list List instance
 * @param e Element to be added in the list
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
int
list_add(list_t *list, void *e, size_t size) {

  assert(NULL != list);
  assert(NULL != e);

  /* Create a new list element */
  list_element_t *list_element = list_create_element(list, e, size);
  if (NULL == list_element) {
    /* Unable to create list element */
    return -1;
  }

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Add element to the list */
  if (NULL == list->first) {
    list->last = list->first = list->curr = list_element;
  } else if (NULL != list->sort) {
    list_element_t *tmp = list->first;
    while (NULL != tmp) {
      /* Invoke sort callback to know if the new element must be added before the current element */
      if (false == list->sort(list, tmp->e, e)) {
        /* Element must be added just before the current element */
        if (NULL == tmp->prev) {
          list->first = list_element;
        } else {
          tmp->prev->next    = list_element;
          list_element->prev = tmp->prev;
        }
        list_element->next = tmp;
        tmp->prev          = list_element;
        goto ADDED;
      }
      tmp = tmp->next;
    }
    /* End of the list reached, element must be added at the end of the list */
    list->last->next   = list_element;
    list_element->prev = list->last;
    list->last         = list_element;
  } else {
    list->last->next   = list_element;
    list_element->prev = list->last;
    list->last         = list_element;
  }
  ADDED:
  list->count++;

  /* Release semaphore */
  sem_post(&list->sem);

  return 0;
}

/**
 * @brief Add element to the head of the list
 * @param list List instance
 * @param e Element to be added in the list
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
int
list_add_head(list_t *list, void *e, size_t size) {

  assert(NULL != list);
  assert(NULL != e);

  /* Create a new list element */
  list_element_t *list_element = list_create_element(list, e, size);
  if (NULL == list_element) {
    /* Unable to create list element */
    return -1;
  }

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Add element to the list */
  if (NULL == list->first) {
    list->first = list->last = list->curr = list_element;
  } else {
    list->first->prev  = list_element;
    list_element->next = list->first;
    list->first        = list_element;
  }
  list->count++;

  /* Release semaphore */
  sem_post(&list->sem);

  return 0;
}

/**
 * @brief Add element to the tail of the list
 * @param list List instance
 * @param e Element to be added in the list
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
int
list_add_tail(list_t *list, void *e, size_t size) {

  assert(NULL != list);
  assert(NULL != e);

  /* Create a new list element */
  list_element_t *list_element = list_create_element(list, e, size);
  if (NULL == list_element) {
    /* Unable to create list element */
    return -1;
  }

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Add element to the list */
  if (NULL == list->last) {
    list->last = list->first = list->curr = list_element;
  } else {
    list->last->next   = list_element;
    list_element->prev = list->last;
    list->last         = list_element;
  }
  list->count++;

  /* Release semaphore */
  sem_post(&list->sem);

  return 0;
}

/**
 * @brief Get number of element in the list
 * @param list List instance
 * @return Number of elements in the list
 */
size_t
list_get_count(list_t *list) {

  assert(NULL != list);

  size_t count = 0;

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Get number of elements */
  count = list->count;

  /* Release semaphore */
  sem_post(&list->sem);

  return count;
}

/**
 * @brief Get head element of the list
 * @param list List instance
 * @return Head element of the list, NULL if the list is empty
 */
void *
list_get_head(list_t *list) {

  assert(NULL != list);

  void *e = NULL;

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Get head list element */
  list->curr = list->first;

  /* Get element */
  if (NULL != list->curr) {
    e = list->curr->e;
  }

  /* Release semaphore */
  sem_post(&list->sem);

  return e;
}

/**
 * @brief Get tail element of the list
 * @param list List instance
 * @return Tail element of the list, NULL if the list is empty
 */
void *
list_get_tail(list_t *list) {

  assert(NULL != list);

  void *e = NULL;

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Get last list element */
  list->curr = list->last;

  /* Get element */
  if (NULL != list->curr) {
    e = list->curr->e;
  }

  /* Release semaphore */
  sem_post(&list->sem);

  return e;
}

/**
 * @brief Get next element of the list
 * @param list List instance
 * @return Next element of the list, NULL if the end of the list is reached
 */
void *
list_get_next(list_t *list) {

  assert(NULL != list);

  void *e = NULL;

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Get next list element */
  if (NULL != list->curr) {
    list->curr = list->curr->next;
  }

  /* Get element */
  if (NULL != list->curr) {
    e = list->curr->e;
  }

  /* Release semaphore */
  sem_post(&list->sem);

  return e;
}

/**
 * @brief Get previous element of the list
 * @param list List instance
 * @return Previous element of the list, NULL if the beginning of the list is reached
 */
void *
list_get_prev(list_t *list) {

  assert(NULL != list);

  void *e = NULL;

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Get previous list element */
  if (NULL != list->curr) {
    list->curr = list->curr->prev;
  }

  /* Get element */
  if (NULL != list->curr) {
    e = list->curr->e;
  }

  /* Release semaphore */
  sem_post(&list->sem);

  return e;
}

/**
 * @brief Remove element of the list
 * @param list List instance
 * @return Next element of the list, NULL if the element was tail element of the list
 */
void *
list_remove(list_t *list, void *e) {

  assert(NULL != list);

  void *ret = NULL;

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Search for the list element in the list */
  list_element_t *tmp = list->first;
  while (NULL != tmp) {
    if (tmp->e == e) {
      break;
    }
    tmp = tmp->next;
  }
  if (NULL == tmp) {
    /* The element is not part of the list */
    sem_post(&list->sem);
    return NULL;
  }

  /* Update first element if required */
  if (tmp == list->first) {
    list->first = tmp->next;
  }

  /* Update last element if required */
  if (tmp == list->last) {
    list->last = tmp->prev;
  }

  /* Update current element if required */
  if (tmp == list->curr) {
    list->curr = tmp->prev;
  }

  /* Update the list */
  if (NULL != tmp->prev) {
    tmp->prev->next = tmp->next;
  }
  if (NULL != tmp->next) {
    tmp->next->prev = tmp->prev;
  }
  list->count--;

  /* Set next element */
  ret = tmp->next;

  /* Release memory */
  if ((true == list->alloc) && (NULL != tmp->e)) {
    free(tmp->e);
  }
  free(tmp);

  /* Release semaphore */
  sem_post(&list->sem);

  return ret;
}

/**
 * @brief Remove head element of the list
 * @param list List instance
 * @return Head element of the list, NULL if the list is empty
 */
void *
list_remove_head(list_t *list) {

  assert(NULL != list);

  void *e = NULL;

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Update current element if required */
  if ((NULL != list->curr) && (list->curr == list->first)) {
    list->curr = list->first->next;
  }

  /* Get head element */
  if (NULL != list->first) {
    e = list->first->e;
  }

  /* Update the list */
  if (NULL != list->first) {
    list_element_t *tmp = list->first;
    if (NULL != list->first->next) {
      list->first->next->prev = NULL;
      list->first             = list->first->next;
    } else {
      list->first = list->last = NULL;
    }
    list->count--;
    free(tmp);
  }

  /* Release semaphore */
  sem_post(&list->sem);

  return e;
}

/**
 * @brief Remove tail element of the list
 * @param list List instance
 * @return Tail element of the list, NULL if the list is empty
 */
void *
list_remove_tail(list_t *list) {

  assert(NULL != list);

  void *e = NULL;

  /* Wait semaphore */
  sem_wait(&list->sem);

  /* Update current element if required */
  if ((NULL != list->curr) && (list->curr == list->last)) {
    list->curr = list->last->prev;
  }

  /* Get tail element */
  if (NULL != list->last) {
    e = list->last->e;
  }

  /* Update the list */
  if (NULL != list->last) {
    list_element_t *tmp = list->last;
    if (NULL != list->last->prev) {
      list->last->prev->next = NULL;
      list->last             = list->last->prev;
    } else {
      list->first = list->last = NULL;
    }
    list->count--;
    free(tmp);
  }

  /* Release semaphore */
  sem_post(&list->sem);

  return e;
}

/**
 * @brief Release list instance
 * @param list List instance
 */
void
list_release(list_t *list) {

  /* Release list instance */
  if (NULL != list) {

    /* Wait semaphore */
    sem_wait(&list->sem);

    /* Release list elements */
    list_element_t *list_element = list->first;
    while (NULL != list_element) {
      list_element_t *tmp = list_element;
      list_element        = list_element->next;
      if ((true == list->alloc) && (NULL != tmp->e)) {
        free(tmp->e);
      }
      free(tmp);
    }

    /* Release semaphore */
    sem_post(&list->sem);
    sem_close(&list->sem);

    /* Release list instance */
    free(list);
  }
}

/**
 * @brief Create a list element
 * @param list List instance
 * @param e Element to be added in the list
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
static list_element_t *
list_create_element(list_t *list, void *e, size_t size) {

  assert(NULL != list);
  assert(NULL != e);

  /* Create a new list element */
  list_element_t *list_element = (list_element_t *)malloc(sizeof(list_element_t));
  if (NULL == list_element) {
    /* Unable to allocate memory */
    return NULL;
  }
  memset(list_element, 0, sizeof(list_element_t));

  /* Store element */
  if (true == list->alloc) {
    if (NULL == (list_element->e = malloc(size))) {
      /* Unable to allocate memory */
      free(list_element);
      return NULL;
    }
    memcpy(list_element->e, e, size);
  } else {
    list_element->e = e;
  }

  return list_element;
}