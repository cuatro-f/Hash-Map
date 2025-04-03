#ifndef KEYSPACE_H
#define KEYSPACE_H

#include <stdlib.h>

typedef struct Item {
	size_t data;
} Item; 

// список значений с одинаковыми ключами
typedef struct Node {
	size_t release;
	Item *info;
	struct Node *next;
} Node;

typedef struct KeySpace {
	int busy;
	char *key;
	Node *node;
} KeySpace;

Item* create_item(size_t); //+
int push(Node**, size_t); //+
int remove_node(Node**, size_t); //+
//Node* create_node(Item*, Node*);
KeySpace* create_keyspace(); //+
void print_keyspace(KeySpace*); //+
void print_nodes(Node*); //+
void free_keyspace(KeySpace*); //+
void free_nodes(Node*); //+
int node_len(Node*);

#endif
