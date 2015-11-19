/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>

#define PORT "25251"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

#define MAXDATASIZE 100 // max number of bytes we can get at once

#define SERVERPORT_A "21251"	// the port users will be connecting to
#define SERVERPORT_B "22251"	// the port users will be connecting to
#define SERVERPORT_C "23251"	// the port users will be connecting to
#define SERVERPORT_D "24251"	// the port users will be connecting to

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Number of vertices in the graph
#define V 4

typedef int bool;
enum { false, true };

char name_port[4] = {'A','B','C','D'};

// A utility function to find the vertex with minimum key value, from
// the set of vertices not yet included in MST
int minKey(int key[], bool mstSet[])
{
	int v;
   // Initialize min value
   int min = INT_MAX, min_index;

   for (v = 0; v < V; v++)
     if (mstSet[v] == false && key[v] < min)
         min = key[v], min_index = v;

   return min_index;
}

// A utility function to print the constructed MST stored in parent[]
int printMST(int parent[], int n, int graph[V][V])
{
	int i;
   printf("Edge   Weight\n");
   for (i = 1; i < V; i++)
      printf("%d - %d    %d \n", parent[i], i, graph[i][parent[i]]);
}

// Function to construct and print MST for a graph represented using adjacency
// matrix representation
void primMST(int graph[V][V])
{
     int parent[V]; // Array to store constructed MST
     int key[V];   // Key values used to pick minimum weight edge in cut
     bool mstSet[V];  // To represent set of vertices not yet included in MST
     int i, count;
     // Initialize all keys as INFINITE
     for (i = 0; i < V; i++)
        key[i] = INT_MAX, mstSet[i] = false;

     // Always include first 1st vertex in MST.
     key[0] = 0;     // Make key 0 so that this vertex is picked as first vertex
     parent[0] = -1; // First node is always root of MST

     // The MST will have V vertices
     for (count = 0; count < V-1; count++)
     {
    	 int v;
        // Pick thd minimum key vertex from the set of vertices
        // not yet included in MST
        int u = minKey(key, mstSet);

        // Add the picked vertex to the MST Set
        mstSet[u] = true;

        // Update key value and parent index of the adjacent vertices of
        // the picked vertex. Consider only those vertices which are not yet
        // included in MST
        for (v = 0; v < V; v++)

           // graph[u][v] is non zero only for adjacent vertices of m
           // mstSet[v] is false for vertices not yet included in MST
           // Update the key only if graph[u][v] is smaller than key[v]
          if (graph[u][v] && mstSet[v] == false && graph[u][v] <  key[v])
             parent[v]  = u, key[v] = graph[u][v];
     }

     // print the constructed MST
     printMST(parent, V, graph);
}

void print_neighbor_information(int graph[4], int count) {
	int i, flag = 0;
	for(i=0;i<4;i++) {
		if(graph[i] > 0) {
			if(flag == 0) {
				printf("The Server %c has the following neighbor information:\n",name_port[count]);
				printf("Neighbor-------Cost\n");
				flag = 1;
			}
			switch(i) {
			case 0: printf("serverA\t\t%d\n",graph[i]);
					break;
			case 1: printf("serverB\t\t%d\n",graph[i]);
					break;
			case 2: printf("serverC\t\t%d\n",graph[i]);
					break;
			case 3: printf("serverD\t\t%d\n",graph[i]);
					break;
			}
		}
	}
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

int main(void)
{
	int temp[4], topology[4][4], adjacency[4][4];
	struct hostent *he;
	struct in_addr **addr_list;
	struct sockaddr_in sa_port;
	int sa_len;

	/*Phase 1*/
	{
		int sockfd, new_fd, numbytes;  // listen on sock_fd, new connection on new_fd
		struct addrinfo hints, *servinfo, *p;
		struct sockaddr_storage their_addr; // connector's address information
		socklen_t sin_size;
		char buf[MAXDATASIZE];
		struct sigaction sa;
		int yes=1;
		char s[INET6_ADDRSTRLEN];
		int rv, count = 0;
		int i,j;
		int port_count;

	    if ((he = gethostbyname("localhost")) == NULL) {  // get the host info
	        herror("gethostbyname");
	        return 2;
	    }

	    addr_list = (struct in_addr **)he->h_addr_list;
	    for(port_count = 0; addr_list[port_count] != NULL; port_count++) {
	        printf("The Client has TCP port number %s and IP address %s.\n",PORT, inet_ntoa(*addr_list[port_count]));
	    }

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE; // use my IP

		if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				perror("server: socket");
				continue;
			}

			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					sizeof(int)) == -1) {
				perror("setsockopt");
				exit(1);
			}

			if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("server: bind");
				continue;
			}

			break;
		}

		freeaddrinfo(servinfo); // all done with this structure

		if (p == NULL)  {
			fprintf(stderr, "server: failed to bind\n");
			exit(1);
		}

		if (listen(sockfd, BACKLOG) == -1) {
			perror("listen");
			exit(1);
		}

		sa.sa_handler = sigchld_handler; // reap all dead processes
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &sa, NULL) == -1) {
			perror("sigaction");
			exit(1);
		}

		for(count = 0; count < 4; count++) {  // main accept() loop
			sin_size = sizeof their_addr;
			new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
			if (new_fd == -1) {
				perror("accept");
				continue;
			}

			inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s);

			if ((numbytes = recv(new_fd, (int *)temp, MAXDATASIZE-1, 0)) == -1) {
				perror("recv");
				exit(1);
			}

			sa_len = sizeof(sa_port);
			if (getsockname(new_fd, (struct sockaddr * __restrict__)&sa_port, &sa_len) == -1) {
			  perror("getsockname() failed");
			  return 2;
			}

			printf("The Client receives neighbor information from the Server %c with TCP port number %d and IP address "
					"%s. (The Server %c's TCP port number and IP address)\n",name_port[count],
													(int) ntohs(sa_port.sin_port), inet_ntoa(sa_port.sin_addr),name_port[count]);

			print_neighbor_information(temp, count);

			printf("For this connection with Server %c, The Client has TCP port number %s and IP address %s.\n",name_port[count],
																	PORT,inet_ntoa(*addr_list[port_count-1]));
			fflush(stdout);
			for(i = 0; i < 4; i++) {
				topology[count][i] = temp [i];
			}
		}
		for(i = 0; i < 4; i++) {
			for(j = 0; j < 4; j++) {
				if(topology[i][j] > 0)
					adjacency[i][j] = 1;
				else
					adjacency[i][j] = 0;
			}
		}
		close(sockfd);
		close(new_fd);  // don't need this
	}/*End of Phase1*/

	/*Phase 2*/
	{
		int count;
		char s[INET6_ADDRSTRLEN];
		/*UDP client for portA,B,C,D*/
		for(count=0;count<4;count++)
		{
			int sockfd;
			struct addrinfo hints, *servinfo, *p;
			int rv;
			int numbytes;

			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_DGRAM;

			if(count == 0) {
				if ((rv = getaddrinfo("localhost", SERVERPORT_A, &hints, &servinfo)) != 0) {
					fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
					return 1;
				}
			} else if(count == 1) {
				if ((rv = getaddrinfo("localhost", SERVERPORT_B, &hints, &servinfo)) != 0) {
					fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
					return 1;
				}
			} else if(count == 2) {
				if ((rv = getaddrinfo("localhost", SERVERPORT_C, &hints, &servinfo)) != 0) {
					fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
					return 1;
				}
			} else if(count == 3) {
				if ((rv = getaddrinfo("localhost", SERVERPORT_D, &hints, &servinfo)) != 0) {
					fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
					return 1;
				}
			}

			// loop through all the results and make a socket
			for(p = servinfo; p != NULL; p = p->ai_next) {
				if ((sockfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
					perror("talker: socket");
					continue;
				}

				break;
			}

			if (p == NULL) {
				fprintf(stderr, "talker: failed to create socket\n");
				return 2;
			}

			if ((numbytes = sendto(sockfd, &topology, sizeof(topology), 0,
					 p->ai_addr, p->ai_addrlen)) == -1) {
				perror("talker: sendto");
				exit(1);
			}

			freeaddrinfo(servinfo);

			inet_ntop(p->ai_family,get_in_addr(p->ai_addr),s, sizeof s);

			printf("\nThe Client has sent the network topology to the Server %c with UDP port number %s and IP address %s"
					" (Server %c's UDP port number and IP address) as follows.\n",name_port[count],SERVERPORT_A,s
													,name_port[count]);

			print_topology(topology);

			sa_len = sizeof(sa_port);
			if (getsockname(sockfd, (struct sockaddr * __restrict__)&sa_port, &sa_len) == -1) {
			  perror("getsockname() failed");
			  return 2;
			}

			printf("\nFor this connection with Server %c, the Client has UDP port number %d and IP address %s.\n",name_port[count],
													(int) ntohs(sa_port.sin_port), inet_ntoa(sa_port.sin_addr));
			close(sockfd);
		}/*End of UDP for portA,B,C,D*/
	}

	/*Phase 3, calculate the tree*/
	{
		primMST(topology);
	}
	return 0;
}
