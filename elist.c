#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "elist.h"

#define DEFAULT_INIT_SZ 10
#define RESIZE_MULTIPLIER 2

struct elist {
    size_t capacity;         /*!< Storage space allocated for list items */
    size_t size;             /*!< The actual number of items in the list */
    size_t item_sz;          /*!< Size of the items stored in the list */
    void *element_storage;   /*!< Pointer to the beginning of the array */
};

bool idx_is_valid(struct elist *list, size_t idx);

/**
 * Creates a new elist using the given size (or the default if set to 0) and
 * element size. 
 */
struct elist *elist_create(size_t list_sz, size_t item_sz)
{
    struct elist *new_elist;
    new_elist = (void*) malloc(sizeof(struct elist));
    if(new_elist == NULL){
        free(new_elist);
        return NULL;
    }
    if(list_sz == 0){
        list_sz = DEFAULT_INIT_SZ;
    }
    new_elist->size = 0;
    new_elist->item_sz = item_sz;
    new_elist->element_storage = (void*) malloc(list_sz * item_sz);
    new_elist->capacity = list_sz;
    if(new_elist->element_storage == NULL){
        free(new_elist);
        return NULL;
    }
    return new_elist;
}

/**
 * Frees memory for the specified elist.
 */
void elist_destroy(struct elist *list)
{
    free(list->element_storage);
    free(list);
}

/**
 * Sets the capacity for an elist. This can grow or shrink the amount of memory
 * set aside for element storage.
 * 
 * @return -1 on failure, 0 on success
 */
int elist_set_capacity(struct elist *list, size_t capacity)
{
    if(capacity <= 0){
        capacity = 1;
        list->size = 0;
    }

    if((list->element_storage = realloc(list->element_storage, capacity * list->item_sz)) != NULL){
        list->capacity = capacity;
        if(list->capacity < list->size){
            list->size = list->capacity;
        }
        return 0;
    }else{
        free(list->element_storage);
    }
    return -1;
}

/**
 * Retrieves the elist capacity (amount of space set aside for elements in
 * memory).
 */
size_t elist_capacity(struct elist *list)
{
    return list->capacity;
}

/**
 * Adds an item to the end of the list.
 * 
 * @return -1 on failure, 0 on success
 */
ssize_t elist_add(struct elist *list, void *item)
{
    // if(list->size >= list->capacity){
    //     if(elist_set_capacity(list, list->capacity * RESIZE_MULTIPLIER) == -1){
    //         return -1;
    //     }
    // }
    memcpy(elist_add_new(list), item, list->item_sz);
    return 0;
}

/**
 * Allocates memory for an item, adds it to the end of the list, and returns a
 * pointer to the newly-allocated memory.
 * 
 * @return -1 on failure, 0 on success
 */

void *elist_add_new(struct elist *list)
{
    return list->element_storage + list->size++ * list->item_sz;
}

/**
 * Copies an item into the list at the specified index.
 *
 * @return -1 on failure, 0 on success
 */
int elist_set(struct elist *list, size_t idx, void *item)
{
    if(idx_is_valid(list, idx)){
        memcpy(list->element_storage + idx * list->item_sz, item, list->item_sz);
        return 0;
    }
    return -1;
}

/**
 * Retrieves a particular item from the list based on its index.
 * 
 * @return NULL if the index does not exist or is invalid
 */
void *elist_get(struct elist *list, size_t idx)
{
    if(idx >= list->size){
        return NULL;
    }
    return list->element_storage + idx * list->item_sz;
}

size_t elist_size(struct elist *list)
{
    return list->size;
}

/**
 * Removes an item from the list.
 *
 * @return -1 on failure, 0 on success
 */
int elist_remove(struct elist *list, size_t idx)
{
    if(idx_is_valid(list, idx)){
        memmove(list->element_storage + idx * list->item_sz,
         list->element_storage + (idx + 1) * list->item_sz, list->item_sz * (list->size - idx));
         list->size--;
        return 0;
    }
    return -1;
}

/**
 * "Clears" the array by resetting its size to 0.
 */
void elist_clear(struct elist *list)
{
    list->size = 0;
}

/**
 * Clears the array AND zeroes out its memory.
 */
void elist_clear_mem(struct elist *list)
{
    // list = calloc(list->size, list->size * list->item_sz);
    memset(list->element_storage, 0, list->item_sz *list->size);
    list->size = 0;
}

/**
 * Sorts the list using the given comparator. This function does not actually
 * implement a sorting algorithm, but simply acts as interface to the qsort
 * function.
 */
void elist_sort(struct elist *list, int (*comparator)(const void *, const void *))
{
    qsort(list->element_storage, list->size, list->item_sz, comparator);
}

bool idx_is_valid(struct elist *list, size_t idx)
{
    if(idx >= list->size){
        return false;
    }
    return true;
}