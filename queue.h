// Adapted from https://www.geeksforgeeks.org/queue-linked-list-implementation/s
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./project2.h"

struct QNode {
	struct msg message;
	struct QNode *next;
};

struct Queue {
	struct QNode *front, *rear;
};

struct QNode* create_node(struct msg message);
struct Queue* create_queue();
void enqueue(struct Queue* q, struct msg message);
struct msg* dequeue(struct Queue* q);
