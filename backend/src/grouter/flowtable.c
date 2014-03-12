#include "flowtable.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

struct message {
	int src_mac[6];
	int src_ip[4];
	int dest_ip[4];
	int protocol;
	int tos;
};

struct reply {
	int dest_mac[6];
	int interface;
};

struct reply* callServerToFindInterface(struct message *query) {

	int sockfd = 0, n = 0, interface = -1;
	char recvBuff[1024];
	struct sockaddr_in serv_addr;
	void *buffer;
	char *ip_addr = "132.206.55.121";
	struct reply *response = (struct reply*)malloc(sizeof(struct reply));

	buffer = malloc(1024);
	struct message m, *temp;
	m.src_mac[0] = query->src_mac[0];
	m.src_mac[1] = query->src_mac[1];
	m.src_mac[2] = query->src_mac[2];
	m.src_mac[3] = query->src_mac[3];
	m.src_mac[4] = query->src_mac[4];
	m.src_mac[5] = query->src_mac[5];

	m.src_ip[0] = query->src_ip[0];
	m.src_ip[1] = query->src_ip[1];
	m.src_ip[2] = query->src_ip[2];
	m.src_ip[3] = query->src_ip[3];

	m.dest_ip[0] = query->dest_ip[0];
	m.dest_ip[1] = query->dest_ip[1];
	m.dest_ip[2] = query->dest_ip[2];
	m.dest_ip[3] = query->dest_ip[3];

	m.protocol = query->protocol;
	m.tos = query->tos;

	temp = &m;

	memset(recvBuff, '0', sizeof(recvBuff));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Error : Could not create socket \n");
		exit(0);
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5001);

	if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
		printf("\n inet_pton error occured\n");
		exit(0);
	}

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		printf("\n Error : Connect Failed \n");
		exit(0);
	}



	memcpy(buffer, (void*) temp, sizeof(m));

	struct message *temp2;
	temp2 = (struct message*) buffer;


	write(sockfd, buffer, sizeof(m));

	n = read(sockfd, recvBuff, sizeof(recvBuff) - 1);
	if (n > 0) {
		recvBuff[n] = 0;
		char * pch;
		int interface = atoi(strtok(recvBuff, "#"));
		int m0 = atoi(strtok(NULL, "#"));
		int m1 = atoi(strtok(NULL, "#"));
		int m2 = atoi(strtok(NULL, "#"));
		int m3 = atoi(strtok(NULL, "#"));
		int m4 = atoi(strtok(NULL, "#"));
		int m5 = atoi(strtok(NULL, "#"));

		response->interface = interface;
		response->dest_mac[0] = m0;
		response->dest_mac[1] = m1;
		response->dest_mac[2] = m2;
		response->dest_mac[3] = m3;
		response->dest_mac[4] = m4;
		response->dest_mac[5] = m5;

	}

	if (n < 0) {
		printf("\n Read error \n");
	}

	close(sockfd);

	return response;

}

void process_packet_from_queue1(pthread_t *thread2, struct flowentry* flowtable) {

	int iret1;
	/* Create independent threads each of which will execute function */

	struct arguments *args = (struct arguments*) malloc(
			sizeof(struct arguments)); //WILL THIS BE A PROBLEM?? SHOULD I DO MALLOC??
	args->flowtable = flowtable;
	iret1 = pthread_create(thread2, NULL, process_packet, (void*) args);
}

void* process_packet(void * args) {

	struct arguments *args_ptr = (struct arguments *) args;

	struct flowentry* flowtable = (struct flowentry*) args_ptr->flowtable;

	int flowtableSize = 0;
	while (1) {
		int count = -1;


		if (queue1->cursize > 0) {

			gpacket_t *data;
			int len, i=0;
			readQueue(queue1, (void**) &data, &len);
			gpacket_t *temp_gpacket = data;
			pkt_data_t temp_data_packet = temp_gpacket->data;
			pkt_frame_t iface = temp_gpacket->frame;

			ip_packet_t *ip_pkt;
			ip_pkt = (ip_packet_t *) temp_data_packet.data;


/*			printf("\nFOUND THE PACKET::\n");
			printf("\nip_src: %d", ip_pkt->ip_src[0]);
			printf("\nip_src: %d", ip_pkt->ip_src[1]);
			printf("\nip_src: %d", ip_pkt->ip_src[2]);
			printf("\nip_src: %d", ip_pkt->ip_src[3]);

			printf("\nip_dest: %d", ip_pkt->ip_dst[0]);
			printf("\nip_dest: %d", ip_pkt->ip_dst[1]);
			printf("\nip_dest: %d", ip_pkt->ip_dst[2]);
			printf("\nip_dest: %d", ip_pkt->ip_dst[3]);
			printf("\nip_prot: %d", ip_pkt->ip_prot);
			printf("\ndata_packet: %d", temp_data_packet.header.prot);
			printf("\ndata_packet: %d", temp_data_packet.header.dst[0]);
			printf("\ndata_packet: %d", temp_data_packet.header.dst[1]);
			printf("\ndata_packet: %d", temp_data_packet.header.dst[2]);
			printf("\ndata_packet: %d", temp_data_packet.header.dst[3]);
			printf("\ndata_packet: %d", temp_data_packet.header.dst[4]);
			printf("\ndata_packet: %d", temp_data_packet.header.dst[5]);

			printf("\nSrc: %d", temp_data_packet.header.src[0]);
			printf("\nSrc: %d", temp_data_packet.header.src[1]);
			printf("\nSrc: %d", temp_data_packet.header.src[2]);
			printf("\nSrc: %d", temp_data_packet.header.src[3]);
			printf("\nSrc: %d", temp_data_packet.header.src[4]);
			printf("\nSrc: %d", temp_data_packet.header.src[5]);*/

			//Comparison logic

			bool match_found = FALSE;

/*			printf("\nPRINTING FLOW TABLE ");

			for(i=0; i<flowtableSize; i++){
				printf("\n %d	%d	%d	%d	%d	%d\n", flowtable[i].match.hw_addr_src[0],flowtable[i].match.hw_addr_src[1],flowtable[i].match.hw_addr_src[2],flowtable[i].match.hw_addr_src[3],flowtable[i].match.hw_addr_src[4],flowtable[i].match.hw_addr_src[5]);
				printf("\n %d	%d	%d	%d	%d	%d\n", flowtable[i].match.hw_addr_dest[0],flowtable[i].match.hw_addr_dest[1],flowtable[i].match.hw_addr_dest[2],flowtable[i].match.hw_addr_dest[3],flowtable[i].match.hw_addr_dest[4],flowtable[i].match.hw_addr_dest[5]);

			}*/


			//printf("\nFlowtable size is: %d",flowtableSize);

			while (flowtableSize > 0 && ++count < flowtableSize
					&& flowtable[count].match.protocol != 0
					&& flowtable[count].match.ip_addr_src[0] != 0) { // Check for end of flowtable entries


				/*if (flowtable[count].match.protocol
						!= temp_data_packet.header.prot)
					continue;*/
				//printf("\nCheckpoint 1\n");

				if (flowtable[count].match.hw_addr_src[0]
						!= temp_data_packet.header.dst[0])
					continue;
				//printf("\nCheckpoint 2\n");

				if (flowtable[count].match.hw_addr_src[1]
						!= temp_data_packet.header.dst[1])
					continue;
				//printf("\nCheckpoint 3\n");

				if (flowtable[count].match.hw_addr_src[2]
						!= temp_data_packet.header.dst[2])
					continue;
			//	printf("\nCheckpoint 4\n");

				if (flowtable[count].match.hw_addr_src[3]
						!= temp_data_packet.header.dst[3])
					continue;
			//	printf("\nCheckpoint 5\n");

				if (flowtable[count].match.hw_addr_src[4]
						!= temp_data_packet.header.dst[4])
					continue;
			//	printf("\nCheckpoint 6\n");

				if (flowtable[count].match.hw_addr_src[5]
						!= temp_data_packet.header.dst[5])
					continue;
			//	printf("\nCheckpoint 7\n");

/*				if (flowtable[count].match.ip_protocol != ip_pkt->ip_prot)
					continue;
				printf("\nCheckpoint 8\n");*/

				if (flowtable[count].match.ip_tos != ip_pkt->ip_tos)
					continue;
			//	printf("\nCheckpoint 9\n");

				if (flowtable[count].match.ip_addr_src[0] != ip_pkt->ip_src[0])
					continue;
			//	printf("\nCheckpoint 10\n");

				if (flowtable[count].match.ip_addr_src[1] != ip_pkt->ip_src[1])
					continue;
			//	printf("\nCheckpoint 11\n");

				if (flowtable[count].match.ip_addr_src[2] != ip_pkt->ip_src[2])
					continue;
			//	printf("\nCheckpoint 12\n");

				if (flowtable[count].match.ip_addr_src[3] != ip_pkt->ip_src[3])
					continue;
			//	printf("\nCheckpoint 13\n");

				if (flowtable[count].match.ip_addr_dest[0] != ip_pkt->ip_dst[0])
					continue;
			//	printf("\nCheckpoint 14\n");

				if (flowtable[count].match.ip_addr_dest[1] != ip_pkt->ip_dst[1])
					continue;
			//	printf("\nCheckpoint 15\n");

				if (flowtable[count].match.ip_addr_dest[2] != ip_pkt->ip_dst[2])
					continue;
			//	printf("\nCheckpoint 16\n");

				if (flowtable[count].match.ip_addr_dest[3] != ip_pkt->ip_dst[3])
					continue;
			//	printf("\nCheckpoint 17\n");

				match_found = TRUE;

				if(flowtable[count].action == 0){
					printf("\nPacket Dropped!!\n\n");
					break;
				}

				temp_gpacket->frame.dst_interface = flowtable[count].action;

		//		printf("\n Outgoing Interface is: %d	\n", temp_gpacket->frame.dst_interface);

				temp_data_packet.header.src[0] = temp_data_packet.header.dst[0];
				temp_data_packet.header.src[1] = temp_data_packet.header.dst[1];
				temp_data_packet.header.src[2] = temp_data_packet.header.dst[2];
				temp_data_packet.header.src[3] = temp_data_packet.header.dst[3];
				temp_data_packet.header.src[4] = temp_data_packet.header.dst[4];
				temp_data_packet.header.src[5] = temp_data_packet.header.dst[5];

				temp_gpacket->data.header.dst[0] = flowtable[count].match.hw_addr_dest[0];
				temp_gpacket->data.header.dst[1] = flowtable[count].match.hw_addr_dest[1];
				temp_gpacket->data.header.dst[2] = flowtable[count].match.hw_addr_dest[2];
				temp_gpacket->data.header.dst[3] = flowtable[count].match.hw_addr_dest[3];
				temp_gpacket->data.header.dst[4] = flowtable[count].match.hw_addr_dest[4];
				temp_gpacket->data.header.dst[5] = flowtable[count].match.hw_addr_dest[5];

				if(temp_gpacket->data.header.dst[0] == 255 && temp_gpacket->data.header.dst[1] == 255 && temp_gpacket->data.header.dst[2] == 255 &&
						temp_gpacket->data.header.dst[3] == 255 && temp_gpacket->data.header.dst[4] == 255 && temp_gpacket->data.header.dst[5] == 255){
					printf("\nBroadcasing Packet!!\n");
/*
					ip_packet_t *ip_pkt_new;
					ip_pkt_new = (ip_packet_t *) temp_gpacket->data.data;
					ip_pkt_new->ip_src[3] = ip_pkt_new->ip_src[1];
					ip_pkt_new->ip_src[1] = ip_pkt_new->ip_src[2];
					ip_pkt_new->ip_src[2] = ip_pkt_new->ip_src[0];
					ip_pkt_new->ip_src[0] = 0;

					ip_pkt_new->ip_dst[0] = ip_pkt_new->ip_src[2];
					ip_pkt_new->ip_dst[1] = 130;
					ip_pkt_new->ip_dst[2] = 255;
					ip_pkt_new->ip_dst[3] = 255;
					temp_gpacket->frame.nxth_ip_addr[0] = 0;
					temp_gpacket->frame.nxth_ip_addr[1] = 5;
					temp_gpacket->frame.nxth_ip_addr[2] = 168;
					temp_gpacket->frame.nxth_ip_addr[3] = 192;
					temp_gpacket->frame.arp_bcast = 1;

					temp_gpacket->data.header.prot = 1544;
					ip_pkt_new->ip_prot = 253;
					ip_pkt_new->ip_tos = 1;
					ip_pkt_new->ip_frag_off = 256;
					ip_pkt_new->ip_hdr_len = 0;
					ip_pkt_new->ip_identifier = 1030;
					ip_pkt_new->ip_pkt_len = 8;
					ip_pkt_new->ip_version = 0;

					printf("\nip_cksum: %d\n",ip_pkt->ip_cksum);
					printf("\nip_frag_off: %d\n",ip_pkt->ip_frag_off);
					printf("\nip_hdr_len: %d\n",ip_pkt->ip_hdr_len);
					printf("\nip_identifier: %d\n",ip_pkt->ip_identifier);
					printf("\nip_pkt_len: %d\n",ip_pkt->ip_pkt_len);
					printf("\nip_ttl: %d\n",ip_pkt->ip_ttl);
					printf("\nip_version: %d\n",ip_pkt->ip_version);*/
			}

/*				if (flowtable[count].action == FORWARD_ALL) {
					//TODO:Replicate packet for all outgoing ports and place packets in output_queue
				}

				if (flowtable[count].action == FORWARD_TO_DESTINATION) {
					temp_gpacket->frame.dst_interface =
							flowtable[count].port_number;
					printf("\nMATCH FOUND. ADDING PACKET IN OutputQ\n");
					writeQueue(outputQ, (void *) temp_gpacket,
							sizeof(gpacket_t));
				}

				if (flowtable[count].action == FORWARD_CONTROLLER) {
				}

				if (flowtable[count].action == DROP) {
					free(temp_gpacket);
				}*/

				//TODO:update counters
				writeQueue(outputQ, (void *) temp_gpacket, sizeof(gpacket_t));
				break;

			}

			if ((match_found == FALSE || flowtableSize == 0) && ip_pkt->ip_tos == 0 && ip_pkt->ip_prot == 1) {
				struct message query;
				query.src_mac[0] = temp_data_packet.header.dst[0];
				query.src_mac[1] = temp_data_packet.header.dst[1];
				query.src_mac[2] = temp_data_packet.header.dst[2];
				query.src_mac[3] = temp_data_packet.header.dst[3];
				query.src_mac[4] = temp_data_packet.header.dst[4];
				query.src_mac[5] = temp_data_packet.header.dst[5];

				query.src_ip[0] = ip_pkt->ip_src[0];
				query.src_ip[1] = ip_pkt->ip_src[1];
				query.src_ip[2] = ip_pkt->ip_src[2];
				query.src_ip[3] = ip_pkt->ip_src[3];

				query.dest_ip[0] = ip_pkt->ip_dst[0];
				query.dest_ip[1] = ip_pkt->ip_dst[1];
				query.dest_ip[2] = ip_pkt->ip_dst[2];
				query.dest_ip[3] = ip_pkt->ip_dst[3];

				query.protocol = ip_pkt->ip_prot;

				query.tos = ip_pkt->ip_tos;

				struct reply* response = callServerToFindInterface(&query);


				flowtable[flowtableSize].match.hw_addr_src[0] = query.src_mac[0];
				flowtable[flowtableSize].match.hw_addr_src[1] = query.src_mac[1];
				flowtable[flowtableSize].match.hw_addr_src[2] = query.src_mac[2];
				flowtable[flowtableSize].match.hw_addr_src[3] = query.src_mac[3];
				flowtable[flowtableSize].match.hw_addr_src[4] = query.src_mac[4];
				flowtable[flowtableSize].match.hw_addr_src[5] = query.src_mac[5];

				flowtable[flowtableSize].match.hw_addr_dest[0] = response->dest_mac[0];
				flowtable[flowtableSize].match.hw_addr_dest[1] = response->dest_mac[1];
				flowtable[flowtableSize].match.hw_addr_dest[2] = response->dest_mac[2];
				flowtable[flowtableSize].match.hw_addr_dest[3] = response->dest_mac[3];
				flowtable[flowtableSize].match.hw_addr_dest[4] = response->dest_mac[4];
				flowtable[flowtableSize].match.hw_addr_dest[5] = response->dest_mac[5];

				flowtable[flowtableSize].match.ip_addr_src[0] = ip_pkt->ip_src[0];
				flowtable[flowtableSize].match.ip_addr_src[1] = ip_pkt->ip_src[1];
				flowtable[flowtableSize].match.ip_addr_src[2] = ip_pkt->ip_src[2];
				flowtable[flowtableSize].match.ip_addr_src[3] = ip_pkt->ip_src[3];

				flowtable[flowtableSize].match.ip_addr_dest[0] = ip_pkt->ip_dst[0];
				flowtable[flowtableSize].match.ip_addr_dest[1] = ip_pkt->ip_dst[1];
				flowtable[flowtableSize].match.ip_addr_dest[2] = ip_pkt->ip_dst[2];
				flowtable[flowtableSize].match.ip_addr_dest[3] = ip_pkt->ip_dst[3];

				flowtable[flowtableSize].match.protocol = ip_pkt->ip_prot;

				flowtable[flowtableSize].match.ip_tos = ip_pkt->ip_tos;

				flowtable[flowtableSize].action = response->interface;

				flowtableSize++;
				temp_gpacket->frame.dst_interface = response->interface;

				temp_data_packet.header.src[0] = temp_data_packet.header.dst[0];
				temp_data_packet.header.src[1] = temp_data_packet.header.dst[1];
				temp_data_packet.header.src[2] = temp_data_packet.header.dst[2];
				temp_data_packet.header.src[3] = temp_data_packet.header.dst[3];
				temp_data_packet.header.src[4] = temp_data_packet.header.dst[4];
				temp_data_packet.header.src[5] = temp_data_packet.header.dst[5];

				temp_gpacket->data.header.dst[0] = response->dest_mac[0];
				temp_gpacket->data.header.dst[1] = response->dest_mac[1];
				temp_gpacket->data.header.dst[2] = response->dest_mac[2];
				temp_gpacket->data.header.dst[3] = response->dest_mac[3];
				temp_gpacket->data.header.dst[4] = response->dest_mac[4];
				temp_gpacket->data.header.dst[5] = response->dest_mac[5];
				//printf("\nMATCH NOT FOUND. ADDING PACKET IN QUEUE2\n");

				//writeQueue(queue2, (void *)temp_gpacket, sizeof(gpacket_t));
				writeQueue(outputQ, (void *) temp_gpacket, sizeof(gpacket_t));
			}
//////////////////////////



			//Comparision logic
			//logic for placing packet in queue2
		}

	}
	return 0;
}
