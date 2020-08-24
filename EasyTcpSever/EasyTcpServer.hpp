#ifndef __EASY_TCP_SERVER__
#define __EASY_TCP_SERVER__


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



#include <vector>
#include <iostream>
#include <memory>




#define BUFSIZE 1024
enum STATE_RECV_SEND
{
	RECV_ABNORMAL,
	SEND_ABNORMAL,
	NORMAL
};

using namespace std;
class T {
public:
	virtual STATE_RECV_SEND m_recv_data() = 0;
	virtual STATE_RECV_SEND m_process_data() = 0;
	virtual STATE_RECV_SEND m_send_data() = 0;
	virtual ~T() {}
};
class myTask :public T {
public:
	myTask(SOCKET conn_fd):_conn_fd(conn_fd) , curr_state(NORMAL){
		memset(recv_buf, '\0',BUFSIZE);
		memset(send_buf, '\0',BUFSIZE);
	}
	STATE_RECV_SEND m_recv_data() {
		std::cout << "recv.." << std::endl;
		int num = recv(_conn_fd, recv_buf, BUFSIZE, 0);
		if (num <= 0) {
			printf("client exit or recv error...\n");
			curr_state = RECV_ABNORMAL;
			return RECV_ABNORMAL;
		}
		return NORMAL;
	}
	STATE_RECV_SEND m_process_data() {// 解析客户端数据
		std::cout << "server is process received data" << std::endl;
		printf("server get the data come from client : %s\n", recv_buf);
		if (0 == strcmp(recv_buf, "getName")) {
			char t[] = "hello,your name is li ming";
			strncpy(send_buf, t, strlen(t));
		}
		else if (0 == strcmp(recv_buf, "getAge")) {
			char t[] = "hello,your age is 18";
			strncpy(send_buf, t, strlen(t));
		}
		else {
			char t[] = "sorry, server don't konw your command. What do you want to get ??";
			strncpy(send_buf, t, strlen(t));
		}
		return NORMAL;
	}
	STATE_RECV_SEND m_send_data() {
		std::cout << "send data" << std::endl;
		if (send(_conn_fd, send_buf, strlen(send_buf) + 1, 0) < 0) {
			return SEND_ABNORMAL;
		}
		memset(recv_buf, '\0', BUFSIZE);
		memset(send_buf, '\0', BUFSIZE);
		return NORMAL;
	
	}
	SOCKET get_conn_fd() { return _conn_fd; }
	//STATE_RECV_SEND curr_state
private:
	SOCKET _conn_fd;
	char recv_buf[BUFSIZE];
	char send_buf[BUFSIZE];
	STATE_RECV_SEND curr_state;  // 当前状态 读写引起的出错将修改状态

};

class EasyTcpServer {
public:
	EasyTcpServer(): listen_fd(INVALID_SOCKET),num_ready(0){}
	~EasyTcpServer() { closefd(); }
	//打开监听描述符
	SOCKET openListenfd();
	//使用select连接多客户端
	bool runWithSelect();
	//关闭listen_fd
	bool closefd();
	
private:
	//连接客户端请求
	//bool m_accept();
	SOCKET listen_fd;
	void check_client_pool(vector<shared_ptr<myTask>> &pool, fd_set *ready_set, fd_set *read_set);
	vector<shared_ptr<myTask>>task_pool;
	int num_ready;
	

};

SOCKET EasyTcpServer::openListenfd() {
	//socket --> bind() --> listen()
#ifdef _WIN32
	//启动socket网络服务
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
#endif
	if (listen_fd != INVALID_SOCKET) {
		std::cout << "close old listen socket " << (int)listen_fd << std::endl;
		closefd();		
	}
	//1.建立socket
	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2. bind 绑定网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); // 主机到 网络字节顺序
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;  //inet_addr("192.168.1.112")
#else
	_sin.sin_addr.s_addr = INADDR_ANY;
#endif 
	//inet_pton();
	if (bind(listen_fd, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		printf("bind socket port failture\n");
		return SOCKET_ERROR;
	}
	else {
		printf("bind socket port successfully\n");
	}
	//3.监听网络端口
	if (SOCKET_ERROR == listen(listen_fd, 5)) {
		std::cout << "listen socket failture" << std::endl;
		return SOCKET_ERROR;
	}
	else {
		std::cout << "listen socket successfully" << std::endl;
		return listen_fd;
	}
}

//bool EasyTcpServer::m_accept() {}
bool EasyTcpServer::runWithSelect() {
	fd_set ready_read_set;  // 就绪描述符
	static fd_set need_read_set;  // select 第二个参数 读集合
	/*static fd_set ready_write_set;
	static fd_set need_write_set;*/
	//FD_ZERO(&need_read_set);
	FD_SET(listen_fd, &need_read_set);
	static SOCKET max_fd = listen_fd;
	
	ready_read_set = need_read_set;
	//ready_write_set = need_write_set;

	std::cout << "before select" << std::endl;
	for (int i = 0; i < need_read_set.fd_count; i++) {
		std::cout << "n_r is " << need_read_set.fd_array[i] << std::endl;
	}
	/*for (int i = 0; i < need_write_set.fd_count; i++) {
		std::cout << "n_w is " << need_write_set.fd_array[i] << std::endl;
	}*/

	num_ready = select(max_fd + 1, &ready_read_set, NULL, NULL, NULL);
	
	std::cout << "after select" << std::endl;
	for (int i = 0; i < need_read_set.fd_count; i++) {
		std::cout << "n_r is " << need_read_set.fd_array[i] << std::endl;
	}
	/*for (int i = 0; i < need_write_set.fd_count; i++) {
		std::cout << "n_w is " << need_write_set.fd_array[i] << std::endl;
	}*/
	std::cout << "select res is " << num_ready << endl;
	if (num_ready < 0) {
		printf("select failture\n");
		return false;
	}
	if (FD_ISSET(listen_fd, &ready_read_set)) { // listen_fd检测到新连接到来
		// 4. 等待接收客户端连接请求
		sockaddr_in clientAddr = {};
		int lenAddr = sizeof(sockaddr_in);
		SOCKET conn_fd = accept(listen_fd, (sockaddr*)&clientAddr, (socklen_t*)&lenAddr);
		if (conn_fd == INVALID_SOCKET) {
			std::cout << "invalid socket connection..." << endl;
		}
		std::cout << "one client come, it IP = " << inet_ntoa(clientAddr.sin_addr) << " socket fd is " << (int)conn_fd << endl;
		FD_SET(conn_fd, &need_read_set);  // 将新连接的socket fd 加入slect描述符集合
		max_fd = (max_fd < conn_fd) ? conn_fd : max_fd;
		shared_ptr<myTask>p(new myTask(conn_fd));		
		task_pool.push_back(p);  //新的客户端连接放入容器
		num_ready--;  //listen_fd不在被监听
	}
	check_client_pool(task_pool, &ready_read_set, &need_read_set);
}
bool EasyTcpServer::closefd() {
	if (listen_fd != INVALID_SOCKET) {
#ifdef _WIN32
		//6.关闭监听socket
		closesocket(listen_fd);
		WSACleanup();
#else
		close(listen_fd);
#endif 
	}
	return true;

}


void EasyTcpServer::check_client_pool(vector<shared_ptr<myTask>> &pool, fd_set *ready_read_set, fd_set *need_read_set) {
	//if (num_ready <= 0)return;
	for (auto iter = pool.begin(); iter != pool.end();) {
	
		SOCKET t_sock = (*iter)->get_conn_fd();
		std::cout << "curr check socket is " << (int)t_sock << std::endl;
		
		if (FD_ISSET(t_sock, ready_read_set) && (t_sock > 0)) {  //读数据
			STATE_RECV_SEND ret = (*iter)->m_recv_data();
			if (ret != NORMAL) {
#ifdef _WIN32
				closesocket(t_sock);
#else
				close(t_sock);
#endif
				FD_CLR(t_sock, need_read_set);
				iter = pool.erase(iter); // 防止迭代器失效做法
			}
			else {
				(*iter)->m_process_data();  // 读了之后就处理数据
				std::cout << "read to send data" << std::endl;
				(*iter)->m_send_data();   //发送数据
				iter++;
			}
			
			//std::cout << "set write event" << std::endl;
			//FD_SET((*iter)->get_conn_fd(), need_write_set);
			
		}
		else {
			iter++;
		}
		
		
//		else {// 遇到EOF 客户端关闭
//			std::cout << "eof or client close" << std::endl;
//#ifdef _WIN32
//			closesocket(t_sock);
//#else
//			close(t_sock);
//#endif 
//			FD_CLR(t_sock, need_read_set);
//			iter = pool.erase(iter);
//		}

	}
}

#endif
