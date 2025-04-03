#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "table.h"
#include "keyspace.h"

// удаление по ключу
int by_key(KeySpace* keyspace, char* element) {
	return strcmp(keyspace->key, element);
}

int first_hash(char *key) {
	int hash = 0;
	char *str = key;
	while (*str) {
        	hash ^= *str;
        	hash = (hash << 3) ^ (hash >> 13) ^ (hash >> 5);
        	str++;
    }
	return hash;
}

// hash 2 - не равен нулю
// hash 2 - прост по отношению к table_size
int second_hash(size_t msize, char *key) {
    	int hash = 0;
    	int i = 0;
	char *str = key;
    	while (*str) {
        	hash = (hash << 7) ^ (hash >> 25) ^ (hash >> 23);
        	hash = hash ^ ((*str) << 3);
        	str++;
    }
	if (hash == 0) {
		hash++;
	}
	while (hash % msize == 0) {
		hash++;
	}
    	return hash;
}

int common_hash(size_t msize, char* key, int iteration) {
	int first = first_hash(key), second = second_hash(msize, key);
	return first + second * iteration;
	
}

int find(Table *table, char* key) {
	int index = first_hash(key) % table->msize;
	int check = 0, new_position = index, iteration = 1;
	do {
		if (table->ks[new_position]->busy == 0) {
			return 0;
		}
		if (table->ks[new_position]->busy == -1) {
			new_position = common_hash(table->msize, key, iteration) % table->msize;
			iteration++;
			continue;
		}
		if (strcmp(table->ks[new_position]->key, key) == 0) {
			return 1;
		}
		new_position = common_hash(table->msize, key, iteration) % table->msize;
		iteration++;
	} while (index != new_position);
	return 0;
}

int insert(Table *table, size_t element, char *key) {
	if (table->msize == table->csize && find(table, key) == 0) {
		int check = expansion(table);
	}
	int index = first_hash(key) % table->msize;
	int check = 0, new_position = index, iteration = 1;
	do {
		// место не занято
		if (table->ks[new_position]->busy == 0 || table->ks[new_position]->busy == -1) {
			table->ks[new_position]->busy = 1;
			table->ks[new_position]->key = key;
			check = push(&table->ks[new_position]->node, element);
			(table->csize)++;
			return 0;
		}
		// место занаято, но ключи совпадают => доюавить в list релизов 
		else if (strcmp(table->ks[index]->key, key) == 0) {
			check = push(&table->ks[new_position]->node, element);
			free(key);
			return 0;
		}
		new_position = common_hash(table->msize, key, iteration) % table->msize;
		iteration++;
	} while (new_position != index);
	free(key);
	// не получилось вставить
	return 1;
}

Table* create_table(size_t msize) {
	Table *table = calloc(1, sizeof(Table));
	if (table == NULL) {
		return NULL;
	}
	table->ks = calloc(msize, sizeof(KeySpace*));
	if (table->ks == NULL) {
		free(table);
		return NULL;
	}
	for (int i = 0; i < msize; i++) {
		table->ks[i] = create_keyspace();
	}
	table->csize = 0;
	table->msize = msize;
	return table;
}

void print_table(Table* table) {
	for (int i = 0; i < table->msize; i++) {
		if (table->ks[i]->busy == 1) {
			print_keyspace(table->ks[i]);
		}
		else {
			printf("NULL\n");
		}
	}
}

void free_table(Table *table) {
	for (int i = 0; i < table->msize; i++) {
		free_keyspace(table->ks[i]);
	}
	free(table->ks);
	free(table);
}

int remove_by_key(Table *table, char *key) {
	int index = first_hash(key) % table->msize;
	int new_position = index, iteration = 1;
	do {
		if (table->ks[new_position]->busy == 0) {
			return 1;
		}
		else if (table->ks[new_position]->busy == -1) {
			new_position = common_hash(table->msize, key, iteration) % table->msize;
			iteration++;
			continue;
		}
		if (strcmp(table->ks[new_position]->key, key) == 0) {
			// busy = 1 and ks->key = key
			table->ks[new_position]->busy = -1;
			free(table->ks[new_position]->key);
			table->ks[new_position]->key = NULL;
			free_nodes(table->ks[new_position]->node);
			table->ks[new_position]->node = NULL;
			(table->csize)--;
			return 0;
		}
	} while (new_position != index);
	// не удалось убрать элемент
	return 1;
}

int remove_by_version(Table* table, char* key, size_t version) {
	int index = first_hash(key) % table->msize;
	int new_position = index, iteration = 1, check = 0;
	do {
		if (table->ks[new_position]->busy == 0) {
			return 1;
		}
		else if (table->ks[new_position]->busy == -1){
			new_position = common_hash(table->msize, key, iteration) % table->msize;
			iteration++;
			continue;
		}
		if (strcmp(table->ks[new_position]->key, key) == 0) {
			// нашли такой же ключ
			check = remove_node(&(table->ks[new_position]->node), version);
			if (table->ks[new_position]->node == NULL) {
				table->ks[new_position]->busy = -1;
				free(table->ks[new_position]->key);
				table->ks[new_position]->key = NULL;
				table->csize--;
			}
			return check;
		}
	} while(new_position != index);
	// нет такого ключа
	return 1;
}

Table* find_by_key(Table *table, char *key) {
	Table *new = create_table(1);
	int index = first_hash(key) % table->msize;
	int new_position = index, iteration = 1;
	do {
		if (table->ks[new_position]->busy == 0) {
			return NULL;
		}
		else if (table->ks[new_position]->busy == -1) {
			new_position = common_hash(table->msize, key, iteration) % table->msize;
			iteration++;
			continue;
		}
		if (strcmp(table->ks[new_position]->key, key) == 0) {
			//memcpy(new->ks[0], table->ks[new_position], sizeof(KeySpace));
			*new->ks[0] = *(table->ks[new_position]);
			new->csize = 1;
			return new;
		}
		new_position = common_hash(table->msize, key, iteration) % table->msize;
		iteration++;
	} while (new_position != index);
	free_table(new);
	return NULL;
}

Node* find_by_version(Table *table, char *key, size_t version) {
	Table* new = find_by_key(table, key);
	if (new == NULL) {
		return NULL;
	}
	Node *head = new->ks[0]->node, *result = NULL;
	while (head) {
		if (head->release == version) {
			printf("find\n");
			free(new->ks[0]);
			free(new->ks);
			free(new); 
			return head;
		}
		head = head->next;
	}
	free(new->ks[0]);
	free(new->ks);
	free(new);
	return NULL;
}

int import_from_file(Table *table, char *filename) {
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		return 1;
	}
	for (int i = 0; i < table->msize; i++) {
		free_keyspace(table->ks[i]);
	}
	free(table->ks);
	// читаем msize
	size_t msize = 0;
	int check = fread(&msize, sizeof(size_t), 1, file);
	if (check != 1) {
		return 1;
	}
	table->msize = msize;
	table->ks = calloc(table->msize, sizeof(KeySpace*));
	for (int i = 0; i < table->msize; i++) {
		table->ks[i] = create_keyspace();
	}
	// читаем csize
	check = fread(&(table->csize), sizeof(size_t), 1, file);
	if (check != 1) {
		return 1;
	}
	for (int i = 0; i < msize; i++) {
		// читаем busy
		check = fread(&(table->ks[i]->busy), sizeof(int), 1, file);
		if (check != 1) {
			return 1;
		}
		printf("busy %d\n", table->ks[i]->busy);
		if (table->ks[i]->busy == 0 || table->ks[i]->busy == -1) {
			continue;
		}
		int key_len = 0;
		// читаем длину ключа
		check = fread(&key_len, sizeof(int), 1, file);
		if (check != 1) {
			return 1;
		}
		printf("key len %d\n", key_len);
		// читаем ключ
		table->ks[i]->key = calloc(key_len + 1, sizeof(char));
		check = fread(table->ks[i]->key, sizeof(char), key_len, file);
		printf("key %s\n", table->ks[i]->key);
		if (check != key_len) {
			return 1;
		}
		// считываем длину списка
		int node_len = 0;
		check = fread(&node_len, sizeof(int), 1, file);
		if (check != 1) {
			return 1;
		}
		printf("node len %d\n", node_len);
		for (int j = 0; j < node_len; j++) {
			// считываем info
			size_t info = 0;
			check = fread(&info, sizeof(size_t), 1, file);
			if (check != 1) {
				return 1;
			}
			check = push(&(table->ks[i]->node), info);
			// считываем release
			check = fread(&(table->ks[i]->node->release), sizeof(size_t), 1, file);
			if (check != 1) {
				return 1;
			}
			printf("nodes\n");
			print_nodes(table->ks[i]->node);
			printf("_______\n");
		}
	}
	fclose(file);
	return 0;
}

int import_to_file(Table *table, char *filename) {
	FILE *file = fopen(filename, "wb");
	if (file == NULL) {
		return 1;
	}
	// записываем msize
	int check = fwrite(&(table->msize), sizeof(size_t), 1, file);
	if (check != 1) {
		return 1;
	}
	// записываем csize
	check = fwrite(&(table->csize), sizeof(size_t), 1, file);
	if (check != 1) {
		return 1;
	}
	for (int i = 0; i < table->msize; i++) {
		// записываем keyspace
		// записываем busy
		check = fwrite(&table->ks[i]->busy, sizeof(int), 1, file);
		if (check != 1) {
			return 1;
		}
		if (table->ks[i]->busy == 0) {
			continue;
		}
		// записываем длину ключа
		int len = strlen(table->ks[i]->key);
		printf("len eq %d\n", len);
		check = fwrite(&len, sizeof(int), 1, file);
		if (check != 1) {
			return 1;
		}
		// записываем ключ
		check = fwrite(table->ks[i]->key, sizeof(char), len, file);
		if (check != len) {
			return 1;
		}
		if (table->ks[i]->busy == 0) {
			continue;
		}
		// записываем длину списка
		int nodes_len = node_len(table->ks[i]->node);
		check = fwrite(&nodes_len, sizeof(int), 1, file);
		if (check != 1) {
			return 1;
		}
		// записываем список
		Node *head = table->ks[i]->node;
		while (head) {
			// записываем info
			check = fwrite(&head->info->data, sizeof(size_t), 1, file);
			if (check != 1) {
				return 1;
			}
			// записываем release
			check = fwrite(&head->release, sizeof(size_t), 1, file);
			if (check != 1) {
				return 1;
			}
			head = head->next;
		}
	}
	fclose(file);
	return 0;
}

int is_prime(size_t number) {
	if (number <= 1) {
		return 0;
	}
	for (size_t i = 2; i < sqrt(number) + 1; i++) {
		if (number % i == 0) {
			return 0;
		}
	}
	return 1;
}

int expansion(Table *table) {
	size_t new = (table->msize) + 1;
	while (is_prime(new) != 1) {
		new++;
	}
	Table *new_table = create_table(new);
	for (int i = 0; i < table->msize; i++) {
		if (table->ks[i]->busy == 1) {
			Node *node = table->ks[i]->node;
			char *key = strdup(table->ks[i]->key);
			while (node) {
				int check = insert(new_table, node->info->data, key);
				node = node->next;
			}
		}
		free_keyspace(table->ks[i]);
	}
	free(table->ks);
	table->msize = new_table->msize;
	table->csize = new_table->csize;
	table->ks = new_table->ks;
	free(new_table);
	return 0;
}
