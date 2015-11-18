/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 100
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc)
{
	FILE *fp;
	int sockfd, numbytes;
	int topology[4];
	char buf[MAXDATASIZE], *send_buf;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int mapper = 0;
	char s[INET6_ADDRSTRLEN];
	int file_size;
	char *start_ptr, *tab_ptr = buf;
	int count = 0, return_value = 0;
	memset(topology,0,sizeof(topology));
	/*Phase 1 of the code*/
	{
		fp = fopen("serverA.txt", "r");
		fseek(fp,0,SEEK_END);
		file_size = ftell(fp);
		fseek(fp,0,SEEK_SET);

		send_buf = (char *)malloc(sizeof(char) * file_size);

		if(fp == NULL) {
			perror("Error while reading serverA.txt\n");
		}

		while(fgets(buf,80,fp) != NULL) {
//			strcat(send_buf, buf);
			tab_ptr = buf;
			do {
				start_ptr = tab_ptr;
				tab_ptr = strchr(start_ptr, ' ');
				if (tab_ptr != NULL) {
				 *tab_ptr++ = '\0';
				}
				count++;
				if(count % 2 == 1) {
					return_value = strcmp(start_ptr,"serverA");
					if(mapper == 0) {mapper = (return_value == 0) ? 1 : 0;}
					printf("1 return : %d mapper : %d\n",return_value, mapper);
					return_value = strcmp(start_ptr,"serverB");
					if(mapper == 0) {mapper = (return_value == 0) ? 2 : 0;}
					printf("2 return : %d mapper : %d\n",return_value, mapper);
					return_value = strcmp(start_ptr,"serverC");
					if(mapper == 0) {mapper = (return_value == 0) ? 3 : 0;}
					printf("3 return : %d mapper : %d\n",return_value, mapper);
					return_value = strcmp(start_ptr,"serverD");
					if(mapper == 0) {mapper = (return_value == 0) ? 4 : 0;}
					printf("4 return : %d mapper : %d\n",return_value, mapper);
				}

//				printf("count : %d start : %s mapper : %d\n",count,start_ptr,mapper);
				if(count % 2 == 0) {
					switch(mapper) {
					case 1:topology[0] = atoi(start_ptr);
							printf("top[0] : %d\n",topology[0]);
							break;
					case 2:topology[1] = atoi(start_ptr);
							printf("top[1] : %d\n",topology[1]);
							break;
					case 3:topology[2] = atoi(start_ptr);
							printf("top[2] : %d\n",topology[2]);
							break;
					case 4:topology[3] = atoi(start_ptr);
							printf("top[3] : %d\n",topology[3]);
							break;
					default : printf("Error in reading the file %d mapper\n",mapper);
					}
					mapper = 0;
				}
			} while(tab_ptr != NULL);
		}
		printf("[0] : %d [1] : %d [2] : %d [3] : %d\n",topology[0],topology[1],topology[2],topology[3]);
		if (argc != 1) {
			fprintf(stderr,"usage: client hostname\n");
			exit(1);
		}

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("client: connect");
				continue;
			}

			break;
		}

		if (p == NULL) {
			fprintf(stderr, "client: failed to connect\n");
			return 2;
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);
		printf("client: connecting to %s\n", s);

		freeaddrinfo(servinfo); // all done with this structure

		if (send(sockfd, send_buf, file_size, 0) == -1)
			perror("send");

		close(sockfd);
	}/*End of Phase1*/
	return 0;
}
