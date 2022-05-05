#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define PENDING_QUEUE_SIZE 1 
#define MAX_MESSAGE_SIZE 128
#define LOCAL       "./sockA"
#define DESTINATION "./sockB" 

int main(int argc, const char *argv[]){
	struct sockaddr_un address;
	int fd, connected_fd;
	char message[MAX_MESSAGE_SIZE];
	message[MAX_MESSAGE_SIZE -1] = '\0';

	
	address.sun_family = AF_UNIX;
	memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1); 

	
	if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("Socket cannot be instantiated");
		exit(EXIT_FAILURE);
	}

	
	unlink(LOCAL);
	if(bind(fd, (struct sockaddr*) &address, sizeof(address)) == -1){
		perror("Cannot bind socket");
		exit(EXIT_FAILURE);
	}

	
	if(listen(fd, PENDING_QUEUE_SIZE) == -1){
		perror("Cannot listen");
		exit(EXIT_FAILURE);
	}

	
	struct sockaddr_un agent; 
	socklen_t agent_length;
	memset(&agent, 0, sizeof(struct sockaddr_un));
	printf("Waiting for incoming connection...\n");
	if((connected_fd = accept(fd,(struct sockaddr*) &agent, &agent_length))== -1){
		perror("Cannot accept connection");
		exit(EXIT_FAILURE);
	}
	printf("Established connection with: %s\n", agent.sun_path); 
	char *buffer = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
	size_t buffer_size = MAX_MESSAGE_SIZE;
	size_t size;
	do{
		printf("Write a message: ");
		getline(&buffer, &buffer_size, stdin);
		memset(message, 0, MAX_MESSAGE_SIZE);
		memcpy(message, buffer, strlen(buffer) -1 ); 
		
		size = send(connected_fd, message, strlen(message), 0);
		printf("Sent %lu B\n", size);
	}while(strcmp(message, "quit") != 0);
	free(buffer);



	close(connected_fd);
	close(fd); 
	return 0;
}
