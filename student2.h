#include "./queue.h"

extern int TraceLevel;

#define DEFAULT_ACK_NUM 0
#define TIMEOUT 20.0

enum sender_mode {
	WAIT_FOR_MSG, // wait for message from app (layer 5)
	WAIT_FOR_ACK  // wait for acknowledge packet from layer 3
};

struct current_state {
	int seqnum;// current expected seq
	enum sender_mode senderMode;// sender's current state
};

struct current_state A_state, B_state;
struct pkt current_packet;
struct Queue* p_message_queue;

struct pkt make_pkt(const char data[MESSAGE_LENGTH], int seqNum, int ackNum);
int calc_checksum(const struct pkt *p);
void debug_log(char *function_name, char *log_message);
