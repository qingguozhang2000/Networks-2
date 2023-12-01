// #include <stdio.h>
// #include <stdlib.h>
// #include "project2.h"

// // void toLayer3(int AorB, struct pkt packet) {

// // }

// // void toLayer5(int AorB, struct msg datasent) {
    
// // }

// struct pkt message_to_packet(struct msg *p_message) {
//     struct pkt new_packet;

//     new_packet.seqnum = 0;
//     new_packet.acknum = 0;
//     // new_packet.checksum = calculateChecksum(p_message->data, new_packet.acknum, new_packet.seqnum);
//     new_packet.checksum = 0;
//     printf("Ok until here.(now)\n");

//     memcpy(new_packet.payload, p_message->data, 20 * sizeof(char));

//     return new_packet;
// }

// struct msg packet_to_message(struct pkt *p_packet) {
//     struct msg new_message;

//     memcpy(new_message.data, p_packet->payload, 20 * sizeof(char));
//     printf("%s\n", new_message.data);
//     // printf("Ok until here.\n");

//     return new_message;
// }

// void sendACK(int AorB) {
//     char* ACK_msg = (char)malloc(20 * sizeof(char));
//     ACK_msg = "ACK ";
//     char* ACK_number = (char)4;
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

// //calculate checksum; this function comes from internet
// int calculateChecksum(char* vdata, int acknum, int seqnum) {
//     // printf("Ok until here.\n");
//     int i, checksum = 0;
//     // printf("Ok until here.\n");

//     for(i = 0; i < MESSAGE_LENGTH; i++){
//         checksum += (int)(vdata[i]) * i;
//     }
//     // printf("Ok until here.\n");

//     checksum += acknum * 21;
//     checksum += seqnum * 22;
//     printf("Ok until here.(now)\n");

//     return checksum;
// }