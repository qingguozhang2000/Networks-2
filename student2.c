#include "student2.h"

// These are global variables to keep track of state etc.
struct Queue *p_message_queue;		  // when A is waiting for B's ACK, this queues messages for later processing
struct CurrentState A_state, B_state; // this keeps track of A and B's waiting modes and expected sequence numbers etc.
struct pkt current_packet;			  // this saves current packet in case we need to resend it

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

void A_output(struct msg message)
{
	/*
	 * https://gaia.cs.umass.edu/kurose_ross/interactive/rdt30.php
	 * Wait for call 0 from above (layer 5 - application)
	 * S0: rdt_send(data)
	 * sndpkt = make_plt(0, data, checksum)
	 * udt_send(sndpkt)
	 * start_timer
	 */
	debug_log("A_output", "A receives message from application");

	switch (A_state.waiting_mode)
	{
	case WAIT_FOR_MSG:
		debug_log("A_output", "A is waiting for message now; so A sends packet to layer3");
		current_packet = make_packet(message.data, A_state.seqnum, DEFAULT_ACK_NUM);
		tolayer3(AEntity, current_packet);

		debug_log("A_output", "A starts timer and waits for ACK");
		startTimer(AEntity, TIMEOUT);
		A_state.waiting_mode = WAIT_FOR_ACK;
		break;

	case WAIT_FOR_ACK:
		debug_log("A_output", "A is waiting for ACK from last message; so have put this message on the queue for future processing");
		enqueue(p_message_queue, message);
		break;
	}
}

/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet)
{
	/*
	 * rdt_rcv(rcvpkt)
	 * if (corrupt(rcvpkt) || isACK(rcvpkt, 1)) -> do nothing (S1)
	 * if (!corrupt(rcvpkt) && isACK(rcvpkt, 0)) -> stop_timer (S3)
	 * else -> do nothing (S4)
	 */
	debug_log("A_input", "received packet from B");

	int is_corrupt = packet_corrupted(&packet);
	int is_ACK = packet.acknum == A_state.seqnum ? TRUE : FALSE;

	if (is_ACK && !is_corrupt)
	{
		debug_log("A_input", "A gets success ACK from B");
		debug_log("A_input", "So A stops timer and flips sequence number");
		stopTimer(AEntity);

		debug_log("A_input", "A is back waiting for message and flips sequence number");
		A_state.seqnum = (A_state.seqnum + 1) % 2;
		A_state.waiting_mode = WAIT_FOR_MSG;

		debug_log("A_input", "A checks is message queue is empty");
		if (p_message_queue->front != NULL)
		{
			debug_log("A_input", "A still has queued messages; dequeue the first one");
			struct msg *p_message = dequeue(p_message_queue);

			debug_log("A_input", "call A_output to process it");
			A_output(*p_message);
		}
	}
	else if (!is_ACK)
	{
		debug_log("A_input", "A gets a NACK from B; so A stops the timer, resends the packet, and starts the timer");
		stopTimer(AEntity);
		tolayer3(AEntity, current_packet);
		startTimer(AEntity, TIMEOUT);
	}
	else if (is_corrupt)
	{
		debug_log("A_input", "ACK corrupted, do nothing and wait for timeout");
	}
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See startTimer()
 * and stopTimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt()
{
	/*
	 * timeout (S2)
	 * udt_send(sndpkt)
	 * start_timer
	 */
	debug_log("A_timerinterrupt", "A timed out");

	// should be a redundant check
	if (A_state.waiting_mode == WAIT_FOR_ACK)
	{
		debug_log("A_timerinterrupt", "A is indeed timedout waiting for ACK response from B");
		debug_log("A_timerinterrupt", "A resends the packet and starts the timer again");
		tolayer3(AEntity, current_packet);
		startTimer(AEntity, TIMEOUT);
	}
	else
	{
		debug_log("A_timerinterrupt", "This is unexpected: A is timedout while waiting for message from application");
	}
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	debug_log("A_init", "initialize A by creating message queue");
	p_message_queue = create_queue();

	A_state.seqnum = 0;
	A_state.waiting_mode = WAIT_FOR_MSG;
}

/*
 * Note that with simplex transfer from A-to-B, there is no routine  B_output()
 */

/*
 * Just like A_output, but residing on the B side.  USED only when the
 * implementation is bi-directional.
 */
void B_output(struct msg message)
{
	debug_log("B_init", "do nothing for now");
}

/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet)
{
	/*
	 * rdt_rcv(rcvpkt) wait for packet 0 from layer 3
	 * R0: !corrupt(rcvpkt) && has_seq0(rcvpkt)
	 *     extract(rcvpkt, data)
	 *     delivert_data(data)
	 *     sndpkt = make_packet(ACK, 0, checksum)
	 *     udt_send(sndpkt)
	 * R1: corrupt(rcvpkt) || has_seq0(rcvpkt)
	 *     sndpkt = make_packet(ACK, 0, checksum)
	 *     udt_send(sndpkt)
	 */
	debug_log("B_input", "B receives request packet from A");

	int is_corrupt = packet_corrupted(&packet);
	int is_expected_packet = packet.seqnum == B_state.seqnum ? TRUE : FALSE;

	if (is_expected_packet && !is_corrupt)
	{
		debug_log("B_input", "good packet, deliver data to layer5");
		B_state.seqnum = (B_state.seqnum + 1) % 2;
		struct msg message;
		memcpy(message.data, packet.payload, MESSAGE_LENGTH);
		tolayer5(BEntity, message);

		debug_log("B_input", "good packet; send ACK to layer 3");
		struct pkt ack = make_packet(packet.payload, B_state.seqnum, packet.seqnum);
		tolayer3(BEntity, ack);
	}
	else
	{
		debug_log("B_input", "packet corrupted or not expected; send NACK to layer 3");
		struct pkt nack = make_packet(packet.payload, packet.seqnum, !B_state.seqnum);
		tolayer3(BEntity, nack);
	}
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See startTimer()
 * and stopTimer() in the writeup for how the timer is started and stopped.
 */
void B_timerinterrupt()
{
	debug_log("B_timerinterrupt", "do nothing for now");
}

/*
 * The following routine will be called once (only) before any other
 * entity B routines are called. You can use it to do any initialization
 */
void B_init()
{
	debug_log("B_init", "B initializes its sequence number");
	B_state.seqnum = 0;
	B_state.waiting_mode = WAIT_FOR_PKT;
}
