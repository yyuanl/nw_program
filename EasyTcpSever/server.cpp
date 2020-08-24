
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#include <winSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif


#include "EasyTcpServer.hpp"
#include <vector>
#include <iostream>
using namespace std;
int main() {
	EasyTcpServer server;
	SOCKET lis_fd = server.openListenfd();
	if (lis_fd == SOCKET_ERROR) {
		std::wcout << "socket error ,server exit..." << std::endl;
		exit(0);
	}
	while (1) {
		server.runWithSelect();
	}

	std::cout << "server exit" << endl;
	getchar();
	return 0;
}