// Adapted from https://www.geeksforgeeks.org/queue-linked-list-implementation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./queue.h"

// A utility function to create a new linked list node.
struct QNode* create_node(struct msg message)
{
	struct QNode* p_node = (struct QNode*) malloc(sizeof(struct QNode));

	memcpy(p_node->message.data, message.data, MESSAGE_LENGTH);
	p_node->next = NULL;

	return p_node;
}

// A utility function to create an empty queue
struct Queue* create_queue()
{
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;

	return q;
}

// The function to add a key k to q
void enqueue(struct Queue* q, struct msg message)
{
	// Create a new LL node
	struct QNode* p_node = create_node(message);

	// If queue is empty, then new node is front and rear
	// both
	if (q->rear == NULL) {
		q->front = q->rear = p_node;
		return;
	}

	// Add the new node at the end of queue and change rear
	q->rear->next = p_node;
	q->rear = p_node;
}

// Function to remove a key from given queue q
struct msg* dequeue(struct Queue* q)
{
	// Store previous front
	struct QNode* node = q->front;

	// If queue is empty, return NULL.
	if (node == NULL) {
		return NULL;
	}

	// move front one node ahead
	q->front = node->next;

	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL) {
		q->rear = NULL;
	}

	// free(node);
	return &(node->message);
}
