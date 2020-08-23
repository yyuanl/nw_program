#include "EasyTcpClient.hpp"
#include <stdlib.h>
#include <thread>

constexpr auto MSGBUFLEN = 128;
using namespace std;
bool is_exit = false;
void cmd_thread(SOCKET req_sock) {
	while (1) {
		char cmdbuf[MSGBUFLEN];
		scanf("%s", cmdbuf);
		if (0 == strcmp(cmdbuf, "exit")) {
			printf("exit client commond.\n");
			is_exit = true;
			return;
		}
		send(req_sock, cmdbuf, MSGBUFLEN, 0);
	}
}

// 可以从配置文件读取
const char ip[] = "192.168.1.112";
unsigned short port = 4567;
int main() {
	EasyClientTcp client;
	client.initSocket();
	client.connect2Server(ip,port);
	std::thread td(cmd_thread, client.getSocket());
	td.detach();
	while (!is_exit) {
		client.runWithSelect();
	}
	client.closeSocket();
	printf("client exit...\n");
	getchar();
	return 0;
}