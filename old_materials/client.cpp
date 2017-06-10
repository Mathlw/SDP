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
 
#define MAX_SIZE 64			// �ѹ��� ���� ������ ũ�⸦ �ִ� 64(Bytes)�� ����

void main(){ 
	//Winsock_data ����(WSADATA type structure)
	WSADATA winsock_data;
	//socket ����
	SOCKET Client_Socket; 
	// declare structure of socket address information.
	SOCKADDR_IN Server_Address; 
	// ������ ��Ʈ �ѹ� ����. 2000 ~ 65535���� ���������� ����
	unsigned short Server_Port = 6801;
	int ReturnVal; 

	//Windows socket start ������, socket DLL files �ʱ�ȭ
	if(WSAStartup(0x202,&winsock_data) == SOCKET_ERROR){
		printf( "WSAStartup Setting Error.\n" );
		// Close initialization failure.
		WSACleanup();
		exit(0);
	}

	int count=1;	// ���Ӽ��� count��
	
	// send message ���� ���� (Max.Size = MAX_SIZE)
	char SendMessage[MAX_SIZE];
	char Tmp[MAX_SIZE]="Off"; // Compare to the Client end.
	

	while(strcmp(SendMessage,Tmp) != 0){
	
		// socket ����
		Client_Socket = socket(AF_INET, SOCK_STREAM,0);

		// socket ���� error��
		if( Client_Socket == INVALID_SOCKET ){ 
			printf("Create Socket Error.");
			WSACleanup(); 
			exit(0);
		}

		// TCP�� ����ϹǷ� AF_INET ���
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
		char x[MAX_SIZE]; //�Է��� x��
		printf("x���� �Է��ϼ���. "); 
		scanf("%s", &x); //x���� �Է¹޽��ϴ�.
		send(Client_Socket, x, SendSize, 0); //char������ ��ȯ�� x���� �����մϴ�.
		printf("����� : %s \n", x); //result ���� ����մϴ�.
	}//while end.

	closesocket(Client_Socket);
	WSACleanup();
	
	// Wait
	getchar();
}


