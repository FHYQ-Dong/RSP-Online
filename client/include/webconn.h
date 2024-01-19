// include/webconn.h

#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>

#pragma  comment(lib, "ws2_32.lib")

typedef struct Connection {
    SOCKET sock;

    int (*connect)(struct Connection conn, char* ip, int port);
    int (*send)(struct Connection conn, char* data, int len);
    int (*recv)(struct Connection conn, char* data, int len);
    int (*close)(struct Connection conn);
} Connection;

Connection new_Connection();
