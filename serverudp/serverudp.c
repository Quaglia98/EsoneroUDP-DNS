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
#include <stdlib.h> // for atoi()
#include <ctype.h>
#include <signal.h>
#include <math.h>
#include "protocol.h"
#include "utilities.h"

void clearwinsock() {
	#if defined WIN32
	WSACleanup();
	#endif
}


void errorhandler(char *errorMessage) {
     printf ("%s", errorMessage);
}


int main(int argc, char *argv[]) {

		#if defined WIN32 // initialize Winsock
		WSADATA wsa_data;
		int result2 = WSAStartup(MAKEWORD(2,2), &wsa_data);
		if (result2 != 0) {
			errorhandler("Error at WSAStartup()\n");
			return 0;
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


		int sock;
		struct sockaddr_in ServAddr;
		struct sockaddr_in ClntAddr;
		struct hostent *hostaddr;
		struct hostent *ipserv;
		int cliAddrLen;
		char sendBuffer[MAXBUFF];
		char recvBuffer[MAXBUFF];
		int operand1 = 0;
		int operand2 = 0;
		float result = -1 ;
		char operator = ' ';
		int neg1 = 0;
		int neg2 = 0;



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

		memset(&ServAddr, 0, sizeof(ServAddr));
		ServAddr.sin_family = AF_INET;
		ServAddr.sin_port = htons(port);
		ServAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*ina));

		//Bind socket
		if ((bind(sock, (struct sockaddr *)&ServAddr, sizeof(ServAddr))) < 0){
			errorhandler("bind() failed");
			closesocket(sock);
			clearwinsock();
			exit(1);
		}


		while (1) {

			memset(sendBuffer,0,sizeof(sendBuffer));
			memset(recvBuffer,0,sizeof(recvBuffer));
			memset(&ClntAddr, 0, sizeof(ClntAddr));
			printf("Waiting for a client request...\n");

			cliAddrLen = sizeof(ClntAddr);
			if(recvfrom(sock, recvBuffer, MAXBUFF, 0, (struct sockaddr*)&ClntAddr, &cliAddrLen) < 0){
				errorhandler("Error while receiving data from the client\n");
			}

			//getting host name
			hostaddr = gethostbyaddr((char*) &ClntAddr.sin_addr, 4, AF_INET);
			char* namehost = hostaddr->h_name;

			printf("Requested operation '%s' from the client %s, ip %s\n", recvBuffer, namehost, inet_ntoa(ClntAddr.sin_addr));

			if(parseExp(recvBuffer, &operator, &operand1, &operand2, &neg1, &neg2)  < 0 ){
				//reset booleans neg1 and neg2
				neg1 = 0;
				neg2 = 0;
				errorhandler("Error parsing the operation\n");
				printf("\n___________________________________________________________\n");
				char* errore = "Operation not valid, try again";

				if(sendto(sock, errore, strlen(errore), 0, (struct sockaddr *)&ClntAddr,
						sizeof(ClntAddr)) < 0) {
					errorhandler("Error while sending data to the client\n");
				}


			}

			//expression valid
			else{

				//checking if the operand1 and/or the operand2 are negative
			    if(neg1){
			    	int temp1 = 0;
					temp1 = -abs(operand1);
					operand1 = temp1;
					neg1 = 0;
			    }
			    if(neg2){
				    int temp2 = 0;
					temp2 = -abs(operand2);
					operand2 = temp2;
					neg2 = 0;
			    }

			switch(operator){
			case '+': result = add(&operand1,&operand2);
			break;
			case '-': result = sub(&operand1,&operand2);
			break;
			case '/': result = division(&operand1,&operand2);
			break;
			case 'x': result = mult(&operand1,&operand2);
			break;
			}

			printf("Sending result to the client...\n");

			if(operator == '/') //case division
				sprintf(sendBuffer, "%d %c %d = %.2f", operand1, operator, operand2, result);
			else //other cases
				sprintf(sendBuffer, "%d %c %d = %d", operand1, operator, operand2, (int)result);


			if(sendto(sock, sendBuffer, strlen(sendBuffer), 0, (struct sockaddr *)&ClntAddr,
									sizeof(ClntAddr)) < 0){
				errorhandler("Error while sending data to the client\n");

			}
		    printf("\n___________________________________________________________\n");

			}
		}

		return 0;
}
