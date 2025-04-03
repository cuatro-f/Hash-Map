#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>
#include "keyspace.h"

typedef struct Table {
	KeySpace **ks;
	size_t msize;
	size_t csize;
} Table;

int first_hash(char*); //+ придумать что-то поинтереснее
int second_hash(size_t, char*); //+ тоже что-то интереснее
int common_hash(size_t, char*, int); //+
Table* create_table(size_t); //+
int insert(Table*, size_t, char*); // +
// удаление по ключу
int remove_by_key(Table*, char*); //+
// удаление по версии
int remove_by_version(Table*, char*, size_t); //+
// поиск возвращает таблицу
Table* find_by_key(Table*, char*); // +
Node* find_by_version(Table*, char*, size_t);
// +
void print_table(Table*); //+
int import_from_file(Table*, char*);
int import_to_file(Table*, char*);
void free_table(Table*); //+
size_t find_step(size_t); //+
int find(Table*, char*);
int expansion(Table *table);
#endif
