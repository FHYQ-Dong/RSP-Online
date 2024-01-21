#include <iostream>
#include <Windows.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to an address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345); // Change this to the desired port number

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen for connections" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started. Listening for connections..." << std::endl;

    // Accept incoming connections
    SOCKET clientSocket;
    sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    while ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize)) != INVALID_SOCKET) {
        std::cout << "New connection accepted" << std::endl;

        // Handle client connection here
        char buf[4] = {0}; *(int*)buf = ntohl(13);
        send(clientSocket, buf, 4, 0);
        send(clientSocket, "Hello World!", 13, 0);
        char buff[1024];
        memset(buff, 0, sizeof(buf));
        recv(clientSocket, buff, sizeof(buff), 0);
        std::cout << buff << std::endl;
        // Close the client socket
        closesocket(clientSocket);
    }

    // Close the server socket
    closesocket(serverSocket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}