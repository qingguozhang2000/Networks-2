p2: student2.c project2.c
	gcc -g -Wall project2.c student2.c utilities.c -o project

clean:
	rm project
