// main file of the server

#include <iostream>
#include <cstdlib>
#include <thread>
#include "./include/webconn.h"

#define DEBUG

Connection create_server(const char ip[16], u_short port) {
    Connection conn = Connection(ip, port);
    conn.bind_and_listen(ip, port);
    return conn;
}

void th_func(Connection conn) {
    char msg[MSG_LEN] = "Hello World!";
    SEND(conn, msg, strlen(msg));
    // char msg[MSG_LEN];
    memset(msg, 0, sizeof(msg));
    RECV(conn, msg);
    printf("%s\n", msg);
    conn.close();
}

int main(int argc, char *argv[]) {
    #ifndef DEBUG
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    char *ip = argv[1]; u_short port = atoi(argv[2]);
    #elif defined(DEBUG)
    const char *ip = "0.0.0.0"; u_short port = 12345;
    #endif

    Connection ServerConnection = create_server(ip, port);
    while (true) {
        Connection ClientConnection = ServerConnection.accept();
        if (ClientConnection.sock == INVALID_SOCKET) {
            std::cerr << "accept error!" << std::endl;
            ClientConnection.close();
            continue;
        }
        std::thread t(th_func, ClientConnection);
        // th_func(ClientConnection);
        t.detach();
    }
    WSACleanup();
}