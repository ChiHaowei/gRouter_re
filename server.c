#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

int MAX_ENTRIES;

struct table{
int src_mac[6];
int dest_mac[6];
int src_ip[4];
int dest_ip[4];
int protocol;
int tos;
int interface;
};

struct table entries[1000];

void readData()
{


  FILE* file = fopen ("test1.txt", "r");
  int i = 0, j = 0;

  fscanf (file, "%d", &i);
	    
  while (!feof (file))
    {  
      /* Reading src Mac*/
      entries[j].src_mac[0] = i;
      fscanf (file, "%d", &i);

      entries[j].src_mac[1] = i;
      fscanf (file, "%d", &i);

      entries[j].src_mac[2] = i;
      fscanf (file, "%d", &i);

      entries[j].src_mac[3] = i;
      fscanf (file, "%d", &i);

      entries[j].src_mac[4] = i;
      fscanf (file, "%d", &i);  

      entries[j].src_mac[5] = i;
      fscanf (file, "%d", &i);    

      /* Reading Dest Mac*/
      entries[j].dest_mac[0] = i;
      fscanf (file, "%d", &i);

      entries[j].dest_mac[1] = i;
      fscanf (file, "%d", &i);

      entries[j].dest_mac[2] = i;
      fscanf (file, "%d", &i);

      entries[j].dest_mac[3] = i;
      fscanf (file, "%d", &i);

      entries[j].dest_mac[4] = i;
      fscanf (file, "%d", &i);  

      entries[j].dest_mac[5] = i;
      fscanf (file, "%d", &i);  


      /* Reading src IP*/
      entries[j].src_ip[0] = i;
      fscanf (file, "%d", &i);

      entries[j].src_ip[1] = i;
      fscanf (file, "%d", &i);

      entries[j].src_ip[2] = i;
      fscanf (file, "%d", &i);

      entries[j].src_ip[3] = i;
      fscanf (file, "%d", &i);

      /* Reading dest IP*/
      entries[j].dest_ip[0] = i;
      fscanf (file, "%d", &i);

      entries[j].dest_ip[1] = i;
      fscanf (file, "%d", &i);

      entries[j].dest_ip[2] = i;
      fscanf (file, "%d", &i);

      entries[j].dest_ip[3] = i;
      fscanf (file, "%d", &i);

      /*Reading protocol*/
      entries[j].protocol = i;
      fscanf (file, "%d", &i);

      /*Reading tos*/
      entries[j].tos = i;
      fscanf (file, "%d", &i);

      /*Reading interface*/
      entries[j].interface = i;
      fscanf (file, "%d", &i);
	
      j++;
    }

  	  MAX_ENTRIES = j;

    printf("\n %d:%d:%d:%d:%d:%d \n",entries[2].src_mac[0],entries[2].src_mac[1],entries[2].src_mac[2],entries[2].src_mac[3],entries[2].src_mac[4],entries[2].src_mac[5]);
    printf("\n %d:%d:%d:%d:%d:%d \n",entries[2].dest_mac[0],entries[2].dest_mac[1],entries[2].dest_mac[2],entries[2].dest_mac[3],entries[2].dest_mac[4],entries[2].dest_mac[5]);
    printf("\n %d:%d:%d:%d \n",entries[2].src_ip[0],entries[2].src_ip[1],entries[2].src_ip[2],entries[2].src_ip[3]);
    printf("\n %d:%d:%d:%d \n",entries[2].dest_ip[0],entries[2].dest_ip[1],entries[2].dest_ip[2],entries[2].dest_ip[3]);

    printf("\n Prot: %d tos: %d  interface: %d \n",entries[2].protocol,entries[2].tos,entries[2].interface);

  fclose (file); 
}

int main(int argc, char *argv[])
{
    readData();
    int listenfd = 0, connfd = 0, n=0, i=0;
    struct sockaddr_in serv_addr; 
struct message{
	int src_mac[6];
	//int dest_mac[6];
	int src_ip[4];
	int dest_ip[4];
	int protocol;
	int tos;
};
    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5001); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
	printf("\nWaiting for a client to connect...\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

    n = read(connfd, sendBuff, sizeof(sendBuff)-1);
	printf("\nn is: %d\n",n);

    if(n>0)
    {

    sendBuff[n] = 0;
	struct message *temp;
	temp = (struct message*)&sendBuff;
	
    printf("\n Messgae : %d:%d:%d:%d:%d:%d \n",temp->src_mac[0],temp->src_mac[1],temp->src_mac[2],temp->src_mac[3],temp->src_mac[4],temp->src_mac[5]);
    printf("\n Messgae : %d:%d:%d:%d \n",temp->src_ip[0],temp->src_ip[1],temp->src_ip[2],temp->src_ip[3]);
    printf("\n Messgae : %d:%d:%d:%d \n",temp->dest_ip[0],temp->dest_ip[1],temp->dest_ip[2],temp->dest_ip[3]);

    printf("\n Messgae : Prot: %d tos: %d  \n",temp->protocol,temp->tos);


	for(i=0; i<MAX_ENTRIES;i++){
		printf("\ni IS: %d\n",i);

		printf("\nBreakpoint A \n");
		if(temp->src_mac[0] != entries[i].src_mac[0])
			continue;
		printf("\nBreakpoint B \n");
		if(temp->src_mac[1] != entries[i].src_mac[1])
			continue;
		printf("\nBreakpoint C \n");

		if(temp->src_mac[2] != entries[i].src_mac[2])
			continue;
		printf("\nBreakpoint D \n");

		if(temp->src_mac[3] != entries[i].src_mac[3])
			continue;
		printf("\nBreakpoint E \n");

		if(temp->src_mac[4] != entries[i].src_mac[4])
			continue;
		printf("\nBreakpoint F \n");

		if(temp->src_mac[5] != entries[i].src_mac[5])
			continue;
/*

		if(temp->dest_mac[0] != entries[i].dest_mac[0])
			continue;

		if(temp->dest_mac[1] != entries[i].dest_mac[1])
			continue;

		if(temp->dest_mac[2] != entries[i].dest_mac[2])
			continue;

		if(temp->dest_mac[3] != entries[i].dest_mac[3])
			continue;

		if(temp->dest_mac[4] != entries[i].dest_mac[4])
			continue;

		if(temp->dest_mac[5] != entries[i].dest_mac[5])
			continue;
*/
		printf("\nBreakpoint G %d , %d\n",temp->src_ip[0],entries[i].src_ip[0]);
		if(temp->src_ip[0] != entries[i].src_ip[0])
			continue;

		printf("\nBreakpoint H \n");

		if(temp->src_ip[1] != entries[i].src_ip[1])
			continue;
		printf("\nBreakpoint I \n");

		if(temp->src_ip[2] != entries[i].src_ip[2])
			continue;
		printf("\nBreakpoint J \n");
		if(temp->src_ip[3] != entries[i].src_ip[3])
			continue;
		printf("\nBreakpoint K \n");


		if(temp->dest_ip[0] != entries[i].dest_ip[0])
			continue;
		printf("\nBreakpoint L \n");

		if(temp->dest_ip[1] != entries[i].dest_ip[1])
			continue;
		printf("\nBreakpoint M \n");

		if(temp->dest_ip[2] != entries[i].dest_ip[2])
			continue;
		printf("\nBreakpoint N \n");

		if(temp->dest_ip[3] != entries[i].dest_ip[3])
			continue;
		printf("\nBreakpoint O \n");


		if(temp->protocol != entries[i].protocol)
			continue;
		printf("\nBreakpoint P \n");

		if(temp->tos != entries[i].tos)
			continue;

		break;	

	}	       


    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 

	printf("\nSending Message\n");

	printf("\ni is: %d\n",i);
	if(i >= MAX_ENTRIES)
		snprintf(sendBuff, sizeof(sendBuff), "%d#%d#%d#%d#%d#%d#%d\n", 0,0,0,0,0,0,0);
	else{
		
		snprintf(sendBuff, sizeof(sendBuff), "%d#%d#%d#%d#%d#%d#%d\n", entries[i].interface, entries[i].dest_mac[0], entries[i].dest_mac[1], entries[i].dest_mac[2], entries[i].dest_mac[3], entries[i].dest_mac[4], entries[i].dest_mac[5]);
	}
        write(connfd, sendBuff, strlen(sendBuff)); 
	printf("\nMessage Sent: sendBudd was: %s\n",sendBuff);
        sleep(1);
     }
}
