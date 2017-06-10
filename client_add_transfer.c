#include <stdio.h>	// ǥ�� �����
#include <stdlib.h>	// ǥ�� ���̺귯��
#include <unistd.h>	// ���н� ǥ��
#include <string.h>	// ���ڿ� ó��
#include <arpa/inet.h>	// ���ͳ� ��������
#include <sys/socket.h>	// �����Լ�
#include <pthread.h>
	
#define BUF_SIZE 100		// �޽��� ���� ����
#define NAME_SIZE 20		// �̸��� ����
#define NOTSET 0
#define EXIST 1
#define NOTEXIST 2
	
void * send_msg(void * arg);	// ������ �Լ� - �۽ſ�
void * recv_msg(void * arg);	// ������ �Լ� - ���ſ�
void error_handling(char * msg);	// ����ó�� �Լ�
	
char name[NAME_SIZE]="[DEFAULT]";	// �� �̸� - �ʱⰪ�� "[DEFAULT]"
char msg[BUF_SIZE];			// �޽��� ����

int cli_exist = NOTSET;

// ���ڷ� IP�� port �� �޴´�
int main(int argc, char *argv[])
{
	int sock;				// ��ſ� ���� ���� ��ũ����
	struct sockaddr_in serv_addr;	// ���� �ּ� ����ü ����
	pthread_t snd_thread, rcv_thread;	// �۽� ������, ���� ������
	void * thread_return;			// ������ ��ȯ ���ΰ�
	if(argc!=4)		// ����ڰ� ���α׷� �� �� ���۽�������
	{	
		printf("Usage : %s <IP> <port> <name>\n", argv[0]); // ���� �ȳ�
		exit(1);						// ���α׷� ������ ����
	 }
	
	sprintf(name, "[%s]", argv[3]);			// ������� �̸�
	sock=socket(PF_INET, SOCK_STREAM, 0);	// TCP ��� ���� ����
	
	memset(&serv_addr, 0, sizeof(serv_addr));	// ������ ���� �ּ� ����
	serv_addr.sin_family=AF_INET;			// ���ͳ� ���
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);	
	serv_addr.sin_port=htons(atoi(argv[2]));	// �򿣵���� ��Ʋ���������
	 
	// ������ ���� �õ�
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");

	printf("\n\n");
	printf("filetransfer is /sendfile \n\n"); //�������� ��ɾ� �ȳ�
	
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);	// �۽� ������ ����
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);	//  ���� ������ ����
	pthread_join(snd_thread, &thread_return);	// ���� �Լ��� �ƹ��͵� ���ϰ� ��ٸ���.
	pthread_join(rcv_thread, &thread_return);	// ����� 2���� �����尡 ó���Ѵ�.
	close(sock);  		// ���α׷� ������ �� ���� �Ҹ� ��Ű�� ����
	return 0;
}
	
void * send_msg(void * arg)   // send thread main
{
	int sock=*((int*)arg);		// ������ �޴´�.
	int Flength = 0;
	int i=0;
	int fSize = 0;
	int fEnd = 0;

	char name_msg[NAME_SIZE+BUF_SIZE];
	char t_msg[BUF_SIZE] = {NULL};	
	char t_name_msg[BUF_SIZE] = {NULL};
	char last_msg[BUF_SIZE] = {NULL};
	char noUse[BUF_SIZE] = {NULL};

	while(1) 		// ���� ���� ���鼭
	{
		fgets(msg, BUF_SIZE, stdin);		// Ű���� �Է��� �޾Ƽ�
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")) 	// q�� Q�� �����ϰ�
		{
			close(sock);
			exit(0);
		}
		// ���� ���� �Է��� �ϸ�

		else if(!strcmp(msg, "/sendfile\n")) // /sendfile�� �ԷµǸ�
		{
			char location[BUF_SIZE];
			char who[NAME_SIZE];
			FILE *fp;
			FILE *size;

			printf("(!Record)File location : ");
			scanf("%s", location);

			size = fopen(location, "rb");
			if(size == NULL) {
				printf("(!Notice)No file like that \n");
				continue;
			}
			// �� ���� ������ ��ȿ�Ѱ� Ȯ��

			printf("(!Record)To who(ID)? : ");
			scanf("%s", who);

			write(sock, "file : cl->sr", BUF_SIZE);
			// �� ���� ������ �����ٴ� ��ȣ�� �����ʿ� ����.

			write(sock, who, NAME_SIZE);
			// �� ��������� ����� ���̵� ����.

			while(cli_exist == NOTSET) {
				sleep(1);
			}

			if(cli_exist == NOTEXIST) {
				printf("(!Notice)No user like that \n");
				cli_exist = NOTSET;
				continue;
			} 
			// �� Ŭ���̾�Ʈ�� ������ ��������

			while(1) {	
				fEnd = fread(noUse, 1 , BUF_SIZE, size);
				fSize += fEnd;

				if(fEnd != BUF_SIZE)
					break;
			}
			fclose(size);

			printf("(!Notice)File transfer start \n(File Size : %d Byte)\n", fSize); 
			write(sock, &fSize, sizeof(int)); // ���� ũ������ ���� ����.
			fSize = 0;
			
			fp = fopen(location, "rb");
			

			while(1) {
				
				Flength = fread(t_msg, 1 , BUF_SIZE, fp);

				if(Flength != BUF_SIZE) {
					for(i=0; i<Flength; i++) {
						last_msg[i] = t_msg[i];
					} 
					//�� fread �� ���ϳ��� ����� �� ������ �����Ϳ� �������� ���� �� �����Ƿ� �����Ͽ���.

					write(sock, last_msg, BUF_SIZE);

					write(sock, "FileEnd_cl->sr", BUF_SIZE);
					break;
				}
				write(sock, t_msg, BUF_SIZE); 

			}
			// �� ������ ������ ������ �����ϴ�.		

			fclose(fp);
			printf("(!Notice)File transfer finish \n");
			cli_exist = NOTSET;
			
		}

		else 
		{
			strcpy(t_msg, "\n");
			sprintf(t_name_msg,"[%s] %s", name, t_msg);
			sprintf(name_msg,"[%s] %s", name, msg);

			if(strcmp(name_msg, t_name_msg) != 0) 
				write(sock, name_msg, BUF_SIZE);
			// �� �ƹ��͵� �Է¹��� �ʾ������� ������� ����
			// �� �޽��� ������
			
		}
	}
	return NULL;
}

// ������ ������ �Լ�
void * recv_msg(void * arg)   // read thread main
{
	int sock=*((int*)arg);		// ��ſ� ������ �ް�
	char name_msg[NAME_SIZE+BUF_SIZE];	// �̸� + �޽��� ����
	char file_msg[BUF_SIZE] = {NULL};       // ���� ����
	int str_len;				// ���ڿ� ����	
	while(1)				// ���� ���� ���鼭
	{
		// �޽����� ������
		str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
		if(str_len==-1) 		// ���� ����� ��������
			return (void*)-1;	// ������ ����
		name_msg[str_len]=0;	// �޽������� ����
		fputs(name_msg, stdout);	// ȭ�鿡 ���ŵ� �޽��� ǥ��
	}
	return NULL;
}

// ���� ó�� �Լ�
void error_handling(char *msg)
{
	fputs(msg, stderr);	// ���� �޽����� ǥ���ϰ�
	fputc('\n', stderr);
	exit(1);			// ������ ���� ó��
}
