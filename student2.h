#include "./queue.h"

extern int TraceLevel;

double TIMER_INCREMENT = 1000.0;

#define ACK_ABP_DEFAULT 0
#define TIMEOUT 20.0

struct current_state A_state, B_state;
struct pkt current_packet;

enum sender_mode {
	WAITING_FOR_PKT, WAITING_FOR_ACK
};

struct current_state {
	int seqnum;// current expected seq
	enum sender_mode senderMode;// sender's current state
	int n_layer3;// for stat
	int n_layer5;// for stat
};

struct Queue* p_message_queue;

int calc_checksum(const struct pkt *p);
int calculateChecksum(int seqnum, int acknum, const char* vdata);
void a_send_message(int seqnum, int acknum, struct msg message);
void debug_log(char *function_name, char *log_message);

struct pkt make_pkt_msg(struct msg message, int seqNum, int ackNum);
struct pkt make_pkt_data(const char data[MESSAGE_LENGTH], int seqNum, int ackNum);
void writeLog(FILE *fp, int AorB, char *msg, const struct pkt *p, struct msg *m, float time);
