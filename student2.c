#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student2.h"
 
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

/*
 * A_output(message), where message is a structure of type msg, containing
 * data to be sent to the B-side. This routine will be called whenever the
 * upper layer at the sending side (A) has a message to send. It is the job
 * of your protocol to insure that the data in such a message is delivered
 * in-order, and correctly, to the receiving side upper layer.
 */

void A_output(struct msg message) {
	/*
	* https://gaia.cs.umass.edu/kurose_ross/interactive/rdt30.php
	* Wait for call 0 from above (layer 5 - application)
	* S0: rdt_send(data)
	* sndpkt = make_plt(0, data, checksum)
	* udt_send(sndpkt)
	* start_timer
	*/
    debug_log("A_output", "received message from application");

	A_state.n_layer5++;

	switch (A_state.senderMode) {
		case WAIT_FOR_MSG:
			// sending packets do not need to set acknum
			debug_log("A_output", "Send packet to layer3; start timer; wait for ACK");
			current_packet = make_pkt(message.data, A_state.seqnum, ACK_ABP_DEFAULT);
			tolayer3(AEntity, current_packet);
			A_state.n_layer3++;

			// A now waits for ACK; start timer
			startTimer(AEntity, TIMEOUT);
			A_state.senderMode = WAIT_FOR_ACK;
			break;

		case WAIT_FOR_ACK:
			// Last packet not ACKed, push message to queue for later processing
			debug_log("A_output", "Last message have not finished; put this message on the queue");
			enQueue(p_message_queue, message);
			break;
	}
}

/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
	/*
	 * rdt_rcv(rcvpkt)
	 * if (corrupt(rcvpkt) || isACK(rcvpkt, 1)) -> do nothing (S1)
	 * if (!corrupt(rcvpkt) && isACK(rcvpkt, 0)) -> stop_timer (S3)
	 * else -> do nothing (S4)
	 */
    debug_log("A_input", "received packet from B");

	int actual_checksum = calc_checksum(&packet);
	int packet_checksum = packet.checksum;
	int is_corrupt = actual_checksum == packet_checksum ? FALSE : TRUE;

	int is_ACK = packet.acknum == A_state.seqnum ? TRUE : FALSE;

	if (is_ACK && !is_corrupt) {
	    debug_log("A_input", "success ACK, stop timer and change state");
		stopTimer(AEntity);
		A_state.seqnum = (A_state.seqnum + 1) % 2;
		A_state.senderMode = WAIT_FOR_MSG;
	} else if (!is_ACK) {
	    debug_log("A_input", "got NACK, resend packet and stop timer");
		stopTimer(AEntity);
		tolayer3(AEntity, current_packet);
		startTimer(AEntity, TIMEOUT);
		A_state.n_layer3++;
	} else if (is_corrupt) {
	    debug_log("A_input", "ACK corrupted, do nothing and wait for timeout");
	}
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See startTimer()
 * and stopTimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
	/*
	 * timeout (S2)
	 * udt_send(sndpkt)
	 * start_timer
	 */
    debug_log("A_timerinterrupt", "A timed out waiting for ACK response from B");

	// should be a redundant check
	if (A_state.senderMode == WAIT_FOR_ACK) {
	    debug_log("A_timerinterrupt", "Resend the packet again");
		tolayer3(AEntity, current_packet);
		A_state.n_layer3++;
		startTimer(AEntity, TIMEOUT);
	} else {
		perror(">> Problem\n");
	}
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
    debug_log("A_init", "initialize A by creating message queue");
    p_message_queue = createQueue();

	A_state.seqnum = 0;
	A_state.senderMode = WAIT_FOR_MSG;
	A_state.n_layer3 = 0;
	A_state.n_layer5 = 0;
}

/*
 * Note that with simplex transfer from A-to-B, there is no routine  B_output()
 */


/*
 * Just like A_output, but residing on the B side.  USED only when the
 * implementation is bi-directional.
 */
void B_output(struct msg message)  {
    debug_log("B_init", "do nothing for now");
}


/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
	/*
	 * rdt_rcv(rcvpkt) wait for packet 0 from layer 3
	 * R0: !corrupt(rcvpkt) && has_seq0(rcvpkt)
	 *     extract(rcvpkt, data)
	 *     delivert_data(data)
	 *     sndpkt = make_pkt(ACK, 0, checksum)
	 *     udt_send(sndpkt)
	 * R1: corrupt(rcvpkt) || has_seq0(rcvpkt)
	 *     sndpkt = make_pkt(ACK, 0, checksum)
	 *     udt_send(sndpkt)
	 */
    debug_log("B_input", "B received request packets from A");

	int actual_checksum = calc_checksum(&packet);
	int packet_checksum = packet.checksum;
	int is_corrupt = actual_checksum == packet_checksum ? FALSE : TRUE;

	int is_expected_packet = packet.seqnum == B_state.seqnum ? TRUE : FALSE;

	B_state.n_layer3++;

	if (is_expected_packet && !is_corrupt) {
		debug_log("B_input", "normal packet, deliver data to layer5");
		B_state.seqnum = (B_state.seqnum + 1) % 2;
        struct msg message;
        memcpy(message.data, packet.payload, MESSAGE_LENGTH);
		tolayer5(BEntity, message);
		B_state.n_layer5++;
	} else if (is_corrupt) {
		debug_log("B_input", "packet corrupted; send NACK to layer 3");
		// send packet seq no as it is received
		// nextseqnum doesn't have any meaning here
		struct pkt ack = make_pkt(packet.payload, packet.seqnum, !B_state.seqnum);
		tolayer3(BEntity, ack);
	} else if (!is_expected_packet) {
		debug_log("B_input", "duplicate packet; resend ACK to layer 3");
		// send packet seq no as it is received
		// nextseqnum doesn't have any meaning here
		struct pkt ack = make_pkt(packet.payload, packet.seqnum, !B_state.seqnum);
		tolayer3(BEntity, ack);
	}
}


/*
 * B_timerinterrupt()  This routine will be called when B's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See startTimer()
 * and stopTimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
    debug_log("B_timerinterrupt", "do nothing for now");
}


/*
 * The following routine will be called once (only) before any other  
 * entity B routines are called. You can use it to do any initialization
 */
void B_init() {
    // debug_log("B_init", "do nothing for now");

	B_state.seqnum = 0;
	B_state.senderMode = WAIT_FOR_MSG;
	B_state.n_layer3 = 0;
	B_state.n_layer5 = 0;
}


void a_send_message(int seqnum, int acknum, struct msg message) {
    struct pkt packet;

    packet.seqnum = seqnum;
    packet.acknum = acknum;
    packet.checksum = calculateChecksum(seqnum, acknum, message.data);

    memcpy(packet.payload, message.data, MESSAGE_LENGTH);

    tolayer3(AEntity, packet);
}

int calc_checksum(const struct pkt *packet) {
    return calculateChecksum(packet->seqnum, packet->acknum, packet->payload);
}

//calculate checksum; this function comes from internet
int calculateChecksum(int seqnum, int acknum, const char* vdata) {
    int i, checksum = 0;

    if (vdata != NULL) {
        for(i = 0; i < MESSAGE_LENGTH; i++) {
            checksum += (int)(vdata[i]) * i;
        }
    }

    checksum += acknum * 21;
    checksum += seqnum * 22;

    return checksum;
}

void debug_log(char *function_name, char *log_message) {
    if ( TraceLevel >= 0 ) {
        printf("--- In %s: %s\n", function_name, log_message);
    }
}

struct pkt make_pkt(const char data[MESSAGE_LENGTH], int seqNum, int ackNum) {
	struct pkt packet = {seqNum, ackNum, 0, ""};
	strncpy(packet.payload, data, MESSAGE_LENGTH);
	packet.checksum = calc_checksum(&packet);
	return packet;
}

void writeLog(FILE *fp, int AorB, char *msg, const struct pkt *p, struct msg *m, float time) {
	char ch = (AorB == AEntity) ? 'A' : 'B';
	if (p != NULL) {
		fprintf(fp, "[%c] @%f %s. Packet[seq=%d,ack=%d,check=%d,data=%s..]\n", ch, time,
		        msg, p->seqnum, p->acknum, p->checksum, p->payload);
	} else if (m != NULL) {
		fprintf(fp, "[%c] @%f %s. Message[data=%s..]\n", ch, time, msg, m->data);
	} else {
		fprintf(fp, "[%c] @%f %s.\n", ch, time, msg);
	}
}
