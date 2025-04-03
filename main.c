#include <stdio.h>
#include <stdlib.h>
#include "structs/table.h"
#include "dialog.h"
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
	size_t table_size = 0;
	if (argc < 2) {
		printf("You did not enter table size, using standart size: 10\n");
		table_size = 10;
	}
	else {
		for (int i = 0; i < strlen(argv[1]); i++) {
			if (isdigit(argv[1][i]) == 0) {
				printf("wrong argument. The first argumen is table size.\n");
				return 1;
			}
		}
		table_size = atoi(argv[1]);
		if (table_size <= 1) {
			printf("table size should be bigger than one. Using standart size: 10\n");
			table_size = 10;
		}
	}
	if (table_size == 0) {
		printf("Table size should be bigger than zero.\n");
		return 1;
	}
	Table *table = create_table(table_size);
	if (table == NULL) {
		return 1;
	}
	int check = process(table);
	free_table(table);
	return 0;
}
