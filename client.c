/* CS744--Assignment5(IIT Bombay)
* Author-- Satyam Behera
* Author-- Nitesh Singh
* Author-- Raghav Thakkar
*/ 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <fcntl.h>
#include "xml.h"
 
#define MAXLINE 262146




// Driver code 
int main(int argc,char const *argv[]) { 
	char typee[MAXLINE];
	int my_port = atoi(argv[2]);
	char * k;
	char * v;
	char * t;
	char key[258];
	char type[5];
	char value[MAXLINE];
	int sockfd; 
	char buffer[MAXLINE]; 
	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr));
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(atoi(argv[1])); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	while (1)
	{
		printf("enter command\n");
		scanf("%s",typee);
		strcpy(buffer,typee);
		t = strtok(typee, ",");
	
		if(strcmp(t,"PUT")==0)
		{
		strcpy(type, "I");
		k = strtok(NULL, ",");
		strcpy(key, k);
		if(strlen(key)>256)
		{
			perror("Oversized key");
			continue;
		}
		v = strtok(NULL, ",");
		strcpy(value, v);
		if(strlen(value)>262144)
		{
			perror("Oversized value");
			continue;
		}
         //filemaker(key,buffer,type)

		 xmlmaker(key,value,type);
		 int fd2 = open("client.xml",O_RDONLY);
        read(fd2,buffer,262146);
		 sendto(sockfd, (const char *)buffer, strlen(buffer), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr));
			bzero(buffer,262146);
			int n, len;
			n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				0, (struct sockaddr *) NULL, 
				NULL); 
	buffer[n] = '\0'; 
	printf("Server : %s\n", buffer); 
		}
		else if(strcmp(t,"GET")==0 || strcmp(t,"DEL")==0)
		{
			if(strcmp(t,"GET")==0)
			strcpy(type,"S");
			else
			strcpy(type, "D");
			k = strtok(NULL, ",");
			strcpy(key, k);
			if(strlen(key)>256)
		{
			perror("Oversized key");
			continue;
		}
		//filemaker(key,buffer,type);
			xmlmaker(key,NULL,type);
			int fd2 = open("client.xml",O_RDONLY);
        	read(fd2,buffer,262146);
		 sendto(sockfd, (const char *)buffer, strlen(buffer), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr));
			bzero(buffer,262146);
			int n, len;
			n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, (struct sockaddr *) &servaddr, 
				&len); 
	buffer[n] = '\0'; 
	printf("Server : %s\n", buffer);	
		}
		
		else
		{
			printf("enter the valid command\n");
			continue;
		}
		
	}
	close(sockfd); 
	return 0; 
} 
