#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <winSock2.h>
// #pragma comment(lib, "ws2_32.lib")
using namespace std;
int main() {
	//����socket�������
	WORD ver = MAKEWORD(2,2);
	WSADATA data;
	WSAStartup(ver, &data);

	/*
	
	
	
	*/


	// �ر�
	WSACleanup();
	cout << "test" << endl;


	return 0;
}