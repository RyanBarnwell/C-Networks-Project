//Ryan Barnwell
//ECE 7428
//November 5, 2023
//Server Code

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#define MYPORT "7777" // the port users will be connecting to
//Line definition for sending file
#define MAX_LINE 512
#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void) {
	//variables
	FILE *fp;
	int sockfd;
	char line[MAX_LINE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	
	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}
	break;
	}
	//check if socket creation failed
	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}
	freeaddrinfo(servinfo);

	//create file to store incoming packets
	fp = fopen("file.txt", "w");
	if(fp == NULL){
		perror("file not opened");
	}
	//loops until all data is received
	while(1){
		printf("listener: waiting to recvfrom...\n");
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		printf("listener: got packet from %s\n",
		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
		printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';
		printf("listener: packet contains \"%s\"\n", buf);
		//check is close command was sent
		if(buf[0] == 'c'){
			break;;
		}
		//takes received packets and puts them in a file
		fputs(buf, fp);
		
	}
	fclose(fp);
	//opens file for reading
	fp = fopen("file.txt", "r");


	//wait ten seconds to give time for client to prepare
	sleep(10);
	while(1){
		//send txt file back to client
		if(fgets(line, MAX_LINE, fp) != NULL){
			sendto(sockfd, line, sizeof line, 0, p->ai_addr, p->ai_addrlen);
			printf("talker: sent %d bytes from %s\n", numbytes, line);
		}
		else{
			
			break;
		}
	}
	//send close command
	sendto(sockfd, "c", sizeof line, 0, p->ai_addr, p->ai_addrlen);
	close(sockfd);
	return 0;
}