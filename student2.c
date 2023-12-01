#include <stdio.h>
#include <stdlib.h>
#include "project2.h"
 
/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* 
 * The routines you will write are detailed below. As noted above, 
 * such procedures in real-life would be part of the operating system, 
 * and would be called by other procedures in the operating system.  
 * All these routines are in layer 4.
 */

struct pkt* message_to_packet(struct msg *p_message) {
    struct pkt* new_packet = (struct pkt*) malloc(sizeof(struct pkt));


    new_packet->seqnum = 0;
    new_packet->acknum = 0;
    new_packet->checksum = calculateChecksum(p_message->data, new_packet->acknum, new_packet->seqnum);
    // new_packet->checksum = 0;
    printf("Ok until here.(now)\n");


    memcpy(new_packet->payload, p_message->data, 20 * sizeof(char));


    return new_packet;
}


struct msg* packet_to_message(struct pkt *p_packet) {
    struct msg* new_message = (struct msg*) malloc(sizeof(struct msg));

    memcpy(new_message->data, p_packet->payload, 20 * sizeof(char));
    printf("%s\n", new_message->data);
    // printf("Ok until here.\n");


    return new_message;
}


/*
 * A_output(message), where message is a structure of type msg, containing
 * data to be sent to the B-side. This routine will be called whenever the
 * upper layer at the sending side (A) has a message to send. It is the job
 * of your protocol to insure that the data in such a message is delivered
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
    // printf("A_output0\n");
    struct pkt *packet =(struct pkt*) malloc(sizeof(struct pkt));
    packet = message_to_packet(&message);
    // printf("A_output1\n");
    tolayer3(AEntity, *packet);
}


/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
    // struct msg *message =(struct msg*) malloc(sizeof(struct msg));
    // message = packet_to_message(&packet);
    // tolayer5(AEntity, *message);
}


/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {


}  


/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
}

/*
 * Note that with simplex transfer from A-to-B, there is no routine  B_output()
 */


/*
 * Just like A_output, but residing on the B side.  USED only when the
 * implementation is bi-directional.
 */
void B_output(struct msg message)  {

}


/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
    struct pkt *respondPkt;
    struct msg *message = (struct msg*) malloc(sizeof(struct msg));
    message = packet_to_message(&packet);

    // // Checks to see if the checksum is good
    // int checksum_good = 0;
    // int checksum_good = strcmp(calculateChecksum(packet.payload, packet.acknum, packet.seqnum), packet.checksum);
    // if (checksum_good == 0) {
    tolayer5(BEntity, *message);
    // sendACK(BEntity);
    respondPkt = (struct pkt*)malloc(sizeof(struct pkt));
    respondPkt->seqnum = 0;
    respondPkt->acknum = 0;//ACK
    respondPkt->checksum = calculateChecksumForResponse(respondPkt->acknum, respondPkt->seqnum);
    tolayer3(BEntity, *respondPkt);

    // }
}


/*
 * B_timerinterrupt()  This routine will be called when B's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
}


/*
 * The following routine will be called once (only) before any other  
 * entity B routines are called. You can use it to do any initialization
 */
void B_init() {
}


// void toLayer3(int AorB, struct pkt packet) {


// }


// void toLayer5(int AorB, struct msg datasent) {
   
// }




// void sendACK(int AorB) {
//     char* ACK_msg = (char*)malloc(20 * sizeof(char));
//     ACK_msg = "ACK ";
//     char* ACK_msg = (char*)malloc(20 * sizeof(char));
//     char ACK_number = ":200";
//     ACK_msg = strcat(ACK_msg, ACK_number);
//     // for (int i = strlen(ACK_msg); i < strlen(ACK_msg) + strlen(ACK_number) + 1; i++) {
//     //     ACK_msg[i] = ACK_number[i - strlen(ACK_msg)];
//     // }
//     printf("Okay here(beginning)\n");


//     struct msg ACK;
//     // memcpy(ACK.data, ACK_msg, 20 * sizeof(char));
//     for (int i = 0; i < 20; i++) {
//         if (ACK_number[i] != NULL){
//             ACK.data[i] = ACK_number[i];
//         } else {
//             ACK.data[i] = 0;
//         }
//     }


//     puts(strlen(ACK.data));
//     puts(strlen(ACK_number));
//     puts(ACK.data);
//     printf("Okay here(loop)\n");


//     if (AorB) {
//         B_output(ACK);
//     } else {
//         A_output(ACK);
//     }
// }


//calculate checksum; this function comes from internet
int calculateChecksum(char* vdata, int acknum, int seqnum) {
    // printf("Ok until here.\n");
    int i, checksum = 0;
    // printf("Ok until here.\n");


    for(i = 0; i < MESSAGE_LENGTH; i++){
        checksum += (int)(vdata[i]) * i;
    }
    // printf("Ok until here.\n");


    checksum += acknum * 21;
    checksum += seqnum * 22;
    printf("Ok until here.(now)\n");


    return checksum;
}

//calculate checksum for respond packet
int calculateChecksumForResponse(int acknum, int seqnum){
    return acknum + seqnum * 2;
}