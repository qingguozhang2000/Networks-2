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

//Structure for storing sent packets, so once timer runs out the 
//last packet can be fetched from the linked list and sent again
struct pktQueue {
    struct pkt bufferedPkt; 
    struct pktQueue *next;
};

#ifndef RDT_VALUES
#define RDT_VALUES

extern int seq_num;
extern int ack_num;
extern struct msgQueue *head;
extern struct pktQueue *pktBufferHead;
extern struct msg last_message;
extern struct pkt *last_packet;

#endif

struct pkt* message_to_packet(struct msg *p_message, int seqnum, int acksum);

struct msg* packet_to_message(struct pkt *p_packet);

int isempty(struct msgQueue messages);

void enqueue_msg(struct msgQueue* messages, struct msg message);

struct msg dequeue_msg(struct msgQueue* messages);

struct msg message_pop(struct msgQueue* messages);

void message_push(struct msgQueue* messages, struct msg message);

void message_push_int(struct msgQueue* messages, struct msg message);

void copyPacket(struct pkt endP, struct pkt initP);

void copyMessage(struct msg* endM, struct msg *initM);

void sendLastPacket();

int calculateChecksum(char* vdata, int acknum, int seqnum);

struct pkt* make_packet(char payload[20], int seqnum, int acknum, int checksum);

int packetNotCorrupt(struct pkt* test_packet);

int responseNotCorrupt(struct pkt* test_packet);

int isCorrectPacket(struct pkt* packet);

int calculateChecksumForResponse(int seqnum, int acknum);

void toggle_0_1(int* p_number);

void debugLog(char* function_name, char* log_message);