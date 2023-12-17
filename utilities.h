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

struct pkt message_to_packet(struct msg *p_message, int seqnum, int acknum);
struct msg packet_to_message(struct pkt *p_packet);
struct pkt make_ack_packet(int seqnum);
void copy_packet(struct pkt *p_to, struct pkt *p_from);
void send_last_packet();
void toggle_0_1(int *p_number);

int calculate_checksum(struct pkt *p_packet);
int is_corrupt(struct pkt *p_packet);

int is_empty(struct msgQueue *message_queue);
void enqueue_msg(struct msgQueue *message_queue, struct msg message);
struct msg dequeue_msg(struct msgQueue *message_queue);

void debug_log(char *function_name, char *log_message);
