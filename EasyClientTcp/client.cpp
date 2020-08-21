#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <winSock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
int main() {
	//启动socket网络服务
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	SOCKET req_sock = socket(AF_INET, SOCK_STREAM, 0);  // socket()
	if (INVALID_SOCKET == req_sock) {
		cout << "error, create socket failture" << endl;
	}
	else {
		cout << "create socket successfully" << endl;
	}
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(req_sock,(sockaddr*)&_sin, sizeof(sockaddr_in));  // connect()
	if (ret == SOCKET_ERROR) {
		cout << "error, connect to server failture" << endl;
	}
	else {
		cout << "connect to server successfully" << endl;
	}

	while (1) {
		char msgbuf[128] = {};
		printf("please input your command:");
		scanf("%s", msgbuf);
		if (0 == strcmp(msgbuf, "exit")) {
			printf("get exit command\n");
			break;
		}
		//printf("====get input is %s \n", msgbuf);
		// send command to server
		send(req_sock, msgbuf, strlen(msgbuf)+1, 0);
		
		char recvbuf[256] = {};
		int num = recv(req_sock, recvbuf, 256, 0);   // recv()
		if (num > 0) {
			printf("get data from server is :%s\n", recvbuf);
		}
	}
	closesocket(req_sock);
	// 关闭
	WSACleanup();
	printf("client exit...\n");
	getchar();
	return 0;
}