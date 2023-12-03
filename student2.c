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
    debug_log("A_output", "received message from application");

	struct current_state *state = &A_state;

	++(state->n_layer5);

	/* If the last packet have not been ACKed, just drop this message */
	if (state->senderMode != WAITING_FOR_PKT) {
        debug_log("A_output", "Last message have not finished; put this message on the queue");
        enQueue(p_message_queue, message);
		return;
	}

	state->senderMode = WAITING_FOR_ACK;/* set current pkt to not finished */

	current_packet = make_pkt_msg(message, state->seqnum, ACK_ABP_DEFAULT);/* sending packets do not need to set acknum */

	tolayer3(AEntity, current_packet);
	++state->n_layer3;

	// printLog(A, const_cast<char *>("Sent packet to layer3"), &current_packet, &message);
	startTimer(AEntity, TIMEOUT);
}


/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
    debug_log("A_input", "received ACK packet from B");

    // int actual_checksum = calculateChecksum(packet.seqnum, packet.acknum, packet.payload);
    // if (packet.checksum == actual_checksum) {
    //     debug_log("A_input", "packet checksum matches actual checksum");

    //     struct msg message;
    //     memcpy(message.data, packet.payload, MESSAGE_LENGTH);

    //     debug_log("A_input", "pass ACK response to A application");
    //     tolayer5(AEntity, message);

    //     debug_log("A_input", "stop timer for A");
    //     stopTimer(AEntity);

    //     debug_log("A_input", "flip A seq num");
    //     a_seq = packet.acknum == 0 ? 1 : 0;
    // } else {
    //     debug_log("A_input", "packet checksum does not match actual checksum");
    // }

	struct current_state *state = &A_state;
	// printLog(A, const_cast<char *>("Receive ACK packet from layer3"), &packet, NULL);

	/// if packet has been received successfully
	if (packet.acknum == state->seqnum && packet.checksum == calc_checksum(&packet)) {
		state->seqnum = (state->seqnum + 1) % 2;
		state->senderMode = WAITING_FOR_PKT;
		stopTimer(AEntity);/// stop timer before changing state

		// printLog(A, const_cast<char *>("ACK packet process successfully accomplished!!"), &packet, NULL);
		return;
	}

		/* check checksum, if corrupted*/
	else if (packet.checksum != calc_checksum(&packet)) {
		// printLog(A, const_cast<char *>("ACK packet is corrupted"), &packet, NULL);
	}
		/* NAK or duplicate ACK : corrupted packet received */
	else if (packet.acknum != state->seqnum) {
		stopTimer(AEntity);
		// printLog(A, const_cast<char *>("ACK is not expected : NACK"), &packet, NULL);
		// printLog(A, const_cast<char *>("Resend the packet again"), &current_packet, NULL);
		tolayer3(AEntity, current_packet);
		++state->n_layer3;
		startTimer(AEntity, TIMEOUT);
	}
}


/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See startTimer()
 * and stopTimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
    debug_log("A_timerinterrupt", "A timed out waiting for ACK response from B");

    // debug_log("A_timerinterrupt", "stop timer for A");
    // stopTimer(AEntity);

    // debug_log("A_timerinterrupt", "resend message to B");
    // a_send_message(a_seq, a_ack, p_message_queue->front->message);

    // debug_log("A_timerinterrupt", "start timer for A");
    // startTimer(AEntity, TIMER_INCREMENT);

	struct current_state *state = &A_state;
	// printLog(AEntity, const_cast<char *>("Timeout occurred"), &current_packet, NULL);
	if (state->senderMode == WAITING_FOR_ACK) { // should be a redundant check
		// printLog(AEntity, const_cast<char *>("Timeout! Resend the packet again"), &current_packet, NULL);
		tolayer3(AEntity, current_packet);
		++state->n_layer3;
		startTimer(AEntity, TIMEOUT);
	} else {
		perror(">> Problem\n");
	}
}


/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
    // debug_log("A_init", "initialize A by creating message queue");
    // p_message_queue = createQueue();

	A_state.seqnum = 0;
	A_state.senderMode = WAITING_FOR_PKT;
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
    // debug_log("B_input", "B received request packets from A");

    // int actual_checksum = calculateChecksum(packet.seqnum, packet.acknum, packet.payload);
    // if (packet.checksum == actual_checksum) {
    //     debug_log("B_input", "checksum matched");
    //     struct msg message;
    //     memcpy(message.data, packet.payload, MESSAGE_LENGTH);
    //     tolayer5(BEntity, message);

    //     struct pkt ackPacket;
    //     ackPacket.seqnum = b_seq;
    //     ackPacket.acknum = packet.seqnum;
    //     ackPacket.checksum = calculateChecksum(ackPacket.seqnum, ackPacket.acknum, NULL);
    //     tolayer3(BEntity, ackPacket);
    // } else {
    //     char errorMessage[50];

    //     sprintf(errorMessage,
    //         "Packet checksum: %d, Actual checksum: %d\n",
    //         packet.checksum, actual_checksum);

    //     debug_log("B_input", errorMessage);
    // }

	struct current_state *state = &B_state;

	// printLog(B, const_cast<char *>("Receive a packet from layer3"), &packet, NULL);
	++(state->n_layer3);
	int acknum = state->seqnum;

	/* normal packet, deliver data to layer5 */
	if (packet.seqnum == state->seqnum && packet.checksum == calc_checksum(&packet)) {
		state->seqnum = (state->seqnum + 1) % 2;
        struct msg message;
        memcpy(message.data, packet.payload, MESSAGE_LENGTH);
		tolayer5(BEntity, message);
		++(state->n_layer5);
		// printLog(B, const_cast<char *>("Sent packet to layer5"), &packet, NULL);
	} else if (packet.checksum != calc_checksum(&packet)) {/* if corrupted send NACK */
		// printLog(B, const_cast<char *>("Data Packet is corrupted"), &packet, NULL);
		acknum = !state->seqnum;
		struct pkt ack = make_pkt_data(packet.payload, packet.seqnum, acknum);// send packet seq no as it is received
		tolayer3(BEntity, ack); // nextseqnum doesn't have any meaning here
		// printLog(B, const_cast<char *>("Send NACK packet to layer3"), &ack, NULL);
		return;
	} else if (packet.seqnum != state->seqnum) {/* duplicate pkt resends the ACK*/
		// printLog(B, const_cast<char *>("Duplicated packet detected"), &packet, NULL);
	}
	struct pkt ack = make_pkt_data(packet.payload, packet.seqnum, acknum);// send packet seq no as it is received
	tolayer3(BEntity, ack); // nextseqnum doesn't have any meaning here
	// printLog(B, const_cast<char *>("Send ACK packet to layer3"), &ack, NULL);
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
	B_state.senderMode = WAITING_FOR_PKT;
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

struct pkt make_pkt_msg(struct msg message, int seqNum, int ackNum) {
	struct pkt packet = {seqNum, ackNum, 0, ""};
	strncpy(packet.payload, message.data, MESSAGE_LENGTH);
	packet.checksum = calc_checksum(&packet);
	return packet;
}

struct pkt make_pkt_data(const char data[MESSAGE_LENGTH], int seqNum, int ackNum) {
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
