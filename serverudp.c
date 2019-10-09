//Header files
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include <signal.h>
#include<pthread.h>
#include <sys/time.h>


#define PACKET_SIZE 1470  //defining packet size


//file 
int file_size;
FILE *file;
char *file_as_string;

int flag = 0;

//defining port number
const int PORT_NUMBER = 24790;
const int TCP_PORT_NUMBER = 40000;


int receiver_socket_tcp[10];
int receiver_socket_tcp_connect;
struct sockaddr_in sender;


//structure for received packet
struct packet_recieve
{
	int sequence;
	char payload[PACKET_SIZE];
};


//structure for sending the NACK packet
struct NACK_packet
{
	int sequence;   //sequence number of the packet received
	
};

//for counting the no of NACKS to send
struct NACK_num 
{
	int no_of_NACKS;
};

struct packet_lastreceive
{
	int sequence;
	char payload[310];
};




//First procedure for all the 10 threads
void* file_receive(void* arg )
{
	

	int run=0; 
	//for positioning
	int i = (intptr_t) arg;

	int u;
	if(i == 9)
	{
		u=6801;
	}
	else
	{
		u=6803;
	}

	//defing a buf to check
	int checker[u];
	bzero(checker, sizeof(checker));


	//creating structure objects
	struct  NACK_packet NACK_sending_packet;
	struct packet_recieve received_packet;
	struct packet_lastreceive received_lastpacket;
	struct NACK_num count_num;

	//creating receiver address to bind the server socket
	struct sockaddr_in receiver;


	//initializing packet number to 0 to keep track of packets
	int packet_num=0;
	int receiver_socket;

	//for positining and indexing
	int position, index;

	//to_send and to_recv variables to check whether they are sent or received
	int to_send, to_recv, len;

	//structure for calculating time. To calculating the throughput
	struct timeval timer;
	


	//creating the socket to receive from the sender
	receiver_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (receiver_socket==-1)
	{				
		perror("Receiver: socket\n");
		exit(0);				
	}

	//defining the address for the receiver
	receiver.sin_family = AF_INET;
	receiver.sin_addr.s_addr = inet_addr("192.168.0.161");
	receiver.sin_port = htons(PORT_NUMBER+i);

	//binding the receiver socket with its address
	int check;
	check = bind(receiver_socket, (struct sockaddr *) &receiver, sizeof(receiver));
	if(check<0)
	{
		printf("Error in binding the server socket : %d\n", i);
	}
	

	len = sizeof(receiver);
	//defining the position to start receiving 
	position = flag*100000000;


	
	
	//usleep(5000000);
event : 
	while(1)
	{	
		
		//clearing put the payload field and sequence field of the received packet
		bzero(received_packet.payload,PACKET_SIZE);

		//setting timer to 2 sec
		timer.tv_sec = 3;
		timer.tv_usec = 0;
		setsockopt(receiver_socket, SOL_SOCKET, SO_RCVTIMEO, &timer, sizeof(timer));


		//receiving packets on the UDP socket
		to_recv = recvfrom(receiver_socket, &received_packet , sizeof(received_packet), 0, (struct sockaddr *)&sender, &len);
		

		if(to_recv == -1) 
		{
			break;
		}
		else
		{
			if(received_packet.sequence == 6801 && i==9)
			{
				checker[received_packet.sequence-1] = 1;
				strncpy(file_as_string+position+(i*10000410) + ((received_packet.sequence-1)*1470), received_packet.payload,sizeof(received_lastpacket.payload));

			}
			else
			{
				checker[received_packet.sequence-1] = 1;
				strncpy(file_as_string+position+ (i*10000410) +((received_packet.sequence-1)*1470), received_packet.payload,sizeof(received_packet.payload));
			}
			

		}


	}



	//counting the no of NACK's
	int count = 0; 
	for(int z =0; z<u; z++ )
	{
		if(checker[z] == 0)
		{
			count++;
		}

	}
	


	//sending the number of NACKS
	count_num.no_of_NACKS = count;
	to_send = send(receiver_socket_tcp[i], &count_num, sizeof(count_num), 0);
	if(to_send == -1)
	{
		perror("send_to: count_num"); 
		exit(1);
	}



	if(count == 0)
	{
		//closing the receiver socket
		close(receiver_socket);
		if(flag == 9)
		{
			close(receiver_socket_tcp[i]);	
		}	
		return 0;
	}
	else
	{
		
		//send NACK's for packets that were not received
		int send_check;
		for(int z =0; z<u; z++)
		{
			if(checker[z] == 0)
			{
				//count2--; 
				NACK_sending_packet.sequence = z+1;
				send_check = send(receiver_socket_tcp[i], &NACK_sending_packet, sizeof(NACK_sending_packet), 0);
				if(send_check == -1)
				{
					perror("send_check: NACK"); 
					exit(1);
				}

			}

		}
		goto event;

	}




}


int main(int argc, char* argv[])
{
	int j;
	
	//creating 10 thread id's
	pthread_t thread_id[10];

	//definiing timeval structures
	struct timeval start, end;


	//defining objects of the structure
	struct  NACK_packet NACK_sending_packet;
	struct packet_recieve received_packet;
	struct sockaddr_in receiver, sender;
	

	
	int receiver_socket;
	int position, index;

	int to_send, to_recv, len;

	char final_receive[256];


	//open file
	file = fopen(argv[1],"w");



	

	struct sockaddr_in receiver_check;
	struct sockaddr_in client;

	int len1 = sizeof(client);

	receiver_check.sin_family = AF_INET;
	receiver_check.sin_addr.s_addr = inet_addr("192.168.0.161");
	receiver_check.sin_port = htons(30000);

	//creating a socket
	int receiver_socket1 = socket(AF_INET, SOCK_DGRAM, 0);
	if (receiver_socket1==-1)
	{				
		perror("Receiver: socket\n");
		exit(0);				
	}

	//binding a socket
	int bind_to = bind(receiver_socket1, (struct sockaddr*)&receiver_check, sizeof(receiver_check));


	//receiving from 
	to_recv = recvfrom(receiver_socket1, final_receive, sizeof(final_receive), 0, (struct sockaddr *)&client, &len1);
	if(to_recv == -1) 
			{
	 			perror("recv"); 
		 		exit(1);
		 	}      
	final_receive[to_recv]='\0';

	printf("Message from client: %s \n", final_receive);



	//get size of file and put it in a string		
	file_size = 1000000000;
	file_as_string = malloc(file_size+1);
	file_as_string[file_size+1]='\0';
	


	printf("Starting to receive file \n");

	

	usleep(7000000);

	//creating 10 TCP sockets
	for(int i=0; i<10; i++)
	{

		receiver_socket_tcp[i] = socket(AF_INET, SOCK_STREAM, 0);


		sender.sin_family  = AF_INET;
		sender.sin_addr.s_addr = inet_addr("192.168.0.133");
		sender.sin_port  = htons(TCP_PORT_NUMBER + i);


		//____________connecting with client for sending NACK's____________
		receiver_socket_tcp_connect = connect(receiver_socket_tcp[i], (struct sockaddr*)&sender, sizeof(sender));
		if(receiver_socket_tcp_connect == -1)
	    {
	        perror("Error in connecting for TCP");
	        exit(0);
	    }


	}

	time_t now1, now2;

	gettimeofday(&start,NULL);
	time(&now1);
	printf("Start time : %s\n",ctime(&now1));


	long start_time = 1000000 * start.tv_sec + start.tv_usec;



next_loop :

	//creates thread with argument as file_as_string and j		
	pthread_create(&thread_id[0], NULL, file_receive, (void *)(intptr_t) 0);
	pthread_create(&thread_id[1], NULL, file_receive, (void *)(intptr_t) 1);
	pthread_create(&thread_id[2], NULL, file_receive, (void *)(intptr_t) 2);
	pthread_create(&thread_id[3], NULL, file_receive, (void *)(intptr_t) 3);
	pthread_create(&thread_id[4], NULL, file_receive, (void *)(intptr_t) 4);
	pthread_create(&thread_id[5], NULL, file_receive, (void *)(intptr_t) 5);
	pthread_create(&thread_id[6], NULL, file_receive, (void *)(intptr_t) 6);
	pthread_create(&thread_id[7], NULL, file_receive, (void *)(intptr_t) 7);
	pthread_create(&thread_id[8], NULL, file_receive, (void *)(intptr_t) 8);
	pthread_create(&thread_id[9], NULL, file_receive, (void *)(intptr_t) 9);
	

	//joining with 10 threads
	for(int j=0; j<10; j++)
	{
		pthread_join(thread_id[j],NULL);
	}

	flag++;
	if(flag<10)
	{
		goto next_loop;
	}

	
	//transferring contents of the file_as_string to fle
	fputs(file_as_string, file);

	
	
	gettimeofday(&end,NULL);
	time(&now2);
	printf("End time : %s\n",ctime(&now2));

	long end_time = 1000000 * end.tv_sec + end.tv_usec;
	

	//File received successfully
	printf("File received\n");
	free(file_as_string);	

	long print_time;
	print_time = ((end_time)-start_time);


	printf("Throughput : %0.3fMbps \n", 1000*8.0/(print_time/1000000.0));

}