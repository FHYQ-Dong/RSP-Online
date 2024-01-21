/*Main file for client*/

#include "include/account.h"
#include "include/hash.h"
#include "include/webconn.h"
#include <stdio.h>
#include <stdlib.h>

// for login & register
#define LOGIN_SUCCESS "login success"
#define LOGIN_NO_ACCOUNT "no such account"
#define LOGIN_WRONG_PASSWD "wrong password"
#define REGISTER_SUCCESS "register success"
#define REGISTER_ALREADY_EXIST "account already exist"

// for list/create/join room
#define ROOM_JOINED "room joined"
#define ROOM_NO_ROOM "no such room"
#define ROOM_FULL "room full"

Connection connet_to_server() {
	printf("server address: ");
	char ip[16]; scanf("%s", ip);
	printf("server port: ");
	u_short port; scanf("%hu", &port);
	char buf; scanf("%c", &buf);
	Connection conn = new_Connection();
    CONN(conn, ip, port);
	char welcome_msg[MSG_LEN] = {0};
	RECV(conn, welcome_msg);
	printf("%s\n\n", welcome_msg);
	return conn;
}

Account login_or_register(Connection conn) {
	Account account = new_Account();
	while (1) {
		printf("login/register (l/r): ");
		char choice; scanf("%c", &choice);
		char buf; scanf("%c", &buf);

		if (choice == 'l') {
			account.login_account(&account);
			char account_info[MSG_LEN] = "L";
			account.to_str(&account, account_info+1);
			
			SEND(conn, account_info, strlen(account_info));
			char login_result[MSG_LEN] = {0};
			RECV(conn, login_result);
			if (strcmp(login_result, LOGIN_NO_ACCOUNT) == 0) {
				printf("no such account\n");
				continue;
			}
			else if (strcmp(login_result, LOGIN_WRONG_PASSWD) == 0) {
				printf("wrong password\n");
				continue;
			}
			else {
				account.credit = atoi(login_result);
				printf("login success, your credit is %d\n", account.credit);
				break;
			}
		}

		else if (choice == 'r') {
			account.register_account(&account);
			char account_info[MSG_LEN] = "R";
			account.to_str(&account, account_info+1);

			SEND(conn, account_info, strlen(account_info));
			char register_result[MSG_LEN] = {0};
			RECV(conn, register_result);
			if (strcmp(register_result, REGISTER_ALREADY_EXIST) == 0) {
				printf("account already exist\n");
				continue;
			}
			else {
				account.credit = atoi(register_result);
				printf("register success, your credit is %d\n", account.credit);
				break;
			}
		}
		else printf("invalid choice, please try again.\n\n");
	}
	return account;
}

void print_guide(){
	/*Print user guide in the beginning*/
	printf("\n");
	printf("User guide:\n");
	printf(" * This is the home page of the Rock-Paper-Scissors game program.\n");
	printf(" * In each game, you can enter 'r', 'p' or 's' represent 'rock', 'paper' or 'scissors' to play the game.\n");
	printf(" * 'Rock' wins over 'scissors', 'scissors' wins over 'paper', 'paper' wins over 'rock'.\n");
	printf(" * As this is a multi-functional game program, here are some additional instructions.\n");
	printf(" * This game allows three or more players in one game.\n");
	printf(" * In each round, players will earn more points, which is calculated to be how many players you've won over in the single round.\n");
	printf(" * You can decide the minimum point reached to win in the game, which means that if a player gets not less than this point after a round, players will be ranked and the game will end.\n");
	printf(" * A tie is allowed in each game.\n");
	printf(" * After each game, the credit of players will be changed accoreding to the rank in the game.\n");
	printf(" * You will see the rooms of players here and you can choose one to join.\n");
	printf(" * Of course, if you are not satisfied with the modes the current rooms are adopting,\n");
	printf(" * You can create a new room and decide your prefered mode.\n");
	printf(" * That's all. Wish you a happy playing experience.\n\n");
}

void list_create_join_room(Connection conn) {
	/*
		* room_id: u_int
	*/
	while (1) {
		printf("list/create/join room (l/c/j): ");
		char buf; // scanf("%c", &buf);
		char choice; scanf("%c%c", &choice, &buf);

		if (choice == 'l') {
			char msg[MSG_LEN] = "l";
			SEND(conn, msg, strlen(msg));
			memset(msg, 0, sizeof(msg));
			RECV(conn, msg);
			printf("room_id\tplayers\tscore\n");
			printf("====================================\n");
			printf("%s\n", msg);
			continue;
		}
		else if (choice == 'c') {
			char msg[MSG_LEN] = "c";
			int max_players; printf("max players: "); scanf("%d", &max_players);
			int max_score; printf("max score: "); scanf("%d", &max_score);
			char buf; scanf("%c", &buf);
			char max_players_str[MSG_LEN] = {0}; itoa(max_players, max_players_str, 10);
			char max_score_str[MSG_LEN] = {0}; itoa(max_score, max_score_str, 10);
			strcat(msg, max_players_str); strcat(msg, " "); strcat(msg, max_score_str);
			SEND(conn, msg, strlen(msg));
			char room_id_str[MSG_LEN] = {0};
			RECV(conn, room_id_str);
			u_int room_id = atoi(room_id_str);
			printf("room %u created, you're in it.\n\n", room_id);
			break;
		}
		else if (choice == 'j') {
			printf("room_id: ");
			int room_id; scanf("%d", &room_id);
			char buf; scanf("%c", &buf);
			char room_id_str[MSG_LEN] = "j";
			itoa(room_id, room_id_str+1, 10);
			SEND(conn, room_id_str, strlen(room_id_str));
			char join_result[MSG_LEN] = {0};
			RECV(conn, join_result);
			if (strcmp(join_result, ROOM_JOINED) == 0) {
				printf("join room %u success\n\n", room_id);
				break;
			}
			else if (strcmp(join_result, ROOM_NO_ROOM) == 0) {
				printf("no such room\n\n");
				continue;
			}
			else if (strcmp(join_result, ROOM_FULL) == 0) {
				printf("room full\n\n");
				break;
			}
			else {
				printf("internal error\n\n");
				exit(1);
			}
		}
		else printf("invalid choice, please try again.\n\n");
	}
}

void play(Connection conn) {
	while (1) {
		while (1) {
			printf("rock/paper/scissors/quit (r/p/s/q): ");
			char choice, buf; scanf("%c%c", &choice, &buf);
			choice = choice < 'a' ?	choice - 'A' + 'a' : choice;
			if (choice == 'r' || choice == 'p' || choice == 's' || choice == 'q') {
				if (choice == 'q') {
					printf("quitting...\n");
					conn.close(conn);
					exit(0);
				}
				char msg[MSG_LEN] = {0};
				msg[0] = choice;
				printf("waiting for other players...\n");
				SEND(conn, msg, strlen(msg));
				break;
			}
			else printf("invalid choice, please try again.\n\n");
		}
	char msg[MSG_LEN] = {0};
	RECV(conn, msg);
	printf("name\toption\tscore\n");
	printf("==============================\n");
	printf("%s\n", msg);
	if (strstr(msg, "game over")) break;
	}
}

int main(int argc, char *argv[]) {

	Connection ClientConnection = connet_to_server();
	Account UserAccount = login_or_register(ClientConnection);
	print_guide();
	while (1) {
		list_create_join_room(ClientConnection);
		play(ClientConnection);
	}
	ClientConnection.close(ClientConnection);
	return 0;
}
