#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
#include "utilities.h"

extern int TraceLevel;

struct pkt* message_to_packet(struct msg *p_message, int seqnum, int acksum) {
    struct pkt* p_packet = (struct pkt*) malloc(sizeof(struct pkt));

    p_packet->seqnum = seqnum;
    p_packet->acknum = acksum;
    memcpy(p_packet->payload, p_message->data, MESSAGE_LENGTH);
    p_packet->checksum = calculateChecksum(p_packet);

    return p_packet;
}


struct msg* packet_to_message(struct pkt *p_packet) {
    struct msg* new_message = (struct msg*) malloc(sizeof(struct msg));

    memcpy(new_message->data, p_packet->payload, 20 * sizeof(char));
    printf("%s\n", new_message->data);
    // printf("Ok until here.\n");

    return new_message;
}

struct pkt make_ack_packet(int a_seq_num) {
    // Declare our ACK packet
    struct pkt ack_packet;

    // Make our ACK packet
    ack_packet.seqnum = a_seq_num;
    ack_packet.acknum = a_seq_num;
    memset(ack_packet.payload, '\0', MESSAGE_LENGTH);
    ack_packet.checksum = calculateChecksum(&ack_packet);

    return ack_packet;
}

int isempty(struct msgQueue *message_queue) {
    if (message_queue->front == NULL && message_queue->rear == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void enqueue_msg(struct msgQueue* message_queue, struct msg message) {
    struct msgQueue *m_ptr;
    m_ptr = (struct msgQueue*) malloc(sizeof(struct msgQueue));
    m_ptr->waitingMessage = message;
    m_ptr->next = NULL;

    if(message_queue->front == NULL) {
        message_queue->front = m_ptr;
        message_queue->rear = m_ptr;
    }
    else
    {
        message_queue->rear->next = m_ptr;
        message_queue->rear = m_ptr;
    }
}

struct msg dequeue_msg(struct msgQueue* message_queue) {
    struct msg dequeued_msg;

    if (isempty(message_queue))
    {
        printf("Queue is empty.\n");
    }
    else if (message_queue->front == message_queue->rear)
    {
        dequeued_msg = message_queue->front->waitingMessage;
        free(message_queue->front);
        message_queue->front = message_queue->rear = NULL;
    }
    else
    {
        dequeued_msg = message_queue->front->waitingMessage;
        message_queue->front = message_queue->front->next;
    }

    return dequeued_msg;
}

struct msg message_pop(struct msgQueue* message_queue) {
    printf("**%s, ", message_queue->waitingMessage.data);
    // Copy our message
    struct msg pop_message;
    if (strlen(message_queue->waitingMessage.data) == 20) {
        copyMessage(&pop_message, &message_queue->waitingMessage);
    } else {
        strcpy(pop_message.data, "");
    }

    // Pop the top off
    message_queue = message_queue->next;

    // Return our message
    return pop_message;
}

void message_push(struct msgQueue* message_queue, struct msg message) {
    // Get the old message_queue
    struct msgQueue* p_old_queue = (struct msgQueue*) malloc(sizeof(struct msgQueue));
    memcpy(p_old_queue, message_queue, sizeof(struct msgQueue));

    // Replace the message in our queue with the new message
    message_queue->waitingMessage = message;
    // Put our old message queue into the chain
    message_queue->next = p_old_queue;
}

void copyPacket(struct pkt *endP, struct pkt *initP) {
    endP->seqnum = initP->seqnum;
    endP->acknum = initP->acknum;
    endP->checksum = initP->checksum;
    memcpy(endP->payload, initP->payload, MESSAGE_LENGTH);
}

void copyMessage(struct msg* endM, struct msg *initM) {
    memcpy(endM->data, initM->data, MESSAGE_LENGTH);
}

void sendLastPacket() {
    stopTimer(AEntity);
    tolayer3(AEntity, last_packet);
    startTimer(AEntity, TIMER_TIME);
}


//calculate checksum; this function comes from internet
int calculateChecksum(struct pkt* p_packet){
    int i, checksum = 0;
    char *payload = p_packet->payload;

    for(i = 0; i < MESSAGE_LENGTH; i++) {
        checksum += (int)(payload[i]) * i;
    }

    checksum += p_packet->acknum * 21;
    checksum += p_packet->seqnum * 22;

    return checksum;
}

struct pkt* make_packet(char payload[20], int seqnum, int acknum, int checksum) {
    struct pkt* return_packet = (struct pkt*) malloc(sizeof(struct pkt));

    memcpy(return_packet->payload, payload, MESSAGE_LENGTH);
    return_packet->seqnum = seqnum;
    return_packet->acknum = acknum;
    return_packet->checksum = checksum;

    return return_packet;
}

int is_corrupt(struct pkt* p_packet) {
    int wanted_sum = p_packet->checksum;
    int current_sum = calculateChecksum(p_packet);
    printf("**Wanted sum: %d, Actual sum: %d\n", wanted_sum, current_sum);

    return (wanted_sum != current_sum) ? TRUE : FALSE;
}

void toggle_0_1(int* p_number) {
    *p_number = (*p_number + 1) % 2;
}

void debugLog(char* function_name, char* log_message) {
    if (TraceLevel >= 2) {
        printf("***%s: %s***\n", function_name, log_message);
    }
}
