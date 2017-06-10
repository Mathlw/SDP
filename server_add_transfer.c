#include <stdio.h>	// 표준 입출력 
#include <stdlib.h>	// 표준 라이브러리 
#include <unistd.h>	// 유닉스 표준 
#include <string.h>	// 문자열 처리 
#include <arpa/inet.h>	// 인터넷 프로토콜 
#include <sys/socket.h>	// 소켓함수 
#include <netinet/in.h>	// 인터넷 주소 체계 (ex. in_port_t) 
#include <pthread.h> 

 
#define BUF_SIZE 256	// j팅할 ?? 메tl지 최대 길이 
#define MAX_CLNT 256	// 최대 동시 접속자 수 
 
 
void * handle_clnt(void * arg);	// 클라이언트 쓰레드용 함수(함수 포인터) 
void send_msg(char * msg, int len);// 메시지 전달 함수 
void error_handling(char * msg);	// 예외 처리 함수 
16 
 
int clnt_cnt=0;	
int clnt_socks[MAX_CLNT]; 

char clnt_name[NAME_SIZE]= {NULL}; // 접속자 이름
char clnt_names[MAX_CLNT][NAME_SIZE]= {NULL};


pthread_mutex_t mutx; //상호 배제를 위한

int main(int argc, char *argv[])		//인자로 포트번호 받음
{
	int serv_sock, clnt_sock, i;		// 소켓 통신용 서버 소켓과 임시 클라이언트 소켓 
	struct sockaddr_in serv_adr, clnt_adr;	// 서버 주소, 클라이언트 주소 구조체 
	int clnt_adr_sz;			// 클라이언트 주소 구조체 
	pthread_t t_id;				// 클라이언트 쓰레드용 ID 
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);	// 사용법을 알려준다.
		exit(1);			// 프로그램 비정상 종료 
	}
  
	pthread_mutex_init(&mutx, NULL);	// 커널에서 Mutex 쓰기위해 얻어 온다. 
 	serv_sock=socket(PF_INET, SOCK_STREAM, 0);	// TCP용 소켓 생성 
 
 
 	memset(&serv_adr, 0, sizeof(serv_adr));	// 서버 주소 구조체 초기화 
 	serv_adr.sin_family=AF_INET; 		// 인터넷 통신 
 	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);	// 현재 IP를 이용하고 
 	serv_adr.sin_port=htons(atoi(argv[1])); // 포트는 사용자가 지정한 포트 사용 
 	 
 	//서버 소켓에 주소를 할당한다. 
 	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1) 
 		error_handling("bind() error"); 
 
 
 	// 서버 소켓을 서버로 사용 
 	if(listen(serv_sock, 5)==-1) 
 		error_handling("listen() error"); 
 	 
 	while(1)	// 무한루프 돌면서 
 	{ 
		clnt_adr_sz=sizeof(clnt_adr);	// 클라이언트 구조체의 크기를 얻고 
 		// 클라이언트의 접속을 받아들이기 위해 Block 된다.(멈춘다) 
 		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); 
 		pthread_mutex_lock(&mutx);		 
		clnt_socks[clnt_cnt++]=clnt_sock; 
		pthread_mutex_unlock(&mutx); 
 
 
		// 클라이언트 구조체의 주소를 쓰레드에 넘긴다.(포트 포함됨) 
 		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);	// 쓰레드 시작 
		pthread_detach(t_id);	// 쓰레드가 종료되면 스스로 소멸되게 함 
		// 접속된 클라이언트 IP를 화면에 찍어준다. 
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr)); 
	} 
	close(serv_sock);	// 쓰레드가 끝나면 소켓을 닫아준다. 
	return 0; 
}	

// 쓰레드용 함스
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg); //쓰레드가 통신할 클라이언트 소켓 변수
	int str_len=0, i;
	int fSize = 0;
	const char sig_file[BUF_SIZE] = {"file : cl->sr"}; 
	const char Fmsg_end[BUF_SIZE] = {"FileEnd_cl->sr"}; 
	const char sig_msg[BUF_SIZE] = {"Msg : cl->sr"}; 
	char msg[BUF_SIZE] = {NULL}; //메세지 버퍼
	char file_msg[BUF_SIZE] = {NULL};
	//클라이언트가 통신을 끊지 않는 한 계속 서비스를 제공한다
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
					fileGo = j; // 보낼 소켓 번호를 저장
					break;
				}
				else if(j == clnt_cnt - 1) {
					noCli = 1; // 그런 사용자가 없을 때 표시
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
			// 해당 사용자가 존재하는지 찾기



			write(clnt_socks[fileGo], "file : sr->cl", BUF_SIZE);
			// 데이터를 보낸다는 신호를 먼저 보냄

			read(clnt_sock, &fSize, sizeof(int));
			printf("File size %d Byte\n", fSize);
			write(clnt_socks[fileGo], &fSize, sizeof(int));
			// 파일 크기 정보를 보냄.(진행중)

			while(1) {
				read(clnt_sock, file_msg, BUF_SIZE);
				if(!strcmp(file_msg, Fmsg_end))
					break;
				write(clnt_socks[fileGo], file_msg, BUF_SIZE);
			}


			write(clnt_socks[fileGo], "FileEnd_sr->cl", BUF_SIZE);
			
			pthread_mutex_unlock(&mutx);
			printf("(!Notice)File data transfered \n");

		} // 파일정송
		
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
					mGo = j; // 보낼 소켓 번호를 저장
					break;
				}
				else if(j == clnt_cnt - 1) {
					noCli = 1; // 그런 사용자가 없을 때 표시
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
		}// 메시지 전송
	}

	
	pthread_mutex_lock(&mutx); // 임계 영역 시작
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
 	pthread_mutex_unlock(&mutx); // 임계 영역 끝 
 	close(clnt_sock);	// 소켓을 닫고 
 	return NULL;		// 서비스 종료 
}
void send_msg(char * msg, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, BUF_SIZE);
	pthread_mutex_unlock(&mutx);
}
//예외 처리 함수
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}