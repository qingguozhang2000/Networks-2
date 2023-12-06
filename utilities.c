#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
#include "utilities.h"

extern int seq_num;
extern struct pkt *last_packet;


struct pkt* message_to_packet(struct msg *p_message, int seqnum, int acksum) {
    struct pkt* new_packet = (struct pkt*) malloc(sizeof(struct pkt));


    new_packet->seqnum = seqnum;
    new_packet->acknum = acksum;
    int new_checksum = calculateChecksum(p_message->data, new_packet->seqnum, new_packet->acknum);
    new_packet->checksum = new_checksum;
    // new_packet->checksum = 0;


    memcpy(new_packet->payload, p_message->data, 20 * sizeof(char));
    // printf("Message: %s, Packet data: %s\n", , supposed_checksum);


    return new_packet;
}


struct msg* packet_to_message(struct pkt *p_packet) {
    struct msg* new_message = (struct msg*) malloc(sizeof(struct msg));

    memcpy(new_message->data, p_packet->payload, 20 * sizeof(char));
    printf("%s\n", new_message->data);
    // printf("Ok until here.\n");

    return new_message;
}

void copyPacket(struct pkt endP, struct pkt initP) {
    endP.acknum = initP.acknum;
    endP.acknum = initP.acknum;
    memcpy(endP.payload, initP.payload, MESSAGE_LENGTH);
}

void copyMessage(struct msg endM, struct msg initM) {
    memcpy(endM.data, initM.data, MESSAGE_LENGTH);
}

void sendLastPacket() {
    tolayer3(AEntity, *last_packet);
    startTimer(AEntity, TIMER_TIME);
}


//calculate checksum; this function comes from internet
int calculateChecksum(char* vdata, int seqnum, int acknum){
    int i, checksum = 0;


    for(i = 0; i < MESSAGE_LENGTH; i++){
        checksum += (int)(vdata[i]) * i;
    }


    checksum += acknum * 21;
    checksum += seqnum * 22;


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

int packetNotCorrupt(struct pkt* test_packet) {
    int not_corrupted;

    int wanted_sum = test_packet->checksum;
    int current_sum = calculateChecksum(test_packet->payload, test_packet->seqnum, test_packet->acknum);

    if (wanted_sum == current_sum) {
        not_corrupted = TRUE;
    } else {
        not_corrupted = FALSE;
    }

    return not_corrupted;
}

int isCorrectPacket(struct pkt* packet) {
    int is_correct;
    
    int packet_ack_num = packet->acknum;

    if (seq_num == packet_ack_num) {
        is_correct = 1;
    } else {
        is_correct = 0;
    }

    return is_correct;
}

//calculate checksum for respond packet
int calculateChecksumForResponse(int seqnum, int acknum){
    return acknum + seqnum * 2;
}

void toggle_0_1(int* p_number) {
    *p_number = (*p_number + 1) % 2;
}

void debugLog(char* function_name, char* log_message) {
    printf("*****************%s: %s*****************\n", function_name, log_message);
}