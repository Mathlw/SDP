#include <stdio.h>			// Set to use printf() Function
#include <stdlib.h>			// Set to use exit() Function
#include <winsock2.h>		// Set to use a Socket (Using winsock.h or winsoch2.h)
#pragma comment (lib, "ws2_32.lib")

#define MAX_SIZE 64         // 한번에 보낼 데이터 크기를 최대 64(Bytes)로 지정 

void main(){   

	//Winsock_data 선언(WSADATA type structure)
	WSADATA winsock_data; 
	//socket 선언
	SOCKET Server_Socket; 
	// declare structure of socket address information.
	SOCKADDR_IN Server_Address; 
	// 접속할 포트 넘버 지정. 2000 ~ 65535까지 임의적으로 설정
	unsigned short Server_Port = 6801;

	//Windows socket start 시작전, socket DLL files 초기화
	if(WSAStartup(0x202,&winsock_data) == SOCKET_ERROR){ 
		printf("WSAStartup Setting Error.\n");
		// Close initialization failure.
		WSACleanup();
		exit(0); 
	}
	
	// TCP를 사용하므로 AF_INET 사용
	Server_Address.sin_family = AF_INET;

	// IP address and connect port number
	// Test IP address : 127.0.0.1 (My computer internal IP address)
	Server_Address.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP address
	Server_Address.sin_port = htons(Server_Port);				// Port Number

	// socket 생성
	Server_Socket = socket(AF_INET, SOCK_STREAM,0);

	// socket 생성 error시
	if(Server_Socket == INVALID_SOCKET){
		printf("Cannot create socket."); 
		closesocket( Server_Socket ); 
		WSACleanup(); 
		exit(0); 
	}

	if(bind(Server_Socket,(struct sockaddr*)&Server_Address,sizeof(Server_Address) ) == SOCKET_ERROR){ 
		printf("Cannot Bind."); 
		closesocket( Server_Socket );
		WSACleanup(); 
		exit(0); 
	} 

	printf(" >>  Server starting...  << \n\n"); 
	int count=1;

	// Loop
	while(1){

		if(listen(Server_Socket,SOMAXCONN) == SOCKET_ERROR){ 
			printf("listen Error.\n"); 
			closesocket(Server_Socket); 
			WSACleanup(); 
			exit(0); 
		}

		// Client Socket 선언
		SOCKET Client_Socket;
		// Socket Address Structure declaration
		SOCKADDR_IN Client_Address;
		// Address size
		int AddressSize = sizeof(Client_Address);

		printf(" --------------------------------------------------------\n");
		// Connection Counting
		printf(" => Wait for Client(%d times).\n",count++); 

		// compare to Client Data and Server Data.
		if((Client_Socket = accept( Server_Socket,(struct sockaddr*)&Client_Address , &AddressSize )) == INVALID_SOCKET){ 
			printf("Accept Error.\n"); 
			getchar(); 
		} 
		else{ 
			printf(" => Connect IP: %s, Port : %d  Connect complete!\n\n", inet_ntoa(Client_Address.sin_addr), htons(Client_Address.sin_port)); 
		}
        int SendSize = MAX_SIZE;
		char x[MAX_SIZE]; //client로 부터 전송 받은 첫번째 변수
		char Buffer_1[999]; //client로 부터 첫번째로 전송받은 값. x값이 들어있습니다.
		recv(Client_Socket,x,SendSize, 0); //x값을 전송받습니다.
		printf("입력받은 x값: %s \n", x); //x값을 출력
        	send( Client_Socket,x, SendSize, 0 ); //결과값의 char형을 전송합니다.	     
      
	}
	// Wait : Server state is listen.
	getchar(); 
} 
