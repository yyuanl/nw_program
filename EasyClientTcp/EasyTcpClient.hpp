#ifndef __EASY_TCP_CLIENT__
#define __EASY_TCP_CLIENT__
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
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
#include <iostream>
class EasyClientTcp
{
public:
	EasyClientTcp() :
		_sock(INVALID_SOCKET)
	{
		//initSocket();
	}
	virtual~EasyClientTcp() {
		closeSocket();
	}
	int initSocket();
	int connect2Server(const char *ip, unsigned short port);
	void closeSocket();
	bool runWithSelect();
	SOCKET getSocket() { return _sock; }

private:
	SOCKET _sock;
	/*char *_ip;
	unsigned short _port;*/
	int receiveData();
	bool processData(); //处理数据接口
	bool sendData(); //发送数据接口
	bool isValidSocket() { return _sock != INVALID_SOCKET; }
};


int EasyClientTcp::initSocket() {
	//windows启动socket网络服务
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
#endif
	if (_sock != INVALID_SOCKET) {  // 如果sock已经被创建过，应该先关闭
		std::cout << "close old socket " << (int)_sock << " connection" << std::endl;
		closeSocket();
	}
	_sock = socket(AF_INET, SOCK_STREAM, 0);  // socket()
	if (INVALID_SOCKET == _sock) {
		std::cout << "error, create socket failture" << std::endl;
	}
	else {
		std::cout << "create socket successfully" << std::endl;
	}
	return 1;
}
int EasyClientTcp::connect2Server(const char *ip, unsigned short port) {
	if (_sock == INVALID_SOCKET) {
		std::cout << "init valid socket connection" << std::endl;
		initSocket();
	}
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.s_addr = inet_addr(ip);
#endif
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));  // connect()
	if (ret == SOCKET_ERROR) {
		std::cout << "error, connect to server failture" << std::endl;
		closeSocket();
	}
	else {
		std::cout << "connect to server successfully" << std::endl;
	}
	return ret;
}
void EasyClientTcp::closeSocket() {
	if (_sock != INVALID_SOCKET) {
#ifdef _WIN32
		closesocket(_sock);
		WSACleanup();
#else
		close(_sock);
#endif
		_sock = INVALID_SOCKET;
	}
}

bool EasyClientTcp::runWithSelect() {
	if (!isValidSocket())return false;
	fd_set read_set;
	fd_set write_set;
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_SET(_sock, &read_set);
	FD_SET(_sock, &write_set);
	timeval t = { 1,0 };  //阻塞1s  不设置阻塞时间则一致阻塞在select这里
	int res = select(_sock + 1, &read_set, &write_set, NULL, &t);
	if (res < 0) {
		printf("select failture");
		closeSocket();
		return false;
	}
	if (FD_ISSET(_sock, &read_set)) {
		int dataLen = receiveData();
		if (dataLen == -1) {
			closeSocket();
			return false;
		}
		processData();
		sendData();
	}
	return true;
}
//接收数据 拆包 粘包
int EasyClientTcp::receiveData() {
	std::cout << "receiveData..." << std::endl;
	char recvbuf[256] = {};
	int num = recv(_sock, recvbuf, 256, 0);   // recv()
	if (num > 0) {
		printf("get data from server is :%s\n", recvbuf);
		return num;
	}
	else {
		printf("server exit...\n");
		return -1;
	}
}
// 处理数据
bool EasyClientTcp::processData() {
	std::cout << "processData..." << std::endl;
	return true;
}
//发送数据
bool EasyClientTcp::sendData() {
	std::cout << "sendData..." << std::endl;
	return true;
}
#endif
