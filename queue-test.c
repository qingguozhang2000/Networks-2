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
	struct Queue* q = createQueue();

	enQueue(q, message(1));
	enQueue(q, message(2));
	deQueue(q);
	deQueue(q);
	enQueue(q, message(3));
	enQueue(q, message(4));
	enQueue(q, message(5));
	deQueue(q);

	printf("Queue Front : %s\n", ((q->front != NULL) ? (q->front)->message.data : ""));
	printf("Queue Rear : %s\n", ((q->rear != NULL) ? (q->rear)->message.data : ""));

	return 0;
}
