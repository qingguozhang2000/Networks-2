#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./queue.h"

struct msg message(int i) {
	struct msg message;
	sprintf(message.data, "Test %d", i);
	return message;
}

int main()
{
	struct Queue* q = create_queue();

	enqueue(q, message(1));
	enqueue(q, message(2));
	dequeue(q);
	dequeue(q);
	enqueue(q, message(3));
	enqueue(q, message(4));
	enqueue(q, message(5));
	dequeue(q);

	printf("Queue Front : %s\n", ((q->front != NULL) ? (q->front)->message.data : ""));
	printf("Queue Rear : %s\n", ((q->rear != NULL) ? (q->rear)->message.data : ""));

	return 0;
}
