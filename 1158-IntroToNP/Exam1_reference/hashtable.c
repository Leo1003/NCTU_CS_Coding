/*
 * Original Author : Pierre-Henri Symoneaux
 * Source : https://gist.github.com/phsym/4605704
 */

#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

void * const HT_ERROR = (void *)-1;

/*  Internal funcion to calculate hash for keys.
    It's based on the DJB algorithm from Daniel J. Bernstein. */
static unsigned int ht_calc_hash(unsigned char *key, size_t key_size)
{
    unsigned int h = 5381;
    while (key_size--) {
        h = ((h << 5) + h) + (*key++);
    }
    return h;
}

hashtable_t *ht_create(size_t capacity, size_t key_size)
{
    if (key_size > MAX_KEYSIZE || key_size == 0) {
        return NULL;
    }
    hashtable_t *hasht = (hashtable_t *)malloc(sizeof(hashtable_t));
    if (hasht == NULL) {
        return NULL;
    }
    if ((hasht->table = (hash_elem_t **)malloc(capacity * sizeof(hash_elem_t *))) == NULL) {
        free(hasht);
        return NULL;
    }
    hasht->capacity = capacity;
    hasht->e_num = 0;
    hasht->key_size = key_size;
    size_t i;
    for (i = 0; i < capacity; i++) {
        hasht->table[i] = NULL;
    }
    return hasht;
}

void *ht_put(hashtable_t *hasht, void *key, void *data)
{
    if (data == NULL) {
        return NULL;
    }
    unsigned int h = ht_calc_hash((unsigned char *)key, hasht->key_size) % hasht->capacity;
    hash_elem_t *e = hasht->table[h];

    while (e != NULL) {
        if (!memcmp(e->key, key, hasht->key_size)) {
            void *ret = e->data;
            e->data = data;
            return ret;
        }
        e = e->next;
    }

    // Getting here means the key doesn't already exist

    if ((e = (hash_elem_t *)malloc(sizeof(hash_elem_t) + hasht->key_size)) == NULL) {
        return HT_ERROR;
    }
    memcpy(e->key, key, hasht->key_size);
    e->data = data;

    // Add the element at the beginning of the linked list
    e->next = hasht->table[h];
    hasht->table[h] = e;
    hasht->e_num++;

    return NULL;
}

void *ht_get(hashtable_t *hasht, void *key)
{
    unsigned int h = ht_calc_hash((unsigned char *)key, hasht->key_size) % hasht->capacity;
    hash_elem_t *e = hasht->table[h];
    while (e != NULL) {
        if (!memcmp(e->key, key, hasht->key_size)) {
            return e->data;
        }
        e = e->next;
    }
    return NULL;
}

void *ht_remove(hashtable_t *hasht, void *key)
{
    unsigned int h = ht_calc_hash((unsigned char *)key, hasht->key_size) % hasht->capacity;
    hash_elem_t *e = hasht->table[h];
    hash_elem_t *prev = NULL;
    while (e != NULL) {
        if (!memcmp(e->key, key, hasht->key_size)) {
            void *ret = e->data;
            if (prev != NULL) {
                prev->next = e->next;
            } else {
                hasht->table[h] = e->next;
            }
            free(e);
            e = NULL;
            hasht->e_num--;
            return ret;
        }
        prev = e;
        e = e->next;
    }
    return NULL;
}

void ht_list_keys(hashtable_t *hasht, unsigned char **k, size_t len)
{
    if (len < hasht->e_num) {
        return;
    }
    int ki = 0; //Index to the current string in **k
    int i = hasht->capacity;
    while (--i >= 0) {
        hash_elem_t *e = hasht->table[i];
        while (e) {
            k[ki++] = e->key;
            e = e->next;
        }
    }
}

void ht_list_values(hashtable_t *hasht, void **v, size_t len)
{
    if (len < hasht->e_num) {
        return;
    }
    int vi = 0; //Index to the current string in **v
    int i = hasht->capacity;
    while (--i >= 0) {
        hash_elem_t *e = hasht->table[i];
        while (e) {
            v[vi++] = e->data;
            e = e->next;
        }
    }
}

hash_elem_t *ht_iterate(hash_elem_it *iterator)
{
    while (iterator->elem == NULL) {
        if (iterator->index < iterator->ht->capacity - 1) {
            iterator->index++;
            iterator->elem = iterator->ht->table[iterator->index];
        } else {
            return NULL;
        }
    }
    hash_elem_t *e = iterator->elem;
    if (e) {
        iterator->elem = e->next;
    }
    return e;
}

unsigned char *ht_iterate_keys(hash_elem_it *iterator)
{
    hash_elem_t *e = ht_iterate(iterator);
    return (e == NULL ? NULL : e->key);
}

void *ht_iterate_values(hash_elem_it *iterator)
{
    hash_elem_t *e = ht_iterate(iterator);
    return (e == NULL ? NULL : e->data);
}

void ht_clear(hashtable_t *hasht, int free_data)
{
    hash_elem_it it = HT_ITERATOR(hasht);
    unsigned char *k = ht_iterate_keys(&it);
    while (k != NULL) {
        if (free_data) {
            free(ht_remove(hasht, k));
        } else {
            ht_remove(hasht, k);
        }
        k = ht_iterate_keys(&it);
    }
}

void ht_destroy(hashtable_t *hasht, int free_data)
{
    ht_clear(hasht, free_data);
    free(hasht->table);
    free(hasht);
}

