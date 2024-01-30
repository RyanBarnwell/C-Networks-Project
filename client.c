//Ryan Barnwell
//ECE 7428
//November 5, 2023
//Client Code

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
#include <stdbool.h>

//port
#define SERVERPORT "7777"

//Set up file sizes
#define FILE_SIZE 16
#define MAX_LINE 512
#define MAXBUFLEN 100

//isInArray function for code clarity, and to check if a file has been sent
bool isInArray(int value, int *arr, int size){
	for(int i = 0; i<size; i++) {
		if(arr[i] == value){
			return true;
		}
	}
	return false;
}



int main(int argc, char *argv[])
{
	//file info
	FILE *file;
	char line[MAX_LINE];
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	struct sockaddr_storage their_addr;

	//set up file naming convention, make file 6 long
	char name[FILE_SIZE] = "File00";
	//array to be used for checking if file has been sent
	int filesUsed[10];
	//count for array check
	int count = 0;
	//make array to keep track of lines left in each file
	int linesSent[10];
	
	
	//Socket info
	int sock;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	//beej's source code
	memset(&hints, 0, sizeof hints);
	//Set up IPv4
	hints.ai_family = AF_INET6;
	//set socket type
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	//loop through all Files.
	for(int i = 1; i<=10; i++) {
		//Obtain rand val to use for file, 49 is used to compensate for ASCII
		int randVal = rand() % 10 +49;
		//check for file 10
		if(randVal == 58){
			name[4] = '1';
			name[5] = '0';
			//set randVal to 48, '0' to ensure sent check works
			randVal = 48;
		}
		else{
			//apply rand val to file
			name[4] = '0';
			name[5] = (char)randVal;
		}

		//Check if curr File has been sent
		if(isInArray(name[5], filesUsed, 10)){
			i--;
			//skips rest of code if File has been accessed
			continue;
		}

		//test if print all files
		printf("%s \n", name);
		//printf("%d \n", randVal);

		//open file
		file = fopen(name, "r");
		
		//Check for no file error
		if(file == NULL){
			perror("File is not opened");
		}

		//make packet, beej's source code
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sock = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
				perror("talker: socket");
				continue;
			}
			break;
		}
		//return if socket creation failed, beej's source code
		if (p == NULL) {
			perror("talker: failed to create socket\n");
		}


		//get 1-3 lines
		//int randVal2 = rand() % 3+1;
		for(int j = 0; j < 11; j++){
			if(fgets(line, MAX_LINE, file) != NULL){
				//Sends File, beej's, sendto will send a char array containing each line
				if ((numbytes = sendto(sock, line, strlen(argv[2]), 0,
					p->ai_addr, p->ai_addrlen)) == -1) {
					perror("talker: sendto");
					exit(1);
				}
				//printf("%s \n", line);
				printf("talker: sent %d bytes from %s\n", numbytes, name);
				j--;
			}
			//adds packet if all lines sent
			else{
				filesUsed[count] = randVal;
				count++;
				break;
			}
		}

		//close file and socket
		fclose(file);
	}
	//final send to acknowledge packet close
	sendto(sock, "c", strlen(argv[2]), 0, p->ai_addr, p->ai_addrlen);

	//open file for writing
	file = fopen("concatfile", "w");
	while(1){
		//receive concat file
		printf("listener: waiting to recvfrom...\n");
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sock, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		//check is close command was sent
		if(buf[0] == 'c'){
			break;;
		}
		//stores data in file after check for close statement
		fputs(buf, file);
	}

	fclose(file);
	freeaddrinfo(servinfo);
	close(sock);
	return (0);
}