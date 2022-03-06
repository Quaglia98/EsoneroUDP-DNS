#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

void errorhandler(char *error_message) {
	printf("%s",error_message);
}


void clearwinsock() {
	#if defined WIN32
	WSACleanup();
	#endif
}




int main(int argc, char *argv[]) {

	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (iResult != 0) {
		printf ("error at WSASturtup\n");
		return EXIT_FAILURE;
	}
	#endif

	char* s_name;
	int port;

	//param 'servername:port'
	if (argc > 1){
		char s[2] = ":";
		s_name = strtok(argv[1],s);
		port = atoi(strtok(NULL,"\0"));
	}

    //default 'localhost:57700'
	else {
		port = PROTOPORT;
		s_name = SERVERNAME;
	}



	struct sockaddr_in ServAddr;
	struct sockaddr_in fromAddr;
	struct hostent *ipserv;
	struct hostent *hostserver;
	int fromSize;
	int StringLen;
	int sock;
	char sendBuffer[MAXBUFF];
	char recvBuffer[MAXBUFF];


	//Create socket
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		errorhandler("socket() failed");
		clearwinsock();
		system("PAUSE");
		exit(1);
	}

		ipserv=gethostbyname(s_name);
		if(ipserv==NULL){
			printf("Gethostbyname failed\n");
			system("PAUSE");
			exit(1);
		}

		struct in_addr *ina = (struct in_addr*) ipserv->h_addr_list[0];

        //Create Server address
		memset(&ServAddr, 0, sizeof(ServAddr));
		ServAddr.sin_family = PF_INET;
		ServAddr.sin_port = htons(port);
		ServAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*ina));

		do{
			memset(recvBuffer,0, sizeof(recvBuffer));
			printf("\nEnter your expression or close the process with '='.\n") ;
			printf("[operator] space [operand1] space [operand2]\n");

			if(fgets(sendBuffer, sizeof(sendBuffer), stdin) == NULL) {
				errorhandler("Error getting the expression\n");
				break;
			}
			sendBuffer[strlen(sendBuffer)-1] = '\0';


			if(strcmp("=", sendBuffer) == 0){
				printf("Closing client process...\n");
				break;
			}

			if ((StringLen = strlen(sendBuffer)) > MAXBUFF){
				errorhandler("sendBuffer's size is over the max\n");
				break;
			}

			if (sendto(sock, sendBuffer, StringLen, 0, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) != StringLen){
				errorhandler("sendto() sent different number of bytes than expected");
				break;
			}

			fromSize = sizeof(fromAddr);
			if(recvfrom(sock, recvBuffer, MAXBUFF, 0, (struct sockaddr*)&fromAddr, &fromSize) < 0){
				errorhandler("recv() failed \n");
				break;
			}

			if (ServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
				errorhandler("Error: received a packet from unknown source.\n");
			    break;
			}
			else{

				hostserver = gethostbyaddr((char*) &fromAddr.sin_addr, 4, AF_INET);
				char* name_server = hostserver->h_name;

				printf("Received result from the server %s, ip: %s: %s\n", name_server, inet_ntoa(fromAddr.sin_addr), recvBuffer);
				memset(recvBuffer,0, sizeof(recvBuffer));
			}
		}
		while(1);


	//Close
	closesocket(sock);
	clearwinsock();
	printf("\n");    // Print a final linefeed
	system("pause");
	return(0);

}
