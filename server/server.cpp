// main file of the server

#include <iostream>
#include <cstdlib>
#include <thread>
#include <string>
#include <bits/std_mutex.h>
#include "./include/webconn.h"
#include "./include/account.h"
#include "./include/room.h"
#include <windows.h>

#define DEBUG

// login and register
#define LOGIN_NO_ACCOUNT "no such account"
#define LOGIN_WRONG_PASSWD "wrong password"
#define REGISTER_EXIST "account already exist"

// thread lock
std::mutex mtx;


Connection create_server(const char ip[16], u_short port) {
    Connection conn = Connection(ip, port);
    conn.bind_and_listen(ip, port);
    return conn;
}

int th_func(Connection conn, AccountPool* UserAccounts, RoomPool* Rooms) {

    // greet
    char msg[MSG_LEN] = "Welcome to the server on ";
    strcat(msg, conn.ip); strcat(msg, ":"); strcat(msg, std::to_string(conn.port).c_str());
    SEND(conn, msg, strlen(msg))
    Account account;

    // register or login
    while (true) {
        char msg[MSG_LEN];
        memset(msg, 0, sizeof(msg));
        RECV(conn, msg)
        if (strlen(msg) == 0) {
            std::cerr << "recv error!" << std::endl;
            conn.close();
            return -1;
        }
        char option = msg[0];
        char username[MAX_USERNAME_LENGTH] = {0};
        unsigned char passwd_hash[SHA256_BLOCK_SIZE] = {0};
        memcpy(passwd_hash, msg + 1, SHA256_BLOCK_SIZE);
        strcpy(username, msg + 1 + SHA256_BLOCK_SIZE);
        account = Account(username, passwd_hash);
        ACCOUNT_TYPE result;
        if (option == 'L') { // login
            result = UserAccounts->check_account(account);
            if (result == ACCOUNT_CORRECT) {
                account = UserAccounts->get_account(username);
                SEND(conn, std::to_string(account.credit).c_str(), strlen(std::to_string(account.credit).c_str()))
                break;
            } else if (result == ACCOUNT_NOT_EXIST) {
                SEND(conn, LOGIN_NO_ACCOUNT, strlen(LOGIN_NO_ACCOUNT))
                continue;
            } else if (result == ACCOUNT_WRONG_PASSWD) {
                SEND(conn, LOGIN_WRONG_PASSWD, strlen(LOGIN_WRONG_PASSWD))
                continue;
            }
        }
        else if (option == 'R') { // register
            mtx.lock();
            result = UserAccounts->add_account(account);
            mtx.unlock();
            if (result == ACCOUNT_CORRECT) {
                SEND(conn, std::to_string(account.credit).c_str(), strlen(std::to_string(account.credit).c_str()))
                break;
            } else if (result == ACCOUNT_ALREADY_EXIST) {
                SEND(conn, REGISTER_EXIST, strlen(REGISTER_EXIST))
                continue;
            }
        } 
        else {
            std::cerr << "unknown option: " << option << std::endl;
            continue;
        }
    }

    while (true) {
        // room
        unsigned int room_id;
        while (true) {
            char msg[MSG_LEN];
            memset(msg, 0, sizeof(msg));
            RECV(conn, msg)
            if (strlen(msg) == 0) {
                std::cerr << "recv error!" << std::endl;
                conn.close();
                return -1;
            }
            char option = msg[0];
            if (option == 'q') {
                conn.close();
                return 0;
            }
            if (option == 'l') {
                std::string res = Rooms->list_rooms();
                SEND(conn, res.c_str(), strlen(res.c_str()))
                continue;
            }
            else if (option == 'c') {
                std::string msg_str(msg+1);
                std::string max_player_str = msg_str.substr(0, msg_str.find_first_of(' '));
                std::string max_score_str = msg_str.substr(msg_str.find_first_of(' ') + 1);
                int max_player = atoi(max_player_str.c_str()); int max_score = atoi(max_score_str.c_str());
                room_id = Rooms->create_room(max_player, max_score);
                Rooms->add_member(room_id, account);
                Rooms->start_game(room_id);
                SEND(conn, std::to_string(room_id).c_str(), strlen(std::to_string(room_id).c_str()))
                break;
            }
            else if (option == 'j') {
                room_id = atoi(msg+1);
                ROOM_TYPE result = Rooms->has_room(room_id);
                if (result == ROOM_NOT_EXIST) {
                    SEND(conn, "no such room", strlen("no such room"))
                    continue;
                }
                else if (result == ROOM_FULL) {
                    SEND(conn, "room full", strlen("room full"))
                    continue;
                }
                else if (result == ROOM_AVAILABLE) {
                    Rooms->add_member(room_id, account);
                    SEND(conn, "room joined", strlen("room joined"))
                    break;
                }
            }
        }

        // play
        while (true) {
            Rooms->rooms[room_id].lock.lock();
            if (Rooms->rooms[room_id].cur_member == Rooms->rooms[room_id].max_member) {
                Rooms->rooms[room_id].lock.unlock();
                break;
            }
            Rooms->rooms[room_id].lock.unlock();
            Sleep(500);
        }
        while (true) {
            char msg[MSG_LEN];
            memset(msg, 0, sizeof(msg));
            if (conn.recv(msg) == SOCKET_ERROR) {
                std::cerr << "recv error!" << std::endl;
                conn.close();
                Rooms->rooms[room_id].lock.lock();
                Rooms->remove_member(room_id, account);
                Rooms->rooms[room_id].lock.unlock();
                account.credit -= 2;
                mtx.lock();
                UserAccounts->update_account(account);
                mtx.unlock();
                return -1;
            }
            if (strlen(msg) == 0) {
                std::cerr << "recv error!" << std::endl;
                conn.close();
                Rooms->rooms[room_id].lock.lock();
                Rooms->remove_member(room_id, account);
                Rooms->rooms[room_id].lock.unlock();
                account.credit -= 2;
                mtx.lock();
                UserAccounts->update_account(account);
                mtx.unlock();
                return -1;
            }
            if (Rooms->rooms[room_id].max_member == 1) {
                std::string res = "game over! current credit: " + std::to_string(account.credit) + '\n';
                if (conn.send(res.c_str(), strlen(res.c_str())) == SOCKET_ERROR) {
                    std::cerr << "send error!" << std::endl;
                    conn.close();
                    Rooms->remove_member(room_id, account);
                    account.credit -= 2;
                    mtx.lock();
                    UserAccounts->update_account(account);
                    mtx.unlock();
                    return -1;
                }
                Rooms->rooms[room_id].lock.lock();
                Rooms->remove_member(room_id, account);
                Rooms->remove_room(room_id);
                Rooms->rooms[room_id].lock.unlock();
                break;
            }
            if (msg[0] == 'q') {
                Rooms->rooms[room_id].lock.lock();
                Rooms->remove_member(room_id, account);
                Rooms->rooms[room_id].lock.unlock();
                conn.close();
                account.credit -= 2;
                mtx.lock();
                UserAccounts->update_account(account);
                mtx.unlock();
                return 0;
            }
            Rooms->rooms[room_id].lock.lock();
            Rooms->rooms[room_id].members[account.username].option = msg[0];
            Rooms->rooms[room_id].lock.unlock();
            Sleep(500);
            while (true) {
                Rooms->rooms[room_id].lock.lock();
                if (Rooms->rooms[room_id].judged) {
                    if (Rooms->rooms[room_id].cur_score >= Rooms->rooms[room_id].max_score) {
                        printf("user: %s, score: %d, game over\n", account.username, Rooms->rooms[room_id].cur_score); 
                        break;
                    }
                    if (conn.send(Rooms->rooms[room_id].result.c_str(), strlen(Rooms->rooms[room_id].result.c_str())) == SOCKET_ERROR) {
                        std::cerr << "send error!" << std::endl;
                        conn.close();
                        Rooms->rooms[room_id].lock.lock();
                        Rooms->remove_member(room_id, account);
                        Rooms->rooms[room_id].lock.unlock();
                        account.credit -= 2;
                        mtx.lock();
                        UserAccounts->update_account(account);
                        mtx.unlock();
                        Rooms->rooms[room_id].lock.unlock();
                        return -1;
                    }
                    Rooms->rooms[room_id].judged--;
                    Rooms->rooms[room_id].lock.unlock();
                    break;
                }
                Rooms->rooms[room_id].lock.unlock();
                Sleep(500);
            }
            if (Rooms->rooms[room_id].cur_score >= Rooms->rooms[room_id].max_score) {
                
                account = Rooms->rooms[room_id].members[account.username].account;
                std::string res = Rooms->rooms[room_id].result + "game over! current credit: " + std::to_string(account.credit) + '\n';
                if (conn.send(res.c_str(), strlen(res.c_str())) == SOCKET_ERROR) {
                    std::cerr << "send error!" << std::endl;
                    conn.close();
                    Rooms->rooms[room_id].lock.lock();
                    Rooms->remove_member(room_id, account);
                    Rooms->rooms[room_id].lock.unlock();
                    account.credit -= 2;
                    mtx.lock();
                    UserAccounts->update_account(account);
                    mtx.unlock();
                    Rooms->rooms[room_id].lock.unlock();
                    return -1;
                }
                Rooms->remove_member(room_id, account);
                Rooms->rooms[room_id].lock.unlock();
                break;
            }
        }
        mtx.lock();
        UserAccounts->update_account(account);
        mtx.unlock();
    }

    conn.close();
    return 0;
}

int main(int argc, char *argv[]) {
    #ifndef DEBUG
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <account_file>\n", argv[0]);
        exit(1);
    }
    char *ip = argv[1]; u_short port = atoi(argv[2]);
    char *account_file = argv[3];
    #elif defined(DEBUG)
    const char *ip = "0.0.0.0"; u_short port = 12345;
    const char *account_file = "./account.dat";
    #endif
    
    AccountPool UserAccounts(account_file);
    RoomPool Rooms;
    
    Connection ServerConnection = create_server(ip, port);
    while (true) {
        Connection ClientConnection = ServerConnection.accept();
        if (ClientConnection.sock == INVALID_SOCKET) {
            std::cerr << "accept error!" << std::endl;
            ClientConnection.close();
            continue;
        }
        std::thread t(th_func, ClientConnection, &UserAccounts, &Rooms);
        t.detach();
    }
    WSACleanup();
}