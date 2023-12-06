all: rdt3

rdt3: project2.c student2.c util.c
	gcc -g project2.c student2.c util.c -o rdt3

clean:
	rm rdt3
