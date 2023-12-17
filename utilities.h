#define TRUE 1
#define FALSE 0

#define SEND_MESSAGE 0
#define WAIT_FOR_ACK 1

#define TIMER_TIME 100

//structure for putting incoming message passed in parameter of function 
//A_output() into a linked list, so that when the ack of the previous 
//sending message arrives, the first message in linked list would be 
//packed and sent from the sender, if there is one 
struct msgQueue { 
        struct msg waitingMessage; 
        struct msgQueue *next;

        struct msgQueue *front;
        struct msgQueue *rear;
};

#ifndef RDT_VALUES
#define RDT_VALUES

extern int a_seq_num;
extern int b_seq_num;
extern struct msgQueue *message_queue;
extern struct pkt last_packet;

#endif

struct pkt* message_to_packet(struct msg *p_message, int seqnum, int acksum);

struct msg* packet_to_message(struct pkt *p_packet);

struct pkt make_ack_packet(int a_seq_num);

int isempty(struct msgQueue *message_queue);

void enqueue_msg(struct msgQueue* message_queue, struct msg message);

struct msg dequeue_msg(struct msgQueue* message_queue);

struct msg message_pop(struct msgQueue* message_queue);

void message_push(struct msgQueue* message_queue, struct msg message);

void message_push_int(struct msgQueue* message_queue, struct msg message);

void copyPacket(struct pkt *endP, struct pkt *initP);

void copyMessage(struct msg* endM, struct msg *initM);

void sendLastPacket();

int calculateChecksum(struct pkt *p_packet);

struct pkt* make_packet(char payload[20], int seqnum, int acknum, int checksum);

int is_corrupt(struct pkt* test_packet);

void toggle_0_1(int* p_number);

void debugLog(char* function_name, char* log_message);