#include <stdio.h>
#include <stdlib.h>
#include "project2.h"

// void toLayer3(int AorB, struct pkt packet) {

// }

// void toLayer5(int AorB, struct msg datasent) {
    
// }

struct pkt message_to_packet(struct msg *p_message) {
    struct pkt new_packet;

    new_packet.seqnum = 0;
    new_packet.acknum = 0;
    new_packet.checksum = 0;

    memcpy(new_packet.payload, p_message->data, 20 * sizeof(char));

    return new_packet;
}

struct msg packet_to_message(struct pkt *p_packet) {
    struct msg new_message;

    memcpy(new_message.data, p_packet->payload, 20 * sizeof(char));

    return new_message;
}