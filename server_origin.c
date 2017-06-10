#include <stdio.h>	// ǥ�� �����
#include <stdlib.h>	// ǥ�� ���̺귯��
#include <unistd.h>	// ���н� ǥ��
#include <string.h>	// ���ڿ� ó��
#include <arpa/inet.h>	// ���ͳ� ��������
#include <sys/socket.h>	// �����Լ�
#include <netinet/in.h>	// ���ͳ� �ּ� ü�� (ex. in_port_t)
#include <pthread.h>

#define BUF_SIZE 256	// �j���� �� ��tl�� �ִ� ����
#define MAX_CLNT 256	// �ִ� ���� ������ ��

void * handle_clnt(void * arg);	// Ŭ���̾�Ʈ ������� �Լ�(�Լ� ������)
void send_msg(char * msg, int len);// �޽��� ���� �Լ�
void error_handling(char * msg);	// ���� ó�� �Լ�

int clnt_cnt=0;	// ���� �������� ����� ��
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;	// ��ȣ ������ ���� 

int main(int argc, char *argv[])		// ���ڷ� ��Ʈ��ȣ ����
{
	int serv_sock, clnt_sock;		// ���� ��ſ� ���� ���ϰ� �ӽ� Ŭ���̾�Ʈ ����
	struct sockaddr_in serv_adr, clnt_adr;	// ���� �ּ�, Ŭ���̾�Ʈ �ּ� ����ü
	int clnt_adr_sz;			// Ŭ���̾�Ʈ �ּ� ����ü
	pthread_t t_id;			// Ŭ���̾�Ʈ ������� ID
	// ��Ʈ �Է� ��������
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);	// ������ �˷��ش�.
		exit(1);						// ���α׷� ������ ����
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

// ������� �Լ�
void * handle_clnt(void * arg)	// ������ ��� Ŭ���̾�Ʈ�� ����ϴ� �Լ�
{
	int clnt_sock=*((int*)arg);	// �����尡 ����� Ŭ���̾�Ʈ ���� ����
	int str_len=0, i;
	char msg[BUF_SIZE];	// �޽��� ����
	
	// Ŭ���̾�Ʈ�� ����� ���� �ʴ��� ��� ���񽺸� �����Ѵ�.
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
		send_msg(msg, str_len);
	
	pthread_mutex_lock(&mutx); // �Ӱ� ���� ����
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
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
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}

// ���� ó�� �Լ�
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
