#ifndef DIALOG_H
#define DIALOG_H
#include <stdlib.h>
#include "structs/keyspace.h"
#include "structs/table.h"

int wraped_exit(Table*); //+
int dialog_remove_by_version(Table*); //+
int dialog_find_by_version(Table*);
int dialog_import_from_file(Table*); //+
int dialog_import_to_file(Table*); //+
int dialog_remove_by_key(Table*); //+
int dialog_add(Table*); //+
int wraped_print(Table*); //+:w
int dialog_find_by_key(Table*); // +
int process(Table*);
int get_key(char**);
int get_index(size_t);
int get_value(size_t);
int get_int(size_t*, int(*)(size_t), char*);
int dialog(const char**, const int);
#endif 
