// include/webconn.h

#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <windows.h>

#pragma  comment(lib, "ws2_32.lib")

#define MSG_LEN 1024
 
#define SEND(conn, data, len) if (conn.send(data, len) == SOCKET_ERROR) exit(1)
#define RECV(conn, data) if (conn.recv(data) == SOCKET_ERROR) exit(1)

typedef struct Connection {
    SOCKET sock;

    int bind_and_listen(const char ip[16], u_short port);
    Connection accept();
    int send(char* data, int len);
    int recv(char* data);
    int close();
    Connection(const char ip[16], u_short port);
    Connection(SOCKET s);
    Connection();
} Connection;
