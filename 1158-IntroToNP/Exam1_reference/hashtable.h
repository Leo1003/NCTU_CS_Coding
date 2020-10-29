#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>

#define MAX_KEYSIZE 256

//Hashtable element structure
typedef struct hash_elem_t {
    struct hash_elem_t *next; // Next element in case of a collision
    void *data; // Pointer to the stored element
    unsigned char key[]; // Key of the stored element
} hash_elem_t;

//Hashtabe structure
typedef struct {
    size_t capacity; // Hashtable capacity (in terms of hashed keys)
    size_t e_num; // Number of element currently stored in the hashtable
    size_t key_size;
    hash_elem_t **table; // The table containaing elements
} hashtable_t;

//Structure used for iterations
typedef struct {
    hashtable_t *ht; // The hashtable on which we iterate
    unsigned int index; // Current index in the table
    hash_elem_t *elem; // Curent element in the list
} hash_elem_it;

void * const HT_ERROR = (void *)-1;

// Inititalize hashtable iterator on hashtable 'ht'
#define HT_ITERATOR(ht)     \
    {                       \
        ht, 0, ht->table[0] \
    }

/*  Create a hashtable with capacity 'capacity'
    and return a pointer to it*/
hashtable_t *ht_create(size_t capacity, size_t key_size);

/* Retrieve data from the hashtable */
void *ht_get(hashtable_t *hasht, void *key);

/*  Store data in the hashtable. If data with the same key are already stored,
    they are overwritten, and return by the function. Else it return NULL.
    Return HT_ERROR if there are memory alloc error*/
void *ht_put(hashtable_t *hasht, void *key, void *data);

/*  Remove data from the hashtable. Return the data removed from the table
    so that we can free memory if needed */
void *ht_remove(hashtable_t *hasht, void *key);

/* List keys. k should have length equals or greater than the number of keys */
void ht_list_keys(hashtable_t *hasht, unsigned char **k, size_t len);

/*  List values. v should have length equals or greater
    than the number of stored elements */
void ht_list_values(hashtable_t *hasht, void **v, size_t len);

/* Iterate through table's elements. */
hash_elem_t *ht_iterate(hash_elem_it *iterator);

/* Iterate through keys. */
unsigned char *ht_iterate_keys(hash_elem_it *iterator);

/* Iterate through values. */
void *ht_iterate_values(hash_elem_it *iterator);

/*  Removes all elements stored in the hashtable.
    if free_data, all stored datas are also freed.*/
void ht_clear(hashtable_t *hasht, int free_data);

/*  Destroy the hash table, and free memory.
    Data still stored are freed*/
void ht_destroy(hashtable_t *hasht, int free_data);

#endif

