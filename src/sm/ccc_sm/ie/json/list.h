/**
 * @file      list.h
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

#ifndef __LIST_H__
#define __LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being
   called from a project with a different default calling convention. For windows you have 3 define options:

LIST_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
LIST_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
LIST_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the LIST_API_VISIBILITY flag to "export" the same symbols the way LIST_EXPORT_SYMBOLS does

*/

#define LIST_CDECL   __cdecl
#define LIST_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(LIST_HIDE_SYMBOLS) && !defined(LIST_IMPORT_SYMBOLS) && !defined(LIST_EXPORT_SYMBOLS)
#define LIST_EXPORT_SYMBOLS
#endif

#if defined(LIST_HIDE_SYMBOLS)
#define LIST_PUBLIC(type) type LIST_STDCALL
#elif defined(LIST_EXPORT_SYMBOLS)
#define LIST_PUBLIC(type) __declspec(dllexport) type LIST_STDCALL
#elif defined(LIST_IMPORT_SYMBOLS)
#define LIST_PUBLIC(type) __declspec(dllimport) type LIST_STDCALL
#endif
#else /* !__WINDOWS__ */
#define LIST_CDECL
#define LIST_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && defined(LIST_API_VISIBILITY)
#define LIST_PUBLIC(type) __attribute__((visibility("default"))) type
#else
#define LIST_PUBLIC(type) type
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
 * List element
 */
typedef struct list_element_s {
  struct list_element_s *prev; /**< Previous element of the list */
  struct list_element_s *next; /**< Next element of the list */
  void *                 e;    /**< Element itself */
} list_element_t;

/**
 * List instance
 */
typedef struct list_s {
  list_element_t *first;                         /**< First element of the list */
  list_element_t *last;                          /**< Last element of the list */
  list_element_t *curr;                          /**< Current element of the list, used to parse the list */
  size_t          count;                         /**< Number of elements in the list */
  bool            alloc;                         /**< Flag to indicate if elements are allocated when they are added in the list */
  bool (*sort)(struct list_s *, void *, void *); /**< Callback function invoked to sort elements of the list, NULL if not used */
  sem_t sem;                                     /**< Semaphore used to protect the access to the list */
} list_t;

/******************************************************************************/
/* Prototypes                                                                 */
/******************************************************************************/

/**
 * @brief Function used to create list instance
 * @param alloc true if element should be allocated when they are added in the list, false for a copy only
 * @param sort Callback function invoked to sort elements of the list, NULL if not used
 * @return List instance if the function succeeded, NULL otherwise
 */
LIST_PUBLIC(list_t *) list_create(bool alloc, bool (*sort)(list_t *, void *, void *));

/**
 * @brief Add element to the the list
 * @param list List instance
 * @param e Element to be added in the list
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
LIST_PUBLIC(int) list_add(list_t *list, void *e, size_t size);

/**
 * @brief Add element to the head of the list
 * @param list List instance
 * @param e Element to be added in the list
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
LIST_PUBLIC(int) list_add_head(list_t *list, void *e, size_t size);

/**
 * @brief Add element to the tail of the list
 * @param list List instance
 * @param e Element to be added in the list
 * @param size Size of the element to be added
 * @return 0 if the function succeeded, -1 otherwise
 */
LIST_PUBLIC(int) list_add_tail(list_t *list, void *e, size_t size);

/**
 * @brief Get number of element in the list
 * @param list List instance
 * @return Number of elements in the list
 */
LIST_PUBLIC(size_t) list_get_count(list_t *list);

/**
 * @brief Get head element of the list
 * @param list List instance
 * @return Head element of the list, NULL if the list is empty
 */
LIST_PUBLIC(void *) list_get_head(list_t *list);

/**
 * @brief Get tail element of the list
 * @param list List instance
 * @return Tail element of the list, NULL if the list is empty
 */
LIST_PUBLIC(void *) list_get_tail(list_t *list);

/**
 * @brief Get next element of the list
 * @param list List instance
 * @return Next element of the list, NULL if the end of the list is reached
 */
LIST_PUBLIC(void *) list_get_next(list_t *list);

/**
 * @brief Get previous element of the list
 * @param list List instance
 * @return Previous element of the list, NULL if the beginning of the list is reached
 */
LIST_PUBLIC(void *) list_get_prev(list_t *list);

/**
 * @brief Remove element of the list
 * @param list List instance
 * @return Next element of the list, NULL if the element was tail element of the list
 */
LIST_PUBLIC(void *) list_remove(list_t *list, void *e);

/**
 * @brief Remove head element of the list
 * @param list List instance
 * @return Head element of the list, NULL if the list is empty
 */
LIST_PUBLIC(void *) list_remove_head(list_t *list);

/**
 * @brief Remove tail element of the list
 * @param list List instance
 * @return Tail element of the list, NULL if the list is empty
 */
LIST_PUBLIC(void *) list_remove_tail(list_t *list);

/**
 * @brief Release list instance
 * @param list List instance
 */
LIST_PUBLIC(void) list_release(list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* __LIST_H__ */