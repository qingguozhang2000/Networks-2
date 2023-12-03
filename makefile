all: p2 queue

p2: project2.c student2.c queue.c
	gcc -g project2.c student2.c queue.c -o p2

queue: queue.c queue-test.c
	gcc -g queue.c queue-test.c -o queue

clean:
	rm p2 queue
