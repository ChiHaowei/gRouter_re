#include"message.h"
#include"grouter.h"
#include "ip.h"

#define FLOWTABLE_SIZE 100
#define FORWARD_TO_DESTINATION 0
#define FORWARD_ALL 1
#define FORWARD_CONTROLLER 2
#define DROP 3
#define MODIFY 4
#define ADD_NEW 5
#define DELETE 6



struct fields{
	int ingress_port;
	uchar hw_addr_src[6];
	uchar hw_addr_dest[6];
	ushort protocol;
	//VLAN ID 										/*To Be Implemented*/
	//VLAN Priority 								/*To Be Implemented*/
	uchar ip_addr_src[4];
	uchar ip_addr_dest[4];
	ushort ip_protocol;
	uint8_t ip_tos;
	// Transport source port / ICMP Type			/*To Be Implemented*/
	// Transport destination port / ICMP Code		/*To Be Implemented*/
};

struct per_flow_counters{
	//Per Flow
	uint64_t received_packets;
	uint64_t received_bytes;
	uint32_t duration;
};


struct flowentry{
	struct fields match;
	struct per_flow_counters stats;
	int action; //Allowed actions on a flow entry are 1, 2, 3
	int port_number; //to be used in case FORWARD_TO_DESTINATION = 0
};

struct per_table_counters{
	uint32_t active_entries;
	uint64_t packet_lookups;
	uint64_t packet_matches;

};

struct port_counters{
	uint64_t received_packets;
	uint64_t transmitted_packets;
	uint64_t received_bytes;
	uint64_t transmitted_bytes;
	uint64_t receive_drops;
	uint64_t transmit_drops;
	uint64_t receive_error;
	uint64_t transmit_error;
};

struct per_port_counters{
	int port_number;
	struct port_counters port_counters;
};

struct arguments{
	struct flowentry* flowtable;
};

void* process_packet(void * args);
void process_packet_from_queue1(pthread_t *, struct flowentry*);

struct flowentry flowtable[FLOWTABLE_SIZE];



