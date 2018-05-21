#include "socket.h"

void init_server(Socket_value *socket_info){
	/* server socket 초기화 & 연결 client로부터 대기*/
	
	// 소켓 생성
	if((socket_info->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Server : Can't open stream socket\n");
		exit(0);
	}
	
	//server_Addr 을 NULL로 초기화
	memset(&socket_info->server_addr, 0x00, sizeof(socket_info->server_addr));
	
	//server_addr 셋팅		 
	socket_info->server_addr.sin_family = AF_INET;
	socket_info->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socket_info->server_addr.sin_port = htons(3086);
	socket_info->addr_len = 0;
	
	//bind() 호출
	if(bind(socket_info->server_fd, (struct sockaddr *)&socket_info->server_addr, sizeof(socket_info->server_addr)) <0)
    {
        printf("Server : Can't bind local address.\n");
        exit(0);
    }

	//소켓을 수동 대기모드로 설정
	if(listen(socket_info->server_fd, 5) < 0)
	{
		printf("Server : Can't listening connect.\n");
		exit(0);
	}

	printf("Server : wating connection request.\n");
}

int connect_server(Socket_value *socket_info) {
	socket_info->client_fd = accept(socket_info->server_fd, (struct sockaddr *)&socket_info->client_addr, &socket_info->addr_len);
	if(socket_info->client_fd < 0) {
		printf("Server: accept failed.\n");
		return -1;
	}
	
	return 0;
}

int connect_client(Socket_value *socket_info, char *severIP){
	if((socket_info->client_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("can't create socket\n");
		exit(0);
	}
	
	socket_info->server_addr.sin_family = AF_INET;
	socket_info->server_addr.sin_addr.s_addr = inet_addr(severIP);
	socket_info->server_addr.sin_port = htons(3086);
    
	//서버로 연결요청
	if(connect(socket_info->client_fd, (struct sockaddr *)&socket_info->server_addr, sizeof(socket_info->server_addr)) < 0)
    {
		printf("can't connect.\n");
		return -1;
	}

	printf("Client : wating connection request.\n");
	return 1;
}
