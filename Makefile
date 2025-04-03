all:
	gcc -o main main.c structs/table.c structs/keyspace.c dialog.c -lm -lreadline
outfile:
	gcc -g -o main.out main.c structs/table.c structs/keyspace.c dialog.c -lm -lreadline
clean:
	rm -rf main
	rm -rf *.bin

