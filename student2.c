#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
#include "utilities.h"

int seq_num;
int ack_num;

struct msgQueue *messages;
struct pktQueue *pktBufferHead;

struct msg *last_message;
struct pkt *last_packet;

int message_state;

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


/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
    printf("**A initialized\n");
    seq_num = 0;
    ack_num = 0;

    message_state = SEND_MESSAGE;

    messages = (struct msgQueue*) malloc(sizeof(struct msgQueue));
    pktBufferHead = (struct pktQueue*) malloc(sizeof(struct pktQueue));
    
    last_message = (struct msg*) malloc(sizeof(struct msg));
    last_packet = (struct pkt*) malloc(sizeof(struct pkt));
}


/*
 * The following routine will be called once (only) before any other  
 * entity B routines are called. You can use it to do any initialization
 */
void B_init() {

}


/*
 * A_output(message), where message is a structure of type msg, containing
 * data to be sent to the B-side. This routine will be called whenever the
 * upper layer at the sending side (A) has a message to send. It is the job
 * of your protocol to insure that the data in such a message is delivered
 * in-order, and correctly, to the receiving side upper layer.
 */
// Outer layer calls this function in order to send packet
void A_output(struct msg message) {
    // Put the next message into the queue
    message_push(messages, message);

    // If ready to send the next message
    if (message_state == SEND_MESSAGE) {
        // Find the first message that came in and pop it out
        struct msg next_message = message_pop(messages);
        // Allocate memory and make our packet
        struct pkt *packet =(struct pkt*) malloc(sizeof(struct pkt));
        packet = message_to_packet(&next_message, seq_num, 0);
        printf("**INITIALIZED A NEW MESSAGE, seq_num: %d, checksum: %d\n", seq_num, calculateChecksum(&next_message, seq_num, 0));
        
        // Send our packet
        tolayer3(AEntity, *packet);

        // Start timer
        startTimer(AEntity, TIMER_TIME);

        // Log this packet as the last packet sent
        copyPacket(*last_packet, *packet);
        // Log the message as the last message sent
        copyMessage(*last_message, next_message);
        message_state = WAIT_FOR_ACK;
    }
}


/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
    printf("**RECEIVED A NEW MESSAGE\n");
    struct msg *message = (struct msg*) malloc(sizeof(struct msg));
    message = packet_to_message(&packet);

    // // Checks to see if the packet was corrupted
    int notcorrupt = packetNotCorrupt(&packet);
    // printf("Packet checksum: %d, Supposed checksum: %d\n", packet.checksum, supposed_checksum);
    
    // If the packet is the right sequence and the right checksum
    if (notcorrupt) {
        // Pass on the message to Layer 5
        tolayer5(BEntity, *message);

        // Declare our ACK packet
        struct pkt *respondPkt;
        // Make our ACK packet
        int respond_checksum = calculateChecksumForResponse(0, packet.seqnum);
        respondPkt = make_packet(message, 0, packet.seqnum, respond_checksum);

        // Send the ACK message
        tolayer3(BEntity, *respondPkt);
    }
}


/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
    printf("**RECEIVE ACK\n");
    // // Extract our message
    // struct msg *message = packet_to_message(&packet);

    // Checks to see if the packet was corrupted
    int notcorrupt = responseNotCorrupt(&packet);
    // Checks if the acknum of the ACK packet is the same as the current seqnum
    int correct_packet = isCorrectPacket(&packet);
    
    // If the packet is the right sequence and the right checksum
    printf("**Not corrupt? %d, Correct packet? %d\n", notcorrupt, correct_packet);
    if (notcorrupt && correct_packet) {
        printf("**Should not be corrupt!\n");
        printf("**Old numbers: %d, %d\n", seq_num, ack_num);
        // Toggle our seq and ack numbers
        toggle_0_1(&seq_num);
        toggle_0_1(&ack_num);
        printf("**New numbers: %d, %d\n", seq_num, ack_num);
        // Stop our timer
        stopTimer(AEntity);
        message_state = SEND_MESSAGE;
    } else {
        // Send the last message again
        printf("**Last message sent!\n");
        message_state = SEND_MESSAGE;
        A_output(*last_message);
    }
}


/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
    message_state = SEND_MESSAGE;
    sendLastPacket();
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
 * B_timerinterrupt()  This routine will be called when B's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
    sendLastPacket();
}