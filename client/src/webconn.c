/*File for client web connection*/
#include "../include/webconn.h"

static int conn_connect(Connection conn, char *ip, int port);
static int conn_send(Connection conn, char *data, int len);
static int conn_recv(Connection conn, char *data);
static int conn_close(Connection conn);

Connection new_Connection(){
    Connection conn;

    conn.connect = &conn_connect;
    conn.send = &conn_send;
    conn.recv = &conn_recv;
    conn.close = &conn_close;

    WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0) {
        printf("WSAStartup() failed with error code %d\n", WSAGetLastError());
        exit(1);
	}
	conn.sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn.sock == INVALID_SOCKET) {
        printf("invalid socket!\n");
        exit(1);
	}

    int send_buf_size = 2048;
    if (setsockopt(conn.sock , SOL_SOCKET , SO_SNDBUF , (const char*)&send_buf_size , sizeof(send_buf_size)) == SOCKET_ERROR) {
        printf("setsockopt() failed with error code %d\n", WSAGetLastError());
        exit(1);
    }
    int recv_buf_size = 2048;
    if (setsockopt(conn.sock , SOL_SOCKET , SO_RCVBUF , (const char*)&recv_buf_size , sizeof(recv_buf_size)) == SOCKET_ERROR) {
        printf("setsockopt() failed with error code %d\n", WSAGetLastError());
        exit(1);
    }
    return conn;
}

static int conn_connect(Connection conn, char *ip, int port) {
    struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(ip);
    int ret = connect(conn.sock, (struct sockaddr *)&serAddr, sizeof(serAddr));
	if (ret == SOCKET_ERROR) {
		printf("connect error!\n");
		conn.close(conn);
    }
    return ret;
}

static int conn_close(Connection conn) {
    closesocket(conn.sock);
    WSACleanup();
    return 0;
}

static int conn_send(Connection conn, char *data, int len) {
    char buf[4] = {0}; *(int*)buf = htonl(len);
    if (send(conn.sock, buf, 4, 0) == SOCKET_ERROR) {
        printf("send error!\n");
        conn.close(conn);
        return SOCKET_ERROR;
    }
    int tmplen = len;
    int ret = send(conn.sock, data, tmplen, 0);
    while (tmplen) {
        if (ret == SOCKET_ERROR) {
            printf("send error!\n");
            conn.close(conn);
            return SOCKET_ERROR;
        }
        tmplen -= ret;
        data += ret;
        ret = send(conn.sock, data, tmplen, 0);
    }
    return len;
}

static int conn_recv(Connection conn, char *data) {
    char buf[4] = {0}; recv(conn.sock, buf, 4, 0); 
    int len = ntohl(*(int*)buf);
    int tmplen = len;
    while (tmplen) {
        int ret = recv(conn.sock, data, tmplen, 0);
        if (ret == SOCKET_ERROR) {
            printf("recv error!\n");
            conn.close(conn);
            return SOCKET_ERROR;
        }
        tmplen -= ret;
        data += ret;
    }
    return len;
}

// static int conn_recv(Connection conn, char *data, int len) {
//     return recv(conn.sock, data, len, 0);
// }