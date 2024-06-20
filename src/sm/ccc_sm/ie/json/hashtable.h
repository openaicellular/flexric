/**
 * @file      hashtable.h
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

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being
   called from a project with a different default calling convention. For windows you have 3 define options:

HASHTABLE_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
HASHTABLE_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
HASHTABLE_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the HASHTABLE_API_VISIBILITY flag to "export" the same symbols the way HASHTABLE_EXPORT_SYMBOLS does

*/

#define HASHTABLE_CDECL   __cdecl
#define HASHTABLE_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(HASHTABLE_HIDE_SYMBOLS) && !defined(HASHTABLE_IMPORT_SYMBOLS) && !defined(HASHTABLE_EXPORT_SYMBOLS)
#define HASHTABLE_EXPORT_SYMBOLS
#endif

#if defined(HASHTABLE_HIDE_SYMBOLS)
#define HASHTABLE_PUBLIC(type) type HASHTABLE_STDCALL
#elif defined(HASHTABLE_EXPORT_SYMBOLS)
#define HASHTABLE_PUBLIC(type) __declspec(dllexport) type HASHTABLE_STDCALL
#elif defined(HASHTABLE_IMPORT_SYMBOLS)
#define HASHTABLE_PUBLIC(type) __declspec(dllimport) type HASHTABLE_STDCALL
#endif
#else /* !__WINDOWS__ */
#define HASHTABLE_CDECL
#define HASHTABLE_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && defined(HASHTABLE_API_VISIBILITY)
#define HASHTABLE_PUBLIC(type) __attribute__((visibility("default"))) type
#else
#define HASHTABLE_PUBLIC(type) type
#endif
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>

/******************************************************************************/
/* Definitions                                                                */
/******************************************************************************/

/**
 * Hashtable element
 */
typedef struct hashtable_element_s {
  struct hashtable_element_s *next; /**< Next element of the hashtable */
  char *                      key;  /**< Element key */
  void *                      e;    /**< Element itself */
} hashtable_element_t;

/**
 * Hashtable instance
 */
typedef struct {
  hashtable_element_t **table; /**< Table of lists of elements */
  size_t                size;  /**< Size of the table of lists of elements */
  size_t                count; /**< Number of elements in the hashtable */
  bool                  alloc; /**< Flag to indicate if elements are allocated when they are added in the hashtable */
  sem_t                 sem;   /**< Semaphore used to protect the access to the hashtable */
} hashtable_t;

/******************************************************************************/
/* Prototypes                                                                 */
/******************************************************************************/

/**
 * @brief Function used to create hashtable instance
 * @param size Horizontal size of the hashtable
 * @param alloc true if element should be allocated when they are added in the hashtable, false for a copy only
 * @return Hashtable instance if the function succeeded, NULL otherwise
 */
HASHTABLE_PUBLIC(hashtable_t *) hashtable_create(size_t size, bool alloc);

/**
 * @brief Add element to the hashtable
 * @param hashtable Hashtable instance
 * @param key Key of the element to be added
 * @param e Element to be added in the hashtable
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
HASHTABLE_PUBLIC(int) hashtable_add(hashtable_t *hashtable, char *key, void *e, size_t size);

/**
 * @brief Get number of element in the hashtable
 * @param hashtable Hashtable instance
 * @return Number of elements in the hashtable
 */
HASHTABLE_PUBLIC(size_t) hashtable_get_count(hashtable_t *hashtable);

/**
 * @brief Check if key is present in the hashtable
 * @param hashtable Hashtable instance
 * @param key Key of the element
 * @return true if the key is found, false otherwise
 */
HASHTABLE_PUBLIC(bool) hashtable_has_key(hashtable_t *hashtable, char *key);

/**
 * @brief Get all keys of the hashtable
 * @param hashtable Hashtable instance
 * @param keys Keys of the hashtable (free required)
 * @return Number of element of the hashtable
 */
HASHTABLE_PUBLIC(size_t) hashtable_get_keys(hashtable_t *hashtable, char ***keys);

/**
 * @brief Lookup element of the hashtable
 * @param hashtable Hashtable instance
 * @param key Key of the element
 * @return Element of the hashtable, NULL if not found
 */
HASHTABLE_PUBLIC(void *) hashtable_lookup(hashtable_t *hashtable, char *key);

/**
 * @brief Remove element of the hashtable
 * @param hashtable Hashtable instance
 * @param key Key of the element
 * @return Head element of the hashtable, NULL if not found
 */
HASHTABLE_PUBLIC(void *) hashtable_remove(hashtable_t *hashtable, char *key);

/**
 * @brief Release hashtable instance
 * @param hashtable Hashtable instance
 */
HASHTABLE_PUBLIC(void) hashtable_release(hashtable_t *hashtable);

#ifdef __cplusplus
}
#endif

#endif /* __HASHTABLE_H__ */