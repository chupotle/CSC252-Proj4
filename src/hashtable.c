#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
/*
   Thomas Jeffries
   Timothy Chu
 */
typedef struct hashtable table;
table* ht_rehash(table* hashtable, int new_capacity);
table* ht_create(int capacity) {
        table* hashTable = NULL;
        int i;
        if(capacity < 1) {return NULL; }
        if((hashTable = malloc(sizeof(table))) == NULL) {return NULL; }
        if( ( hashTable->table = malloc(sizeof(int) * capacity )) == NULL ) {return NULL; }
        for( i = 0; i < capacity; i++ ) {hashTable->table[i] = -1000; }
        hashTable->capacity = capacity;
        hashTable->size = 0;
        return hashTable;
}

int ht_hash(table* hashtable, int key ) {
        unsigned long int hashval;
        int i = 0;
        hashval = key;
        return hashval % hashtable->capacity;
}

table* ht_set(table* hashtable, int key ) {
        if(ht_contains(hashtable, key) == 0) {
                int bin,start;
                bin = ht_hash( hashtable, key );
                start = bin - 1;
                if(((double) hashtable->size/ (double) hashtable->capacity) > (2.0/3.0)) {
                        table* newTable =  ht_rehash(hashtable, (hashtable->capacity * 3) + 1);
                        hashtable = newTable;
                }
                while (bin != start) {
                        if(hashtable->table[bin] == -1000) {
                                hashtable->table[bin] = key;
                                break;
                        }
                        else {
                                bin = (bin+1)%hashtable->capacity;
                        }
                }
                hashtable->size = hashtable->size + 1;
                return hashtable;
        }
        else {
                return hashtable;
        }
}

table* ht_rehash(table* hashtable, int new_capacity) {
        int i;
        table* hashtable2 = ht_create(new_capacity);
        for(i = 0; i < hashtable->capacity; i++)
                if(hashtable->table[i] != -1000)
                        ht_set(hashtable2, hashtable->table[i]);
        return hashtable2;
}

int ht_contains(table* hashtable, int key) {
        int val,i;
        val = ht_hash(hashtable, key);
        i = val;
        val = (val - 1) % hashtable->capacity;
        if(val == -1) {
                val = hashtable->capacity - 1;
        }
        while(i != val) {
                if(hashtable->table[i] == key) {
                        return 1;
                }
                else {
                        i = ((i+1)%hashtable->capacity);
                }
        }
        return 0;
}
