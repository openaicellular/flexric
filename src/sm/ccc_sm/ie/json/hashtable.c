/**
 * @file      hashtable.c
 * @brief     Hashtable library
 *
 * MIT License
 *
 * Copyright (c) 2021-2023 joelguittet and c-hashtable contributors
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

#include "hashtable.h"

/******************************************************************************/
/* Prototypes                                                                 */
/******************************************************************************/

/**
 * @brief Compute hash value of the wanted key
 * @param hashtable Hashtable instance
 * @param key Key as string
 * @return Hash value modulo hashtable size to be used in the hashtable table
 */
static size_t hashtable_compute_hash(hashtable_t *hashtable, char *key);

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/**
 * @brief Function used to create hashtable instance
 * @param size Horizontal size of the hashtable
 * @param alloc true if element should be allocated when they are added in the hashtable, false for a copy only
 * @return Hashtable instance if the function succeeded, NULL otherwise
 */
hashtable_t *
hashtable_create(size_t size, bool alloc) {

  /* Create hashtable instance */
  hashtable_t *hashtable = (hashtable_t *)malloc(sizeof(hashtable_t));
  if (NULL == hashtable) {
    /* Unable to allocate memory */
    return NULL;
  }
  memset(hashtable, 0, sizeof(hashtable_t));

  /* Create table */
  if (NULL == (hashtable->table = (hashtable_element_t **)malloc(size * sizeof(hashtable_element_t *)))) {
    /* Unable to allocate memory */
    free(hashtable);
    return NULL;
  }
  memset(hashtable->table, 0, size * sizeof(hashtable_element_t *));

  /* Save size and alloc flag */
  hashtable->size  = size;
  hashtable->alloc = alloc;

  /* Initialize semaphore used to access the hashtable */
  sem_init(&hashtable->sem, 0, 1);

  return hashtable;
}

/**
 * @brief Add element to the hashtable
 * @param hashtable Hashtable instance
 * @param key Key of the element to be added
 * @param e Element to be added in the hashtable
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
int
hashtable_add(hashtable_t *hashtable, char *key, void *e, size_t size) {

  assert(NULL != hashtable);
  assert(NULL != key);

  /* Wait semaphore */
  sem_wait(&hashtable->sem);

  /* Compute hash value of the wanted key */
  size_t hash = hashtable_compute_hash(hashtable, key);

  /* Check if the element already exist, update the element in this case */
  hashtable_element_t *last = NULL;
  hashtable_element_t *curr = hashtable->table[hash];
  while (NULL != curr) {
    if (!strcmp(curr->key, key)) {
      /* Element found */
      if ((true == hashtable->alloc) && (NULL != curr->e)) {
        free(curr->e);
        curr->e = NULL;
      }
      if ((true == hashtable->alloc) && (NULL != e) && (0 != size)) {
        if (NULL == (curr->e = malloc(size))) {
          /* Unable to allocate memory */
          sem_post(&hashtable->sem);
          return -1;
        }
        memcpy(curr->e, e, size);
      } else {
        curr->e = e;
      }
      sem_post(&hashtable->sem);
      return 0;
    }
    last = curr;
    curr = curr->next;
  }

  /* Element not found, create a new hashtable element */
  hashtable_element_t *hashtable_element = (hashtable_element_t *)malloc(sizeof(hashtable_element_t));
  if (NULL == hashtable_element) {
    /* Unable to allocate memory */
    sem_post(&hashtable->sem);
    return -1;
  }
  memset(hashtable_element, 0, sizeof(hashtable_element_t));

  /* Store key */
  if (NULL == (hashtable_element->key = strdup(key))) {
    /* Unable to allocate memory */
    free(hashtable_element);
    sem_post(&hashtable->sem);
    return -1;
  }

  /* Store element */
  if ((true == hashtable->alloc) && (NULL != e) && (0 != size)) {
    if (NULL == (hashtable_element->e = malloc(size))) {
      /* Unable to allocate memory */
      free(hashtable_element->key);
      free(hashtable_element);
      sem_post(&hashtable->sem);
      return -1;
    }
    memcpy(hashtable_element->e, e, size);
  } else {
    hashtable_element->e = e;
  }

  /* Add element to the hashtable */
  if (NULL == last) {
    hashtable->table[hash] = hashtable_element;
  } else {
    last->next = hashtable_element;
  }
  hashtable->count++;

  /* Release semaphore */
  sem_post(&hashtable->sem);

  return 0;
}

/**
 * @brief Get number of element in the hashtable
 * @param hashtable Hashtable instance
 * @return Number of elements in the hashtable
 */
size_t
hashtable_get_count(hashtable_t *hashtable) {

  assert(NULL != hashtable);

  size_t count = 0;

  /* Wait semaphore */
  sem_wait(&hashtable->sem);

  /* Get number of elements */
  count = hashtable->count;

  /* Release semaphore */
  sem_post(&hashtable->sem);

  return count;
}

/**
 * @brief Check if key is present in the hashtable
 * @param hashtable Hashtable instance
 * @param key Key of the element
 * @return true if the key is found, false otherwise
 */
bool
hashtable_has_key(hashtable_t *hashtable, char *key) {

  assert(NULL != hashtable);
  assert(NULL != key);

  bool found = false;

  /* Wait semaphore */
  sem_wait(&hashtable->sem);

  /* Compute hash value of the wanted key */
  size_t hash = hashtable_compute_hash(hashtable, key);

  /* Lookup for the wanted element */
  hashtable_element_t *curr = hashtable->table[hash];
  while (NULL != curr) {
    if (!strcmp(curr->key, key)) {
      /* Element found */
      found = true;
      break;
    }
    curr = curr->next;
  }

  /* Release semaphore */
  sem_post(&hashtable->sem);

  return found;
}

/**
 * @brief Get all keys of the hashtable
 * @param hashtable Hashtable instance
 * @param keys Keys of the hashtable (free required)
 * @return Number of element of the hashtable
 */
size_t
hashtable_get_keys(hashtable_t *hashtable, char ***keys) {

  assert(NULL != hashtable);

  size_t count = 0;

  /* Wait semaphore */
  sem_wait(&hashtable->sem);

  /* Get number of elements */
  count = hashtable->count;

  /* Check if at least one element is in the hashtable */
  if (0 < count) {

    /* Create table of keys */
    if (NULL != (*keys = (char **)malloc(count * sizeof(char *)))) {

      /* Parse table and store keys */
      size_t index = 0;
      for (size_t hash = 0; hash < hashtable->size; hash++) {
        hashtable_element_t *curr = hashtable->table[hash];
        while (NULL != curr) {
          (*keys)[index] = curr->key;
          index++;
          curr = curr->next;
        }
      }
    }
  }

  /* Release semaphore */
  sem_post(&hashtable->sem);

  return count;
}

/**
 * @brief Lookup element of the hashtable
 * @param hashtable Hashtable instance
 * @param key Key of the element
 * @return Element of the hashtable, NULL if not found
 */
void *
hashtable_lookup(hashtable_t *hashtable, char *key) {

  assert(NULL != hashtable);
  assert(NULL != key);

  void *e = NULL;

  /* Wait semaphore */
  sem_wait(&hashtable->sem);

  /* Compute hash value of the wanted key */
  size_t hash = hashtable_compute_hash(hashtable, key);

  /* Lookup for the wanted element */
  hashtable_element_t *curr = hashtable->table[hash];
  while (NULL != curr) {
    if (!strcmp(curr->key, key)) {
      /* Element found */
      e = curr->e;
      break;
    }
    curr = curr->next;
  }

  /* Release semaphore */
  sem_post(&hashtable->sem);

  return e;
}

/**
 * @brief Remove element of the hashtable
 * @param hashtable Hashtable instance
 * @param key Key of the element
 * @return Head element of the hashtable, NULL if not found
 */
void *
hashtable_remove(hashtable_t *hashtable, char *key) {

  assert(NULL != hashtable);
  assert(NULL != key);

  void *e = NULL;

  /* Wait semaphore */
  sem_wait(&hashtable->sem);

  /* Compute hash value of the wanted key */
  size_t hash = hashtable_compute_hash(hashtable, key);

  /* Lookup for the wanted element */
  hashtable_element_t *last = NULL;
  hashtable_element_t *curr = hashtable->table[hash];
  while (NULL != curr) {
    if (!strcmp(curr->key, key)) {
      /* Element found */
      e = curr->e;
      /* Update the list of elements */
      if (NULL == last) {
        hashtable->table[hash] = curr->next;
      } else {
        last->next = curr->next;
      }
      /* Release memory */
      free(curr->key);
      free(curr);
      break;
    }
    last = curr;
    curr = curr->next;
  }

  /* Release semaphore */
  sem_post(&hashtable->sem);

  return e;
}

/**
 * @brief Release hashtable instance
 * @param hashtable Hashtable instance
 */
void
hashtable_release(hashtable_t *hashtable) {

  /* Release hashtable instance */
  if (NULL != hashtable) {

    /* Wait semaphore */
    sem_wait(&hashtable->sem);

    /* Release hashtable elements */
    for (size_t hash = 0; hash < hashtable->size; hash++) {
      hashtable_element_t *curr = hashtable->table[hash];
      while (NULL != curr) {
        hashtable_element_t *tmp = curr;
        curr                     = curr->next;
        free(tmp->key);
        if ((true == hashtable->alloc) && (NULL != tmp->e)) {
          free(tmp->e);
        }
        free(tmp);
      }
    }

    /* Release table */
    free(hashtable->table);

    /* Release semaphore */
    sem_post(&hashtable->sem);
    sem_close(&hashtable->sem);

    /* Release hashtable instance */
    free(hashtable);
  }
}

/**
 * @brief Compute hash value of the wanted key
 * @param hashtable Hashtable instance
 * @param key Key as string
 * @return Hash value modulo hashtable size to be used in the hashtable table
 */
static size_t
hashtable_compute_hash(hashtable_t *hashtable, char *key) {

  assert(NULL != hashtable);
  assert(NULL != key);

  /* Compute djb2 hash */
  int           c;
  unsigned long hash = 5381;
  while (0 != (c = *key++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash % hashtable->size;
}