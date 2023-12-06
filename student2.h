#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./project2.h"

#define DEFAULT_ACK_NUM 0
#define TIMEOUT 30

struct QNode
{
	struct msg message;
	struct QNode *next;
};

struct Queue
{
	struct QNode *front, *rear;
};

enum WaitingMode
{
	WAIT_FOR_MSG, // wait for message from layer 5 (app layer)
	WAIT_FOR_ACK, // wait for acknowledge packet from layer 3 (transport layer)
	WAIT_FOR_PKT  // wait for packet from layer 3 (more for B)
};

struct CurrentState
{
	enum WaitingMode waiting_mode; // what A or B is waiting for
	int seqnum;					   // A or B's current expected sequence number
};

int calc_checksum(const struct pkt *p_packet);
struct pkt make_packet(const char message[MESSAGE_LENGTH], int seqnum, int acknum);
int packet_corrupted(const struct pkt *p_packet);
void debug_log(char *p_function_name, char *p_log_message);

struct QNode *create_node(struct msg message);
struct Queue *create_queue();
void enqueue(struct Queue *q, struct msg message);
struct msg *dequeue(struct Queue *q);
