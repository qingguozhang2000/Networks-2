#include "./student2.h"

extern int TraceLevel; // this global log level setting is defined in project2.h and used here

/**
 * For a given packet, this calculates its checksum.
 */
int calc_checksum(const struct pkt *p_packet)
{
	int seqnum = p_packet->seqnum;
	int acknum = p_packet->acknum;
	const char *payload = p_packet->payload;

	int i, checksum = 0;

	if (payload != NULL)
	{
		for (i = 0; i < MESSAGE_LENGTH; i++)
		{
			checksum += (int)(payload[i]) * i;
		}
	}

	checksum += acknum * 21;
	checksum += seqnum * 22;

	return checksum;
}

/**
 * For a given message, seqnum, and acknum, this builds a packet with checksum
 * calculated.
 */
struct pkt make_packet(const char message[MESSAGE_LENGTH], int seqnum, int acknum)
{
	struct pkt packet = {seqnum, acknum, 0, ""};
	strncpy(packet.payload, message, MESSAGE_LENGTH);
	packet.checksum = calc_checksum(&packet);
	return packet;
}

/**
 * For a given packet, this compares its current, actual checksum with the
 * expected checksum indicated by sender; if the two do not match, it is
 * an indication that the packet is corrupted.
 */
int packet_corrupted(const struct pkt *p_packet)
{
	int actual_checksum = calc_checksum(p_packet);
	return (actual_checksum != p_packet->checksum) ? TRUE : FALSE;
}

/**
 * This utility function logs debug info; log level set at 2 and above per
 * instruction from project requirement.
 */
void debug_log(char *p_function_name, char *p_log_message)
{
	if (TraceLevel >= 2)
	{
		printf("***** In %s: %s *****\n", p_function_name, p_log_message);
	}
}

/**
 * Message queue functions
 * Adapted from https://www.geeksforgeeks.org/queue-linked-list-implementation
 */

/**
 * For a given message, this utility function creates a new linked list node.
 */
struct QNode *create_node(struct msg message)
{
	struct QNode *p_node = (struct QNode *)malloc(sizeof(struct QNode));

	memcpy(p_node->message.data, message.data, sizeof(message.data));
	p_node->next = NULL;

	return p_node;
}

/**
 * This utility function creates an empty queue; caller is responsible for
 * freeing the allocated memory for the queue.
 */
struct Queue *create_queue()
{
	struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;

	return q;
}

/**
 * This function adds a message to a queue.
 */
void enqueue(struct Queue *q, struct msg message)
{
	// create a new node from the given message
	struct QNode *p_node = create_node(message);

	if (q->rear == NULL)
	{
		// for empty queue, the new node is both front and rear
		q->front = q->rear = p_node;
	}
	else
	{
		// Add the new node at the end of queue and change rear
		q->rear->next = p_node;
		q->rear = p_node;
	}
}

/**
 * This function removes from a queue the front message and returns it. For
 * empty queue, it returns NULL.
 */
struct msg *dequeue(struct Queue *q)
{
	// Store previous front
	struct QNode *node = q->front;

	// If queue is empty, return NULL.
	if (node == NULL)
	{
		return NULL;
	}

	// move front one node ahead
	q->front = node->next;

	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL)
	{
		q->rear = NULL;
	}

	// free node message;
	return &(node->message);
}
