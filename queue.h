// Adapted from https://www.geeksforgeeks.org/queue-linked-list-implementation/s
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./project2.h"

// A linked list (LL) node to store a queue entry
struct QNode {
	struct msg message;
	struct QNode* next;
};

// The queue, front stores the front node of LL and rear
// stores the last node of LL
struct Queue {
	struct QNode *front, *rear;
};

// A utility function to create a new linked list node.
struct QNode* newNode(struct msg message);

// A utility function to create an empty queue
struct Queue* createQueue();

// The function to add a key k to q
void enQueue(struct Queue* q, struct msg message);

// Function to remove a key from given queue q
struct msg* deQueue(struct Queue* q);

