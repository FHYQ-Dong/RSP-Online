// include/webconn.h

#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <windows.h>

#pragma  comment(lib, "ws2_32.lib")

#define MSG_LEN 1024
 
#define SEND(conn, data, len) if (conn.send(data, len) == SOCKET_ERROR) { conn.close(); return SOCKET_ERROR; }
#define RECV(conn, data) if (conn.recv(data) == SOCKET_ERROR) { conn.close(); return SOCKET_ERROR; }

typedef struct Connection {
    SOCKET sock;
    char ip[16]; u_short port;
    int bind_and_listen(const char ip[16], u_short port);
    Connection accept();
    int send(const char* data, int len);
    int recv(char* data);
    int close();
    Connection(const char ip[16], u_short port);
    Connection(SOCKET s, char ip[16], u_short port);
    Connection();
} Connection;
