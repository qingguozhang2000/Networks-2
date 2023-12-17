#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
#include "utilities.h"

int message_state;
int a_seq_num;
int b_seq_num;

struct pkt last_packet;
struct msgQueue *message_queue;

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
    a_seq_num = 0;
    message_state = SEND_MESSAGE;
    message_queue = (struct msgQueue*) malloc(sizeof(struct msgQueue));
}


/*
 * The following routine will be called once (only) before any other  
 * entity B routines are called. You can use it to do any initialization
 */
void B_init() {
    b_seq_num = 0;
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
    // If ready to send the next message
    if (message_state == SEND_MESSAGE) {
        // Allocate memory and make our packet
        struct pkt *packet = message_to_packet(&message, a_seq_num, a_seq_num);
        printf("**\n");
        printf("**INITIALIZED A NEW MESSAGE, a_seq_num: %d, checksum: %d\n", a_seq_num, calculateChecksum(packet));
        
        // Send our packet
        tolayer3(AEntity, *packet);

        // Start timer
        startTimer(AEntity, TIMER_TIME);

        // Log this packet as the last packet sent
        copyPacket(&last_packet, packet);
        message_state = WAIT_FOR_ACK;
    } else if (message_state == WAIT_FOR_ACK) {
        enqueue_msg(message_queue, message);
    }
}


/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
    debugLog("B_input", "received a packet from A");

    // If the packet is not corrupted, we will send a ACK regardless
    if (!is_corrupt(&packet)) {
        struct pkt ack_packet = make_ack_packet(a_seq_num);

        // Send the ACK message
        tolayer3(BEntity, ack_packet);
        startTimer(BEntity, TIMER_TIME);

        // if the packet sequence number is what B expected
        // we send the message to layer 5 and toggle B's expected sequence number
        if (packet.seqnum == b_seq_num) {
            struct msg *message = packet_to_message(&packet);
            // Pass on the message to Layer 5
            tolayer5(BEntity, *message);
            toggle_0_1(&b_seq_num);
        }
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

    // Checks to see if the packet was corrupted
    int packet_corrupted = is_corrupt(&packet);
    // Checks if the acknum of the ACK packet is the same as the current seqnum
    int correct_packet = (packet.seqnum == a_seq_num);
    printf("*** packet.seqnum=%d a_seq_num=%d ***\n", packet.seqnum, a_seq_num);
    
    // If the packet is the right sequence and the right checksum
    printf("**Packet corrupted? %d, Correct packet? %d\n", packet_corrupted, correct_packet);
    if (!packet_corrupted && correct_packet) {
        printf("**Should not be corrupt!\n");
        printf("**Old numbers: %d, %d\n", a_seq_num, a_seq_num);
        // Toggle our seq and ack numbers
        toggle_0_1(&a_seq_num);
        printf("**New numbers: %d, %d\n", a_seq_num, a_seq_num);
        // Stop our timer
        stopTimer(AEntity);
        message_state = SEND_MESSAGE;

        // Send the next message
        printf("**OUR MESSAGE QUEUE:\n");
        struct msg next_message = dequeue_msg(message_queue);
        A_output(next_message);
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
    startTimer(AEntity, TIMER_TIME);
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
}
