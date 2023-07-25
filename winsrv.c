/* by Alexey Subbotin */
#include<winsock2.h> // #include<sock.h>
#include<sys/types.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#pragma comment(lib, "Ws2_32.lib")

// only Win socket()
int init() {
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
}
// out only Win socket()
void clean() {
	WSACleanup();
}

void server(int port) {
	int sock, connected, bytes_recieved, true = 1;  
	char send_data [1024], recv_data[1024];       
	struct sockaddr_in server_addr,client_addr;    
	int sin_size;
        
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

    int iOptVal = 0;
	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *) &iOptVal,sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(1);
	}
        
	server_addr.sin_family = AF_INET;         
	server_addr.sin_port = htons(port);     
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	memset(&(server_addr.sin_zero),0,8);
	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))== -1) {
		perror("Unable to bind");
		exit(1);
	}

	if (listen(sock, 5) == -1) {
		perror("Listen");
		exit(1);
	}
  
	printf("\n[HTTP Server waiting on port %i] ...\n Link: http://localhost:%i/ \n", port, port);
	fflush(stdout);
	sin_size = sizeof(struct sockaddr_in);
	connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);
	char kk[9999];
	recv(connected,kk,sizeof(kk),0);
	printf("\n Received:%s",kk); 
   
	char xx[9999];
	strcpy(xx,"HTTP/1.1 200 OK\nContent-length: 47\nContent-Type: text/html\n\n<html><body><H1>Web Server Alexey Subbotin</H1></body></html>");
	int c=send(connected,xx,sizeof(xx),0);
	printf("\nSTATUS:%d",c);
	printf("\nSent : %s\n",xx);
	close(sock);
	WSACleanup();
}

// point enter.
int main(int argc, char **argv) {
	while(1) {
		init();
		server(atoi(argv[1]));
		clean();
	}
	return 0;
}