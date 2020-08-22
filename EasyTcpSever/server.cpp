#define WIN32_LEAN_AND_MEAN
#include <ws2tcpip.h>
#include <iostream>
#include <windows.h>
#include <winSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <vector>
#define BUFSIZE 128
using namespace std;
enum STATE_RECV_SEND
{
	RECV_ABNORMAL,
	SEND_ABNORMAL,
	NORMAL
};
STATE_RECV_SEND recv_send(SOCKET conn_fd, char *recvbuf, int buf_len) {
	
	int num = recv(conn_fd, recvbuf, buf_len, 0);
	if (num <= 0) {
		printf("client exit or recv error...\n");
		return RECV_ABNORMAL;
	}
	printf("get client command : %s\n", recvbuf);
	//处理请求
	if (0 == strcmp(recvbuf, "getName")) {
		//5. 发送给客户端数据
		char msgbuf[] = "LiMing";
		if (send(conn_fd, msgbuf, strlen(msgbuf) + 1, 0) < 0) {
			return SEND_ABNORMAL;
		}
	}
	else if (0 == strcmp(recvbuf, "getAge")) {
		//5. 发送给客户端数据
		char msgbuf[] = "18";
		if (send(conn_fd, msgbuf, strlen(msgbuf) + 1, 0) < 0) {
			return SEND_ABNORMAL;
		}
		
	}
	else {
		char msgbuf[] = "what dou you want get ?";
		if (send(conn_fd, msgbuf, strlen(msgbuf) + 1, 0) < 0) {
			return SEND_ABNORMAL;
		}
	}
	return NORMAL;
}

void check_client_pool(vector<SOCKET> &pool, fd_set *ready_set, fd_set *read_set) {
	char recvbuf[BUFSIZE];
	for (auto iter = pool.begin(); iter != pool.end();) {
		if (FD_ISSET(*iter, ready_set) && *iter > 0) {
			STATE_RECV_SEND res = recv_send(*iter, recvbuf, BUFSIZE);
			if (res != NORMAL ) {
				closesocket(*iter);
				FD_CLR(*iter, read_set);
				iter = pool.erase(iter);
			}
			else {
				iter++;
			}
		}
		else {
			iter++;
		}
	}
}

vector<SOCKET>client_pool;
int main() {
	//启动socket网络服务
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//1.建立socket
	SOCKET listen_fd =  socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	//2. bind 绑定网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); // 主机到 网络字节顺序
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.1.112");

	//_sin.sin_addr.S_un.S_addr = inet_pton("127.0.0.1");
	//inet_pton();
	if (bind(listen_fd, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		printf("bind socket port failture\n");
	}
	else {
		printf("bind socket port successfully\n");
	}
	//3.监听网络端口
	if (SOCKET_ERROR == listen(listen_fd, 5)) {
		std::cout << "listen socket failture" << endl;
	}
	else {
		std::cout << "listen socket successfully" << endl;
	}
	char recvbuf[BUFSIZE];
	fd_set ready_set;  // 就绪描述符
	fd_set read_set;  // select 第二个参数 读集合
	FD_ZERO(&read_set);
	FD_SET(listen_fd, &read_set);
	SOCKET max_fd = listen_fd;
	while (1) {
		ready_set = read_set;
		int res = select(listen_fd +1,&ready_set, NULL, NULL, NULL);
		if (res < 0) {
			printf("select failture\n");
			break;
		}
		if (FD_ISSET(listen_fd, &ready_set)) { // listen_fd检测到新连接到来
			// 4. 等待接收客户端连接请求
			sockaddr_in clientAddr = {};
			int lenAddr = sizeof(sockaddr_in);
			SOCKET conn_fd = INVALID_SOCKET;
			conn_fd = accept(listen_fd, (sockaddr*)&clientAddr, &lenAddr);
			if (conn_fd == INVALID_SOCKET) {
				std::cout << "invalid socket connection..." << endl;
			}
			std::cout << "one client come, it IP = " << inet_ntoa(clientAddr.sin_addr) <<" socket fd is "<<(int)conn_fd<<endl;
			client_pool.push_back(conn_fd);  //新的客户端连接放入容器
			FD_SET(conn_fd, &read_set);  // 将新连接的socket fd 加入slect描述符集合
		}
		check_client_pool(client_pool, &ready_set, &read_set);
	}
	//6.关闭监听socket
	closesocket(listen_fd);
	// 关闭
	WSACleanup();
	std::cout << "server exit" << endl;
	getchar();
	return 0;
}