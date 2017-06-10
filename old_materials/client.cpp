//--------------------------------------------------------------------------------
//  Code		: Client to send a message to the server
//  Subject		: Computer Network
//  Professor	: Sang Bang Choi
//  File		: Client.cpp 
//--------------------------------------------------------------------------------

#include <stdio.h>			// Set to use printf() Function
#include <stdlib.h>			// Set to use exit() Function
#include <winsock2.h>		// Set to use a Socket (Using winsock.h or winsoch2.h)
#pragma comment (lib, "ws2_32.lib")
 
#define MAX_SIZE 64			// 한번에 보낼 데이터 크기를 최대 64(Bytes)로 지정

void main(){ 
	//Winsock_data 선언(WSADATA type structure)
	WSADATA winsock_data;
	//socket 선언
	SOCKET Client_Socket; 
	// declare structure of socket address information.
	SOCKADDR_IN Server_Address; 
	// 접속할 포트 넘버 지정. 2000 ~ 65535까지 임의적으로 설정
	unsigned short Server_Port = 6801;
	int ReturnVal; 

	//Windows socket start 시작전, socket DLL files 초기화
	if(WSAStartup(0x202,&winsock_data) == SOCKET_ERROR){
		printf( "WSAStartup Setting Error.\n" );
		// Close initialization failure.
		WSACleanup();
		exit(0);
	}

	int count=1;	// 접속수를 count함
	
	// send message 변수 선언 (Max.Size = MAX_SIZE)
	char SendMessage[MAX_SIZE];
	char Tmp[MAX_SIZE]="Off"; // Compare to the Client end.
	

	while(strcmp(SendMessage,Tmp) != 0){
	
		// socket 생성
		Client_Socket = socket(AF_INET, SOCK_STREAM,0);

		// socket 생성 error시
		if( Client_Socket == INVALID_SOCKET ){ 
			printf("Create Socket Error.");
			WSACleanup(); 
			exit(0);
		}

		// TCP를 사용하므로 AF_INET 사용
		Server_Address.sin_family = AF_INET;

		// IP address and connect port number
		// Test IP address : 127.0.0.1 (My computer internal IP address)
		Server_Address.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP address
		Server_Address.sin_port = htons(Server_Port);				// Port Number

		// Attempts to connect to the server.
		ReturnVal = connect(Client_Socket, (struct sockaddr*)&Server_Address, sizeof(Server_Address));

		printf("\n --------------------------------------------------------\n");

		// if ReturnVal is 1, Connection setup error.
		if(ReturnVal){ 
			printf(" >>  Server Connection Error.  << \n\n");
			closesocket(Client_Socket); 
			WSACleanup(); 
			exit(0); 
		}
		else{ 
			// Count the number of connections.
			printf(" >> connected to the server( %d times ).  << \n\n",count++);
		}

		int SendSize = MAX_SIZE; 
		char x[MAX_SIZE]; //입력할 x값
		printf("x값을 입력하세요. "); 
		scanf("%s", &x); //x값을 입력받습니다.
		send(Client_Socket, x, SendSize, 0); //char형으로 변환한 x값을 전송합니다.
		printf("결과값 : %s \n", x); //result 값을 출력합니다.
	}//while end.

	closesocket(Client_Socket);
	WSACleanup();
	
	// Wait
	getchar();
}


