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


#define PACKET_SIZE 1470


int flag = 0;
int file_size;
int count=0;
int k=0;
FILE *file;
FILE *to_write;
char *file_as_string;
const int PORT_NUMBER = 24790;
const int PORT_NUMBER_TCP = 40000;
unsigned long start_time;
struct timeval start, end, cur_time;
int sender_socket_TCP[10];
int new_socket[10];
struct sockaddr_in receiver_TCP, sender_TCP;
int bind_TCP;

struct packet_send
{
	int sequence;
	char payload[PACKET_SIZE];
	//int final;
};

struct packet_lastsend
{
	int sequence;
	char payload[310];
};

struct NACK_packet
{
	int sequence;
};

struct NACK_number
{
	int no_of_NACKS;
};


void* file_send(void *incoming )
{
	int i = (intptr_t)incoming;
	//printf("thread %d got created \n", i);
	struct packet_send sending_packet;
	struct packet_lastsend sending_lastpack;
	struct NACK_packet received_packet;
	struct sockaddr_in receiver;
	struct hostent* receiver_name;
	int packet_num=0;
	int sender_socket;
	int position, index;
	int to_send, to_recv, len, recv_TCP;
	struct timeval timer;
	struct NACK_number pack_num;
	int u=0;
	int var=0;
	//UDP
	receiver.sin_family = AF_INET;
	receiver.sin_addr.s_addr = inet_addr("192.168.0.161");
	receiver.sin_port = htons(PORT_NUMBER+i);
	len = sizeof(receiver);

	//_____________
	sender_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sender_socket==-1)
	{				
		perror("Server: socket\n");
		exit(0);				
	}


	//_____________define position___________________
	position = flag*100000000;

	if(i==9)
	{
		u = 6801;
	}
	else
	{
		u = 6803;
	}

	var = i*10000410;

	//______________first send loop
	for(index = 0, packet_num = 1;    packet_num<=u;   index = index + 1470, packet_num = packet_num+1)
	{
		//_____________clearing out payload field_____________________________
		bzero(sending_packet.payload,PACKET_SIZE);


		//_____________define a packet______________________________________
		if(packet_num == 6801 && i==9)
		{
			strncpy(sending_packet.payload, file_as_string+position+var+index, 310);
			sending_packet.sequence = packet_num;
		}
		else
		{
			strncpy(sending_packet.payload, file_as_string+position+var+index, sizeof(sending_packet.payload));
			sending_packet.sequence = packet_num;
		}

		
		//____________send a packet_____________________________
		to_send = sendto(sender_socket, &sending_packet, sizeof(sending_packet), 0, (struct sockaddr*)&receiver, sizeof(receiver));
		if(to_send == -1) 
		{
			perror("sendto \n"); 
			exit(1);
		} 
		

	}

event:

	recv_TCP = recv(new_socket[i], &pack_num, sizeof(pack_num),0);
	
	int z, count2;

	count2=1000;

	if(pack_num.no_of_NACKS > count2)
	{
		count2 = 1000;
	}
	else
	{
		count2 = pack_num.no_of_NACKS;
	}

	if(pack_num.no_of_NACKS !=0)
	{
		for(z=1 ; z<=count2 ; z++)
		{
			recv_TCP = recv(new_socket[i], &received_packet, sizeof(received_packet),0);

			if(received_packet.sequence == 6801 && i==9)
			{
				
				strncpy(sending_packet.payload, file_as_string+position+var+((received_packet.sequence-1)*1470), sizeof(sending_lastpack.payload));
				sending_packet.sequence = received_packet.sequence;
			}
			else
			{
				strncpy(sending_packet.payload, file_as_string+position+var+((received_packet.sequence-1)*1470), sizeof(sending_packet.payload));
				sending_packet.sequence = received_packet.sequence;
			}
			
			
			to_send = sendto(sender_socket, &sending_packet, sizeof(sending_packet), 0, (struct sockaddr*)&receiver, sizeof(receiver));
			if(to_send == -1) 
			{
				perror("sendto \n"); 
				exit(1);
			}
			
		}
		
		goto event;
	}

	close(sender_socket);
	if(flag == 9)
	{
		close(new_socket[i]);
	}
	
	return NULL;

}








int main(int argc, char* argv[])
{
	pthread_t thread_id[10],thread_id_10;
	struct timeval start, end;
	struct sockaddr_in receiver;
	int to_send, to_recv, len;
	char send_info[256];
	unsigned long end_time, print_time;
	int sender_socket;
	time_t now1, now2;
	//printf("check1\n");
	//____________open and get size of file and put it in a string___________________
	file = fopen(argv[1],"r");
	to_write = fopen("written.txt","w");

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	fseek(file, 0, SEEK_SET);
	file_as_string = malloc(file_size+1);
	file_as_string[file_size+1]=='\0';

	bzero(file_as_string,sizeof(file_as_string));
	
	fread(file_as_string, 1, file_size, file);

	//printf("check2\n");

	//____________UDP check
	receiver.sin_family = AF_INET;
	receiver.sin_addr.s_addr = inet_addr("192.168.0.161");
	receiver.sin_port = htons(30000);
	len = sizeof(receiver);
	sender_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sender_socket==-1)
	{				
		perror("Server: socket\n");
		exit(0);				
	}

	printf("Please enter message\n");
	bzero(send_info,256);
	fgets(send_info,255,stdin);

	to_send = sendto(sender_socket, send_info, sizeof(send_info), 0, (struct sockaddr*)&receiver, sizeof(receiver));
	if(to_send == -1) 
	{
		perror("sendto"); 
		exit(1);
	} 


	//_____________TCP_______________
	for(int i=0; i<10; i++)
	{
		sender_socket_TCP[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (sender_socket==-1)
		{				
			perror("Server: socket\n");
			exit(0);				
		}

		bzero((char *)&sender_TCP, sizeof(sender_TCP));
		sender_TCP.sin_family = AF_INET;
		sender_TCP.sin_port = htons(PORT_NUMBER_TCP + i);
		sender_TCP.sin_addr.s_addr = INADDR_ANY;

		bind_TCP = bind(sender_socket_TCP[i], (struct sockaddr *) &sender_TCP, sizeof(sender_TCP));
		if(bind_TCP<0)
		{
		printf("Error in binding the TCP socket\n");
		exit(1);
		}

		listen(sender_socket_TCP[i], 10);
		
		//printf("thread %d is listening \n", i);
	}

	for(int w=0; w<10; w++)
	{
		new_socket[w] = accept(sender_socket_TCP[w], NULL, NULL);
		if(new_socket[w]<0)
		{
			printf("Error in accepting the TCP socket\n");
		}

		//printf("thread %d got accept \n", w);
	}


	gettimeofday(&start, NULL);
	time(&now1);
	printf("Start time %s \n",ctime(&now1));
	start_time = 1000000*start.tv_sec + start.tv_usec;

	//10 threads deal with 100MB at a time. We call these 10 threads 10 times
next_loop:

	pthread_create(&thread_id[0], NULL, file_send,(void*)(intptr_t) 0);
	pthread_create(&thread_id[1], NULL, file_send,(void*)(intptr_t) 1);
	pthread_create(&thread_id[2], NULL, file_send,(void*)(intptr_t) 2);
	pthread_create(&thread_id[3], NULL, file_send,(void*)(intptr_t) 3);
	pthread_create(&thread_id[4], NULL, file_send,(void*)(intptr_t) 4);
	pthread_create(&thread_id[5], NULL, file_send,(void*)(intptr_t) 5);
	pthread_create(&thread_id[6], NULL, file_send,(void*)(intptr_t) 6);
	pthread_create(&thread_id[7], NULL, file_send,(void*)(intptr_t) 7);
	pthread_create(&thread_id[8], NULL, file_send,(void*)(intptr_t) 8);
	pthread_create(&thread_id[9], NULL, file_send,(void*)(intptr_t) 9);

	
	for(int p=0; p<10; p++)
	{
		pthread_join(thread_id[p],NULL);
	}

	flag++;

	if (flag<10)
	{
		goto next_loop;
	}

		//fputs(file_as_string, to_write);


	usleep(30000000);
	usleep(30000000);
	usleep(30000000);
	usleep(30000000);
	usleep(30000000);
	usleep(30000000);
	usleep(30000000);
	usleep(30000000);
	usleep(300000000);

	gettimeofday(&end, NULL);
	time(&now2);
	printf("End time %s \n",ctime(&now2));
	end_time = 1000000*end.tv_sec + end.tv_usec;
	print_time = ((end_time) - start_time);
    
	printf("\n \n TIME ELAPSED: %0.3fms OR %0.3fs \n", print_time/1000.0, print_time/1000000.0);
	printf("\nThroughput: %0.3fMbps \n",1000*8.0/(print_time/1000000.0));
}