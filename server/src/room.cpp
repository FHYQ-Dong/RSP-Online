#include "../include/room.h"

inline int max(int a, int b) { return a > b ? a : b; }
inline int min(int a, int b) { return a < b ? a : b; }

Player::Player(Account a) {
    account = a;
    score = 0;
    option = NONE;
}

Player::Player() {
    account = Account();
    score = 0;
    option = NONE;
}

Room::Room(unsigned int i, int mm, int ms) {
    id = i;
    max_member = mm;
    cur_member = 0;
    max_score = ms;
    cur_score = 0;
    judged = 0;
    result = "";
    members = std::map<std::string, Player>();
}

Room::Room() {
    id = 0;
    max_member = 0;
    cur_member = 0;
    max_score = 0;
    cur_score = 0;
    judged = 0;
    result = "";
    members = std::map<std::string, Player>();
}

void Room::add_member(Account account) {
    lock.lock();
    Player player(account);
    members[player.account.username] = player;
    cur_member++;
    lock.unlock();
}

void Room::remove_member(Account account) {
    lock.lock();
    std::string username = account.username;
    members.erase(username);
    cur_member--; max_member--;
    lock.unlock();
}

void Room::start_game() {
    std::thread th(__th_func, this);
    th.detach();
}

bool operator > (const Player& a, const Player& b) {
    return a.option == ROCK && b.option == SCISSORS ||
           a.option == SCISSORS && b.option == PAPER ||
           a.option == PAPER && b.option == ROCK;
}
bool operator < (const Player& a, const Player& b) {
    return a.option == ROCK && b.option == PAPER ||
           a.option == SCISSORS && b.option == ROCK ||
           a.option == PAPER && b.option == SCISSORS;
}
bool operator == (const Player& a, const Player& b) {
    return a.option == b.option;
}

void Room::__judge() {
    std::map<std::string, Player>::iterator it1, it2;
    for (it1 = this->members.begin(); it1 != this->members.end(); it1++) {
        for (it2 = this->members.begin(); it2 != this->members.end(); it2++) {
            Player player1 = it1->second, player2 = it2->second;
            if (player1 > player2) player1.score++;
            it1->second = player1;
        }
    }
}

void Room::__th_func() {
    while (this->cur_member < this->max_member) Sleep(500);
    while (true && cur_member) {
        while (true && cur_member) {
            std::map<std::string, Player>::iterator it;
            bool all_selected = true;
            lock.lock();
            for (it = this->members.begin(); it != this->members.end(); it++) {
                Player player = it->second;
                RSP_OPTION option = player.option;
                if (option == NONE) {
                    all_selected = false;
                    break;
                }
                else continue;
            }
            lock.unlock();
            if (all_selected) break;
            else Sleep(500);
        }
        lock.lock();
        __judge();
        judged = cur_member;
        std::map<std::string, Player>::iterator it;
        result = "";
        for (it = this->members.begin(); it != this->members.end(); it++) {
            Player player = it->second;
            std::string username = it->first, opt = player.option == ROCK ? "ROCK" : player.option == SCISSORS ? "SCISSOR" : "PAPER";
            result += username + "\t" + opt + "\t" + std::to_string(player.score) + "\n";
            player.option = NONE;
            it->second = player;
            cur_score = max(cur_score, player.score);
        }
        if (cur_score >= max_score)  {
            std::map<std::string, Player>::iterator it;
            int mn = 0x7fffffff, mx = -1;
            for (it = this->members.begin(); it != this->members.end(); it++) {
                Player player = it->second;
                mn = min(mn, player.score); mx = max(mx, player.score);
            }
            for (it = this->members.begin(); it != this->members.end(); it++) {
                Player player = it->second;
                if (player.score == mx) player.account.credit += 1;
                else if (player.score == mn) player.account.credit -= 1;
                else ;
                it->second = player;
            }
            lock.unlock();
            break;
        }
        else {
            lock.unlock();
            Sleep(500);
        }
    }
    lock.unlock();
}

unsigned int RoomPool::create_room(int max_member, int max_score) {
    Room room(max_id++, max_member, max_score);
    rooms[room.id] = room;
    return room.id;
}

void RoomPool::remove_room(unsigned int id) {
    rooms.erase(id);
}

void RoomPool::add_member(unsigned int id, Account account) {
    rooms[id].add_member(account);
}

void RoomPool::remove_member(unsigned int id, Account account) {
    rooms[id].remove_member(account);
    if (rooms[id].cur_member == 0) remove_room(id);
}

std::string RoomPool::list_rooms() {
    std::string result = "";
    std::map<unsigned int, Room>::iterator it;
    for (it = rooms.begin(); it != rooms.end(); ) {
        if (it->second.max_member == 0) {
            rooms.erase(it++);
            continue;
        }
        result += std::to_string(it->second.id) + "\t" + std::to_string(it->second.cur_member) + "/" + std::to_string(it->second.max_member) + "\t" + std::to_string(it->second.max_score) + "\n";
        it++;
    }
    return result;
}

void RoomPool::start_game(unsigned int id) {
    rooms[id].start_game();
}

ROOM_TYPE RoomPool::has_room(unsigned int id) {
    if (rooms.find(id) == rooms.end()) return ROOM_NOT_EXIST;
    else if (rooms[id].cur_member == rooms[id].max_member) return ROOM_FULL;
    else return ROOM_AVAILABLE;
}