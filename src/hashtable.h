#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
   Thomas Jeffries
   Timothy Chu
 */
struct hashtable {
        int capacity;
        int size;
        int * table;
};

typedef struct hashtable table;

table *ht_create(int capacity);
table* ht_set( table *hashtable, int key );
int ht_contains(table * hashtable, int key);
