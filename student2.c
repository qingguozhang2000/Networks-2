#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
#include "utilities.h"
 
int seq_num;
int ack_num;

struct msgQueue *head;
struct pktQueue *pktBufferHead;

struct msg last_message;
struct pkt last_packet;

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
    head = (struct msgQueue*) malloc(sizeof(struct msgQueue));
    pktBufferHead = (struct pktQueue*) malloc(sizeof(struct pktQueue));

    seq_num = 0;
    ack_num = 0;
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
    // Allocate memory and make our packet
    struct pkt *packet =(struct pkt*) malloc(sizeof(struct pkt));
    packet = message_to_packet(&message, seq_num, 0);
    
    // Send our packet
    tolayer3(AEntity, *packet);

    // Start timer
    startTimer(AEntity, TIMER_TIME);

    // Log this packet as the last packet sent
    copyPacket(last_packet, *packet);
    // Log the message as the last message sent
    copyMessage(last_message, message);
}


/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
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
    // Extract our message
    struct msg *message = packet_to_message(&packet);

    // Checks to see if the packet was corrupted
    int notcorrupt = packetNotCorrupt(&packet);
    // Checks if the acknum of the ACK packet is the same as the current seqnum
    int correct_packet = isCorrectPacket(&packet);
    
    // If the packet is the right sequence and the right checksum
    if (notcorrupt && correct_packet) {
        // Toggle our seq and ack numbers
        toggle_0_1(&seq_num);
        toggle_0_1(&ack_num);
        // Stop our timer
        stopTimer(AEntity);
    } else {
        // Send the last message again
        A_output(last_message);
    }
}


/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
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