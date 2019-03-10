﻿#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Array.h"


typedef struct {
	char *key;  
	void *data; 
} Entry;

Entry* EntryNew(char *key, void *data); 
int EntryCompare(Entry *e1, Entry *e2); 
void EntryFree(Entry *e);

int hash(char *key, int range); 

#define HashTable Array 


void HashTableTest(); 
HashTable* HashTableNew(int size); 
void HashTableFree(HashTable *table); 
void HashTablePut(HashTable *table, char *key, void *data); 
void *HashTableGet(HashTable *table, char *key); 
void HashTableEach(HashTable *table, FuncPtr1 f); 
Array* HashTableToArray(HashTable *table); 

#endif
