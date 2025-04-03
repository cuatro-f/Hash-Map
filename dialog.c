#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dialog.h"
#include "structs/table.h"
#include "structs/keyspace.h"
#include <readline/readline.h>

int get_key(char **key) {
        // дописать проверку
	*key = readline("Enter key: ");
    	if (*key == NULL) {
		return EOF;
 	}
    	return 0;
}

int get_value(size_t value) {
	return 1;
}

int get_index(size_t index) {
    return (index >= 0 && index <= 8);
}
int get_int(size_t* number, int(*func)(size_t), char* str) {
    int flag = 1;
    do {
        printf("%s: ", str);
        int check = scanf("%zu", number);
        if (check == EOF) {
            return check;
        }
        if (func(*number) == 1 && check == 1) {
            return 0;
        }
	printf("\n");
        scanf("%*[^\n]");
        scanf("%*c");
    } while (flag);
    return 1;
}

int dialog(const char* msgs[], const int len) {
    char* errmsg = "";
    int check = 0;
    size_t number = 0;
    do {
        puts(errmsg);
        errmsg = "Repeate input";
        for (int i = 0; i < len; i++) {
            puts(msgs[i]);
        }
        check = get_int(&number, &get_index, "choose one option");
        if (check == EOF) {
            return 0;
        }
    } while (number < 0 || number >= len);
    return number;
}

int process(Table* table) {
    const char* msgs[] = { "0. Quit", "1. Add", "2. Remove by key", "3. Find by key", "4. Print table", "5. Import from file",
    "6. Import to file", "7. Find by version", "8. Remove by version" };
    const int len = sizeof(msgs) / sizeof(msgs[0]);
    int check = 0;
    int flag = 1;
    int (*funcs[]) (Table*) = {&wraped_exit, &dialog_add, &dialog_remove_by_key, &dialog_find_by_key, &wraped_print,
    				&dialog_import_from_file, &dialog_import_to_file, &dialog_find_by_version, &dialog_remove_by_version}; 
    while (flag) {
        int index = dialog(msgs, len);
	check = funcs[index](table);
	if (check == -1) {
		flag = 0;
		return EOF;
	}
    }
}

int wraped_exit(Table* table) {
	return -1;
}

int wraped_print(Table *table) {
	print_table(table);
	return 0;
}

// переписать find_by_version
int dialog_remove_by_version(Table* table) {
	size_t version = 0;
	char *key = NULL;
	int check = get_key(&key);
	if (check == EOF) {
		return EOF;
	}
	check = get_int(&version, &get_value, "input version for removing");
    	if (check == EOF) {
		free(key);
            	return EOF;
    	}
    	check = remove_by_version(table, key, version);
    	free(key);
    	return check;
}

int dialog_import_from_file(Table *table) {
    scanf("%*c");
    char* filename = readline("Input filename ");
    if (filename == NULL) {
	printf("filename error\n");
	return 0;
    }
    for (int i = 0; i < strlen(filename); i++) {
		if (filename[i] == ' ') {
			filename[i] = '\0';
		}
    }
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("there is not file with this file name: %s\n", filename);
        free(filename);
        return 0;
    }
    fclose(file);
    int check = import_from_file(table, filename);
    free(filename);
    return check;
}

int dialog_import_to_file(Table *table) {
	scanf("%*c");
	char* filename = readline("Input filename ");
	if (filename == NULL) {
		printf("filename error\n");
		return 0;
	}
	for (int i = 0; i < strlen(filename); i++) {
		if (filename[i] == ' ') {
			filename[i] = '\0';
		}
	}
	FILE* file = fopen(filename, "wb");
	if (file == NULL) {
		printf("can not open file with this file name %s\n", filename);
		free(filename);
		return 0;
	}
	fclose(file);
	int check = import_to_file(table, filename);
	free(filename);
	return check;
}

int dialog_remove_by_key(Table *table) {
    char *key = NULL;
    int check = get_key(&key);
    if (check == EOF) {
        return EOF;
    }
    check = remove_by_key(table, key);
    free(key);
    return check;
}

int dialog_find_by_key(Table *table) {
    char *key = NULL;
    int check = get_key(&key);
    if (check == EOF) {
        return EOF;
    }
    Table *new_table = find_by_key(table, key);
    if (new_table == NULL) {
	free(key);
	return 0;
    }
    printf("new table:\n");
    print_table(new_table);
    free(new_table->ks[0]);
    free(new_table->ks);
    free(new_table);
    free(key);
    return 0;
}

int dialog_add(Table *table) {
    	size_t info_value = 0;
    	char* key = NULL;
   	int check = get_key(&key);
    	if (check == EOF) {
        	return EOF;
    	}
    	check = get_int(&info_value, &get_value, "input info");
    	if (check == EOF) {
	    	free(key);
        	return EOF;
    	}
    	check = insert(table, info_value, key);
    	if (check == 1) {
        	return -2;
    	}
    	return check;
}

int dialog_find_by_version(Table *table) {
	size_t version = 0;
	char* key = NULL;
	int check = get_key(&key);
	if (check == EOF) {
		return EOF;
	}
	check = get_int(&version, &get_value, "input version for search");
	if (check == EOF) {
		free(key);
		return EOF;
	}
	Node *new = find_by_version(table, key, version);
	free(key);
	if (new == NULL) {
		return 1;
	}
	printf("find: data %zu release %zu\n", new->info->data, new->release);
	return 0;
}


