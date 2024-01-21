#include "../include/webconn.h"

Connection::Connection(const char ip[16], u_short port) {
    this->sock = INVALID_SOCKET;
    strcpy(this->ip, ip); this->port = port;
    WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0) {
        std::cerr << "WSAStartup() failed with error code " << WSAGetLastError() << std::endl;
        exit(1);
	}
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
        std::cerr << "invalid socket!" << std::endl;
        exit(1);
	}

    int send_buf_size = 1024;
    if (setsockopt(sock , SOL_SOCKET , SO_SNDBUF , (const char*)&send_buf_size , sizeof(send_buf_size)) == SOCKET_ERROR) {
        std::cerr << "setsockopt() failed with error code " << WSAGetLastError() << std::endl;
        exit(1);
    }
    int recv_buf_size = 1024;
    if (setsockopt(sock , SOL_SOCKET , SO_RCVBUF , (const char*)&recv_buf_size , sizeof(recv_buf_size)) == SOCKET_ERROR) {
        std::cerr << "setsockopt() failed with error code " << WSAGetLastError() << std::endl;
        exit(1);
    }

    u_long onBlock = 0;
    if (ioctlsocket(sock, FIONBIO, &onBlock) == SOCKET_ERROR) {
        std::cerr << "ioctlsocket() failed with error code " << WSAGetLastError() << std::endl;
        exit(1);
    }
}

Connection::Connection(SOCKET s, char ip[16], u_short port) {
    this->sock = s;
    strcpy(this->ip, ip); this->port = port;
}

Connection::Connection() {
    this->sock = INVALID_SOCKET;
    this->ip[0] = '\0'; this->port = 0;
}

Connection Connection::accept() {
    SOCKET client_sock;
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
    client_sock = ::accept(this->sock, (SOCKADDR*)&remoteAddr, &nAddrlen);
    if (client_sock == INVALID_SOCKET) {
        std::cerr << "accept error!" << std::endl;
        return Connection();
        // this->close();
        // exit(1);
    }
    Connection conn(client_sock, this->ip, this->port);
    printf("accept connection from %s:%d\n", inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));
    return conn;
}

int Connection::bind_and_listen(const char ip[16], u_short port) {
    struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = inet_addr(ip);
	if(::bind(this->sock, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
		std::cerr << "socket bind error" << std::endl;
        exit(1);
	}
    if(::listen(this->sock, 5) == SOCKET_ERROR) {
        std::cerr << "socket listen error" << std::endl;
        exit(1);
    }
    return 0;
}


int Connection::close() {
    closesocket(this->sock);
    return 0;
}

int Connection::send(const char *data, int len) {
    char buf[4] = {0}; *(int*)buf = htonl(len);
    if (::send(this->sock, buf, 4, 0) == SOCKET_ERROR) {
        printf("send error!\n");
        this->close();
        return SOCKET_ERROR;
    }
    int tmplen = len;
    while (tmplen) {
        int ret = ::send(this->sock, data, tmplen, 0);
        if (ret == SOCKET_ERROR) {
            printf("send error!\n");
            this->close();
            return SOCKET_ERROR;
        }
        tmplen -= ret;
        data += ret;
    }
    return len;
}

int Connection::recv(char *data) {
    char buf[4] = {0};
    if (::recv(this->sock, buf, 4, 0) == SOCKET_ERROR) {
        printf("recv error!\n");
        this->close();
        return SOCKET_ERROR;
    }
    int len = ntohl(*(int*)buf);
    int tmplen = len;
    while (tmplen) {
        int ret = ::recv(this->sock, data, tmplen, 0);
        if (ret == SOCKET_ERROR) {
            printf("recv error!\n");
            this->close();
            return SOCKET_ERROR;
        }
        tmplen -= ret;
        data += ret;
    }
    return len;
}
