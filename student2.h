struct pkt message_to_packet(struct msg *p_message);

struct msg packet_to_message(struct pkt *p_packet);

void sendACK(int AorB);

int calculateChecksum(char* vdata, int acknum, int seqnum);