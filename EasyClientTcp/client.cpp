#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <winSock2.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")
constexpr auto MSGBUFLEN = 128;
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
	fd_set read_set;
	fd_set write_set;
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_SET(req_sock, &read_set);
	FD_SET(req_sock, &write_set);
	while (1) {
		FD_SET(req_sock, &read_set);
		FD_SET(req_sock, &write_set);
		int res = select(req_sock + 1, &read_set, &write_set, NULL, NULL);
		if (res < 0) {
			printf("select failture");
			break;
		}
		if (FD_ISSET(req_sock, &read_set)) {
			char recvbuf[256] = {};
			int num = recv(req_sock, recvbuf, 256, 0);   // recv()
			if (num > 0) {
				printf("get data from server is :%s\n", recvbuf);
			}
			else {
				printf("server exit...\n");
				break;
			}
		}

		if (FD_ISSET(req_sock, &write_set)) {
			int flag = rand() % 3;
			char msgbuf1[MSGBUFLEN] = "getName";
			char msgbuf2[MSGBUFLEN] = "getAge";
			char msgbuf3[MSGBUFLEN] = "otherCommand";
			const char *p = NULL;
			switch (flag)
			{
			case 0: p = msgbuf1; break;
			case 1: p = msgbuf2; break;
			case 2: p = msgbuf3; break;
			default:
				break;
			}
			send(req_sock, p, MSGBUFLEN, 0);
			Sleep(1000);
		}

		/*char msgbuf[128] = {};
		printf("please input your command:");
		scanf("%s", msgbuf);
		if (0 == strcmp(msgbuf, "exit")) {
			printf("get exit command\n");
			break;
		}*/
		//printf("====get input is %s \n", msgbuf);
		// send command to server
		
		
	}
	closesocket(req_sock);
	// 关闭
	WSACleanup();
	printf("client exit...\n");
	getchar();
	return 0;
}