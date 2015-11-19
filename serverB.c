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

#define PORT "25251" // the port client will be connecting to

#define MAXDATASIZE 100

#define MYPORT "22251"	// the port users will be connecting to

#define MAXBUFLEN 100
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void print_topology_A(int graph[4][4],int i, int j){
	switch(j) {
	case 0: printf("AA\t\t%d\n",graph[i][j]);
			break;
	case 1: printf("AB\t\t%d\n",graph[i][j]);
			break;
	case 2: printf("AC\t\t%d\n",graph[i][j]);
			break;
	case 3: printf("AD\t\t%d\n",graph[i][j]);
			break;
	default: fprintf(stderr,"Printing the graphA error\n");
	}
}

void print_topology_B(int graph[4][4],int i, int j){
	switch(j) {
	case 0: break;
	case 1: printf("BB\t\t%d\n",graph[i][j]);
			break;
	case 2: printf("BC\t\t%d\n",graph[i][j]);
			break;
	case 3: printf("BD\t\t%d\n",graph[i][j]);
			break;
	default: fprintf(stderr,"Printing the graphB error\n");
	}
}

void print_topology_C(int graph[4][4],int i, int j){
	switch(j) {
	case 0:
	case 1:break;
	case 2: printf("CC\t\t%d\n",graph[i][j]);
			break;
	case 3: printf("CD\t\t%d\n",graph[i][j]);
			break;
	default: fprintf(stderr,"Printing the graphC error\n");
	}
}

void print_topology_D(int graph[4][4],int i, int j){
	switch(j) {
	case 0:
	case 1:
	case 2: break;
	case 3: printf("DD\t\t%d\n",graph[i][j]);
			break;
	default: fprintf(stderr,"Printing the graphD error\n");
	}
}

void print_topology(int graph[4][4]) {
	int i,j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			if(graph[i][j] > 0) {
				switch(i){
				case 0: print_topology_A(graph, i, j);
						break;
				case 1: print_topology_B(graph, i, j);
						break;
				case 2: print_topology_C(graph, i, j);
						break;
				case 3: print_topology_D(graph, i, j);
						break;
				default : fprintf(stderr,"Printing the graph error\n");
				}
			}
		}
	}
}

int main(int argc)
{
	int i, j, topology_receive[4][4], adjacency[4][4];
	struct sockaddr *my_addr;
	socklen_t addrlen;
	int getsock_check;
	struct sockaddr_in sa;
	int sa_len;
	/*Phase 1 of the code*/
	{
		FILE *fp;
		int sockfd, numbytes;
		int topology[4];
		char buf[MAXDATASIZE];
		struct addrinfo hints, *servinfo, *p;
		int rv;
		int mapper = 0;
		char s[INET6_ADDRSTRLEN];
		char *start_ptr, *tab_ptr = buf;
		int count = 0, return_value = 0;
		memset(topology,0,sizeof(topology));
		int flag = 0;
		fp = fopen("serverB.txt", "r");

		if(fp == NULL) {
			perror("Error while reading serverB.txt\n");
		}

		printf("The Server B is up and running.\n");
		while(fgets(buf,80,fp) != NULL) {
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
					return_value = strcmp(start_ptr,"serverB");
					if(mapper == 0) {mapper = (return_value == 0) ? 2 : 0;}
					return_value = strcmp(start_ptr,"serverC");
					if(mapper == 0) {mapper = (return_value == 0) ? 3 : 0;}
					return_value = strcmp(start_ptr,"serverD");
					if(mapper == 0) {mapper = (return_value == 0) ? 4 : 0;}
				}

				if(count % 2 == 0) {
					switch(mapper) {
					case 1:topology[0] = atoi(start_ptr);
							break;
					case 2:topology[1] = atoi(start_ptr);
							break;
					case 3:topology[2] = atoi(start_ptr);
							break;
					case 4:topology[3] = atoi(start_ptr);
							break;
					default : fprintf(stderr,"Error in reading the file %d mapper\n",mapper);
					}
					mapper = 0;
				}
			} while(tab_ptr != NULL);
		}
		for(i=0;i<4;i++) {
			if(topology[i] > 0) {
				if(flag == 0) {
					printf("Neighbor-------Cost\n");
					flag = 1;
				}
				switch(i) {
				case 1: printf("serverA\t\t%d\n",topology[i]);
						break;
				case 2: printf("serverB\t\t%d\n",topology[i]);
						break;
				case 3: printf("serverC\t\t%d\n",topology[i]);
						break;
				case 4: printf("serverD\t\t%d\n",topology[i]);
						break;
				default : fprintf(stderr,"Error reading the file");
				}
			}
		}
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
		freeaddrinfo(servinfo); // all done with this structure
		if (send(sockfd, &topology, sizeof(topology), 0) == -1)
			perror("send");

		printf("The Server B finishes sending its neighbor information to the client with TCP"
				"port number %s and IP address %s (Client's TCP port number and IP address).\n",
				PORT,s);

		sa_len = sizeof(sa);
		if (getsockname(sockfd, (struct sockaddr * __restrict__)&sa, &sa_len) == -1) {
		  perror("getsockname() failed");
		  return 2;
		}

		printf("For this connection with the Client, the Server B has TCP "
				"port number %d and IP address %s.\n", (int) ntohs(sa.sin_port)
													,inet_ntoa(sa.sin_addr));
		close(sockfd);
	}/*End of Phase1*/

	/*Code for Phase 2*/
	{
		int sockfd;
		struct addrinfo hints, *servinfo, *p;
		int rv;
		int numbytes;
		struct sockaddr_storage their_addr;
		char buf[MAXBUFLEN];
		socklen_t addr_len;
		char s[INET6_ADDRSTRLEN];

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE; // use my IP

		if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
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

		if (p == NULL) {
			fprintf(stderr, "listener: failed to bind socket\n");
			return 2;
		}

		freeaddrinfo(servinfo);

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, (int *)topology_receive, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}

		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);

		sa_len = sizeof(sa);
		if (getsockname(sockfd, (struct sockaddr * __restrict__)&sa, &sa_len) == -1) {
		  perror("getsockname() failed");
		  return 2;
		}

		printf("The Server B has received the network topology from the Client with UDP port number %d and IP address %s "
				"(Client's UDP port number and IP address) as follows:\n",ntohs(((struct sockaddr_in*)&their_addr)->sin_port),
																									s);

		print_topology(topology_receive);

		printf("For this connection with Client, The Server B has UDP port number %d and IP address %s.\n",
															(int) ntohs(sa.sin_port),s);
		close(sockfd);
	} /*End of phase2*/

	return 0;
}
