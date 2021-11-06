#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _List list_t;


list_t *list_construct(size_t width, size_t limit, bool (*clone)(const void *source, void *destination), void (*delete)(void *data));
list_t *list_clone(list_t *list);
list_t *list_reverse(list_t *list);

void list_free(list_t *list);
bool list_clear(list_t *list);

bool list_insert(list_t *list, size_t index, const void *data);
bool list_remove(list_t *list, size_t index, void *destination);
bool list_removeall(list_t *list, const void *remove, int (*compare)(const void *data, const void *remove));

bool list_get(list_t *list, size_t index, void *destination);
bool list_set(list_t *list, size_t index, const void *data);

bool list_sort(list_t *list, bool reverse, int (*compare)(const void *first, const void *second));

bool list_contains(list_t *list, const void *key, int (*compare)(const void *data, const void *key));
size_t list_search(list_t *list, const void *key, int (*compare)(const void *data, const void *key));

size_t list_size(list_t *list);
size_t list_width(list_t *list);
size_t list_limit(list_t *list);

bool list_empty(list_t *list);
bool list_full(list_t *list);

#endif // LIST_H
