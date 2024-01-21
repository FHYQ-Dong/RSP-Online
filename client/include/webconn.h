// include/webconn.h

#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>

#pragma  comment(lib, "ws2_32.lib")

#define MSG_LEN 1024

#define CONN(conn, ip, port) if (conn.connect(conn, ip, port) == SOCKET_ERROR) exit(1)
#define SEND(conn, data, len) if (conn.send(conn, data, len) == SOCKET_ERROR) exit(1)
#define RECV(conn, data) if (conn.recv(conn, data) == SOCKET_ERROR) exit(1)

typedef struct Connection {
    SOCKET sock;

    int (*connect)(struct Connection conn, char* ip, int port);
    int (*send)(struct Connection conn, char* data, int len);
    int (*recv)(struct Connection conn, char* data);
    int (*close)(struct Connection conn);
} Connection;

Connection new_Connection();
