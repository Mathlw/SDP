#include <stdio.h>	// ǥ�� ����� 
#include <stdlib.h>	// ǥ�� ���̺귯�� 
#include <unistd.h>	// ���н� ǥ�� 
#include <string.h>	// ���ڿ� ó�� 
#include <arpa/inet.h>	// ���ͳ� �������� 
#include <sys/socket.h>	// �����Լ� 
#include <netinet/in.h>	// ���ͳ� �ּ� ü�� (ex. in_port_t) 
#include <pthread.h> 

 
#define BUF_SIZE 256	// j���� ?? ��tl�� �ִ� ���� 
#define MAX_CLNT 256	// �ִ� ���� ������ �� 
 
 
void * handle_clnt(void * arg);	// Ŭ���̾�Ʈ ������� �Լ�(�Լ� ������) 
void send_msg(char * msg, int len);// �޽��� ���� �Լ� 
void error_handling(char * msg);	// ���� ó�� �Լ� 
16 
 
int clnt_cnt=0;	
int clnt_socks[MAX_CLNT]; 

char clnt_name[NAME_SIZE]= {NULL}; // ������ �̸�
char clnt_names[MAX_CLNT][NAME_SIZE]= {NULL};


pthread_mutex_t mutx; //��ȣ ������ ����

int main(int argc, char *argv[])		//���ڷ� ��Ʈ��ȣ ����
{
	int serv_sock, clnt_sock, i;		// ���� ��ſ� ���� ���ϰ� �ӽ� Ŭ���̾�Ʈ ���� 
	struct sockaddr_in serv_adr, clnt_adr;	// ���� �ּ�, Ŭ���̾�Ʈ �ּ� ����ü 
	int clnt_adr_sz;			// Ŭ���̾�Ʈ �ּ� ����ü 
	pthread_t t_id;				// Ŭ���̾�Ʈ ������� ID 
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);	// ������ �˷��ش�.
		exit(1);			// ���α׷� ������ ���� 
	}
  
	pthread_mutex_init(&mutx, NULL);	// Ŀ�ο��� Mutex �������� ��� �´�. 
 	serv_sock=socket(PF_INET, SOCK_STREAM, 0);	// TCP�� ���� ���� 
 
 
 	memset(&serv_adr, 0, sizeof(serv_adr));	// ���� �ּ� ����ü �ʱ�ȭ 
 	serv_adr.sin_family=AF_INET; 		// ���ͳ� ��� 
 	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);	// ���� IP�� �̿��ϰ� 
 	serv_adr.sin_port=htons(atoi(argv[1])); // ��Ʈ�� ����ڰ� ������ ��Ʈ ��� 
 	 
 	//���� ���Ͽ� �ּҸ� �Ҵ��Ѵ�. 
 	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1) 
 		error_handling("bind() error"); 
 
 
 	// ���� ������ ������ ��� 
 	if(listen(serv_sock, 5)==-1) 
 		error_handling("listen() error"); 
 	 
 	while(1)	// ���ѷ��� ���鼭 
 	{ 
		clnt_adr_sz=sizeof(clnt_adr);	// Ŭ���̾�Ʈ ����ü�� ũ�⸦ ��� 
 		// Ŭ���̾�Ʈ�� ������ �޾Ƶ��̱� ���� Block �ȴ�.(�����) 
 		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); 
 		pthread_mutex_lock(&mutx);		 
		clnt_socks[clnt_cnt++]=clnt_sock; 
		pthread_mutex_unlock(&mutx); 
 
 
		// Ŭ���̾�Ʈ ����ü�� �ּҸ� �����忡 �ѱ��.(��Ʈ ���Ե�) 
 		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);	// ������ ���� 
		pthread_detach(t_id);	// �����尡 ����Ǹ� ������ �Ҹ�ǰ� �� 
		// ���ӵ� Ŭ���̾�Ʈ IP�� ȭ�鿡 ����ش�. 
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr)); 
	} 
	close(serv_sock);	// �����尡 ������ ������ �ݾ��ش�. 
	return 0; 
}	

// ������� �Խ�
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg); //�����尡 ����� Ŭ���̾�Ʈ ���� ����
	int str_len=0, i;
	int fSize = 0;
	const char sig_file[BUF_SIZE] = {"file : cl->sr"}; 
	const char Fmsg_end[BUF_SIZE] = {"FileEnd_cl->sr"}; 
	const char sig_msg[BUF_SIZE] = {"Msg : cl->sr"}; 
	char msg[BUF_SIZE] = {NULL}; //�޼��� ����
	char file_msg[BUF_SIZE] = {NULL};
	//Ŭ���̾�Ʈ�� ����� ���� �ʴ� �� ��� ���񽺸� �����Ѵ�
	while((str_len=read(clnt_sock, msg, BUF_SIZE))!=0) 
	{

		if(!strcmp(msg, sig_file))
		{
			int j;
			int noCli = 0;
			int fileGo = NULL;
			char tmpName[NAME_SIZE]= {NULL};

			read(clnt_sock, tmpName, NAME_SIZE);
			

			pthread_mutex_lock(&mutx);
						
			for(j=0; j<clnt_cnt; j++) {

				
				if(!strcmp(tmpName, clnt_names[j]) ) {
					noCli = 0;
					fileGo = j; // ���� ���� ��ȣ�� ����
					break;
				}
				else if(j == clnt_cnt - 1) {
					noCli = 1; // �׷� ����ڰ� ���� �� ǥ��
					break;
				}
			}

			if(noCli == 1) {

				write(clnt_sock, "[NoClient_sorry]", BUF_SIZE);
				pthread_mutex_unlock(&mutx);
				continue;
			}
			else if(noCli == 0) {

				write(clnt_sock, "[continue_ok_nowgo]", BUF_SIZE);
				
			}
			// �ش� ����ڰ� �����ϴ��� ã��



			write(clnt_socks[fileGo], "file : sr->cl", BUF_SIZE);
			// �����͸� �����ٴ� ��ȣ�� ���� ����

			read(clnt_sock, &fSize, sizeof(int));
			printf("File size %d Byte\n", fSize);
			write(clnt_socks[fileGo], &fSize, sizeof(int));
			// ���� ũ�� ������ ����.(������)

			while(1) {
				read(clnt_sock, file_msg, BUF_SIZE);
				if(!strcmp(file_msg, Fmsg_end))
					break;
				write(clnt_socks[fileGo], file_msg, BUF_SIZE);
			}


			write(clnt_socks[fileGo], "FileEnd_sr->cl", BUF_SIZE);
			
			pthread_mutex_unlock(&mutx);
			printf("(!Notice)File data transfered \n");

		} // ��������
		
		else if(!strcmp(msg, sig_msg)) {
			int j=0;
			int noCli = 0;
			int mGo = 0;
			char tmpName[NAME_SIZE]= {NULL};
			char msg[NAME_SIZE]= {NULL};

			read(clnt_sock, tmpName, NAME_SIZE);
			
			pthread_mutex_lock(&mutx);
			for(j=0; j<clnt_cnt; j++) {
				if(!strcmp(tmpName, clnt_names[j]) ) {
					noCli = 0;
					mGo = j; // ���� ���� ��ȣ�� ����
					break;
				}
				else if(j == clnt_cnt - 1) {
					noCli = 1; // �׷� ����ڰ� ���� �� ǥ��
					break;
				}
			}
			pthread_mutex_unlock(&mutx);

			read(clnt_sock, msg, BUF_SIZE);


		}
		else
		{
			printf("(!Notice)Chatting message transfered \n");
			send_msg(msg, str_len);
		}// �޽��� ����
	}

	
	pthread_mutex_lock(&mutx); // �Ӱ� ���� ����
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1) {
				clnt_socks[i]=clnt_socks[i+1];
				strcpy(clnt_names[i], clnt_names[i+1]);
			}
			break;
		}
	}
	clnt_cnt--; 
 	pthread_mutex_unlock(&mutx); // �Ӱ� ���� �� 
 	close(clnt_sock);	// ������ �ݰ� 
 	return NULL;		// ���� ���� 
}
void send_msg(char * msg, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, BUF_SIZE);
	pthread_mutex_unlock(&mutx);
}
//���� ó�� �Լ�
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}