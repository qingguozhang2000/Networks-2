#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
#include "utilities.h"

extern int TraceLevel;

struct pkt message_to_packet(struct msg *p_message, int seqnum, int acksum) {
    struct pkt packet;

    packet.seqnum = seqnum;
    packet.acknum = acksum;
    memcpy(packet.payload, p_message->data, MESSAGE_LENGTH);
    packet.checksum = calculate_checksum(&packet);

    return packet;
}


struct msg packet_to_message(struct pkt *p_packet) {
    struct msg message;
    memcpy(message.data, p_packet->payload, MESSAGE_LENGTH);
    return message;
}

struct pkt make_ack_packet(int a_seq_num) {
    struct pkt ack_packet;

    ack_packet.seqnum = a_seq_num;
    ack_packet.acknum = a_seq_num;
    memset(ack_packet.payload, '\0', MESSAGE_LENGTH);
    ack_packet.checksum = calculate_checksum(&ack_packet);

    return ack_packet;
}

int is_empty(struct msgQueue *message_queue) {
    if (message_queue->front == NULL && message_queue->rear == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void enqueue_msg(struct msgQueue *message_queue, struct msg message) {
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

struct msg dequeue_msg(struct msgQueue *message_queue) {
    struct msg dequeued_msg;

    if (is_empty(message_queue))
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

void copy_packet(struct pkt *p_to, struct pkt *p_from) {
    p_to->seqnum = p_from->seqnum;
    p_to->acknum = p_from->acknum;
    p_to->checksum = p_from->checksum;
    memcpy(p_to->payload, p_from->payload, MESSAGE_LENGTH);
}

void send_last_packet() {
    stopTimer(AEntity);
    tolayer3(AEntity, last_packet);
    startTimer(AEntity, TIMER_TIME);
}


//calculate checksum; this function comes from internet
int calculate_checksum(struct pkt *p_packet){
    int i, checksum = 0;
    char *payload = p_packet->payload;

    for(i = 0; i < MESSAGE_LENGTH; i++) {
        checksum += (int)(payload[i]) * i;
    }

    checksum += p_packet->acknum * 21;
    checksum += p_packet->seqnum * 22;

    return checksum;
}

int is_corrupt(struct pkt *p_packet) {
    int wanted_sum = p_packet->checksum;
    int current_sum = calculate_checksum(p_packet);
    printf("**Wanted sum: %d, Actual sum: %d\n", wanted_sum, current_sum);

    return (wanted_sum != current_sum) ? TRUE : FALSE;
}

void toggle_0_1(int *p_number) {
    *p_number = (*p_number + 1) % 2;
}

void debug_log(char *function_name, char *log_message) {
    if (TraceLevel >= 2) {
        printf("***%s: %s***\n", function_name, log_message);
    }
}
