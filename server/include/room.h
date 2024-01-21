// include/room.h

#pragma once
#include <iostream>
#include <map>
#include <thread>
#include <bits/std_mutex.h>
#include "account.h"
#include <windows.h>

typedef char RSP_OPTION;
#define ROCK 'r'
#define SCISSORS 's'
#define PAPER 'p'
#define NONE '\0'

typedef int ROOM_TYPE;
#define ROOM_AVAILABLE 0
#define ROOM_FULL 1
#define ROOM_NOT_EXIST 2

typedef struct Player {
    Account account;
    int score;
    RSP_OPTION option;
    Player(Account a);
    Player();
} Player;

typedef struct Room {
    unsigned int id;
    int max_member, cur_member, max_score, cur_score;
    std::map<std::string, Player> members;
    std::mutex lock;
    int judged;
    std::string result;
    Room(unsigned int i, int mm, int ms);
    Room();
    void add_member(Account account);
    void remove_member(Account account);
    void start_game();
    private: void __th_func();
    private: void __judge();
    public: void operator = (Room& r) {
        id = r.id;
        max_member = r.max_member;
        cur_member = r.cur_member;
        max_score = r.max_score;
        cur_score = r.cur_score;
        judged = r.judged;
        result = r.result;
        members = r.members;
        r.members = members;
    }
} Room;

typedef struct RoomPool {
    std::map<unsigned int, Room> rooms;
    unsigned int max_id;
    RoomPool() {
        rooms = std::map<unsigned int, Room>();
        max_id = 0;
    }
    unsigned int create_room(int max_member, int max_score);
    void remove_room(unsigned int id);
    void add_member(unsigned int id, Account account);
    void remove_member(unsigned int id, Account account);
    std::string list_rooms();
    ROOM_TYPE has_room(unsigned int id);
    void start_game(unsigned int id);
} RoomPool;