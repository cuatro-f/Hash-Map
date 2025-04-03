#include <stdlib.h>
#include <stdio.h>
#include "keyspace.h"
#include "table.h"

int push(Node **node, size_t element) {
	Node *new = calloc(1, sizeof(Node));
	if (new == NULL) {
		return 1;
	}
	new->info = create_item(element);
	new->release = (*node == NULL) ? 1 : (*node)->release + 1;
	new->next = *node;
	*node = new;
	return 0;
}

int remove_node(Node** node, size_t version) {
	Node *current = *node, *prev = NULL;
	if (current == NULL) {
		return 1;
	}
	while (current) {
		if (current->release == version) {
			free(current->info);
			if (prev == 0) {
				*node = current->next;
				free(current);
			}
			else {
				prev->next = current->next;
				free(current);
			}
			return 0;
		}
		prev = current;
		current = current->next;
	}
	return 1;
}

Item* create_item(size_t data) {
	Item* item = calloc(1, sizeof(Item));
	if (item == NULL) {
		return NULL;
	}
	item->data = data;
	return item;
}

// создается пустой keyspace
KeySpace* create_keyspace() {
	KeySpace* keyspace = calloc(1, sizeof(KeySpace));
	if (keyspace == NULL) {
		return NULL;
	}
	keyspace->busy = 0;
	keyspace->key = NULL;
	keyspace->node = NULL;
	return keyspace;
}

int node_len(Node* node) {
	int count = 0;
	while (node) {
		node = node->next;
		count++;
	}
	return count;
}

void print_nodes(Node* node) {
	Node *head = node;
	printf(" releases list: ");
	while (head) {
		printf("release %zu info %zu ", head->release, head->info->data);
		head = head->next;
	}
	printf("\n");
}

void print_keyspace(KeySpace* keyspace) {
	printf("busy %zu key %s ", keyspace->busy, keyspace->key);
	print_nodes(keyspace->node);
}

void free_nodes(Node* node) {
	while (node) {
		free(node->info);
		Node* prev = node;
		node = node->next;
		free(prev);
	}
}

void free_keyspace(KeySpace *keyspace) {
	if (keyspace->busy == 1) {
		free(keyspace->key);
		free_nodes(keyspace->node);
	}
	free(keyspace);
}
