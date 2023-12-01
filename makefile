p2: student2A.c student2B.c student_common.c project2.c
	gcc -g project2.c student2A.c student2B.c student_common.c -o p2

clean:
	rm *.o p2
