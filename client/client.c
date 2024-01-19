// main file for client

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

// for list/create/join room
#define MAX_ROOM 1024
#define ROOM_JOINED "room joined"
#define ROOM_NO_ROOM "no such room"
#define ROOM_ALREADY_IN "already in room"

Connection connet_to_server() {
	printf("server address: ");
	char ip[16]; scanf("%s", ip);
	printf("server port: ");
	u_short port; scanf("%hu", &port);

	Connection conn = new_Connection();
    CONN(conn, ip, port);
	char welcome_msg[MSG_LEN] = {0};
	RECV(conn, welcome_msg, MSG_LEN);
	printf("%s\n", welcome_msg);

	return conn;
}

Account login_or_register(Connection conn) {
	Account account = new_Account();
	while (1) {
		printf("login/register (l/r): ");
		char choice; scanf("%c", &choice);

		if (choice == 'l') {
			account.login_account(&account);
			char account_info[MSG_LEN] = "LOGIN";
			account.to_str(&account, account_info+9);
			
			SEND(conn, account_info, MSG_LEN);
			char login_result[MSG_LEN] = {0};
			RECV(conn, login_result, MSG_LEN);
			if (strcmp(login_result, LOGIN_SUCCESS) == 0) {
				printf("login success\n");
				break;
			}
			else if (strcmp(login_result, LOGIN_NO_ACCOUNT) == 0) {
				printf("no such account\n");
				continue;
			}
			else if (strcmp(login_result, LOGIN_WRONG_PASSWD) == 0) {
				printf("wrong password\n");
				continue;
			}
			else {
				printf("internal error\n");
				exit(1);
			}
		}

		else if (choice == 'r') {
			account.register_account(&account);
			char account_info[MSG_LEN] = "REGISTER";
			account.to_str(&account, account_info+9);

			SEND(conn, account_info, MSG_LEN);
			char register_result[MSG_LEN] = {0};
			RECV(conn, register_result, MSG_LEN);
			if (strcmp(register_result, REGISTER_SUCCESS) == 0) {
				printf("register success\n");
				break;
			}
			else {
				printf("internal error\n");
				exit(1);
			}
		}
		else printf("invalid choice, please try again.\n\n");
	}
	return account;
}

void create_join_room(Connection conn) {
	/*
		* room_id: u_int
	*/
	while (1) {
		printf("list/create/join room (l/c/j): ");
		char choice; scanf("%c", &choice);

		if (choice == 'l') {
			char msg[MSG_LEN] = "LIST";
			SEND(conn, msg, MSG_LEN);
			u_int room_id[MAX_ROOM] = {0};
			RECV(conn, (char*)room_id, MSG_LEN);
			for (int i=0; room_id[i]; ++i) 
				room_id[i] = ntohl(room_id[i]);
			printf("room id: ");
			int flag = 0;
			for (int i=0; room_id[i]; ++i) {
				printf("%u%c", room_id[i], (i+1)%4 ? '\t' : '\n');
				flag = (i+1) % 4;
			}
			if (flag) printf("\n");
			continue;
		}
		else if (choice == 'c') {
			char msg[MSG_LEN] = "CREATE";
			SEND(conn, msg, MSG_LEN);
			char room_id_str[MSG_LEN] = {0};
			RECV(conn, room_id_str, MSG_LEN);
			u_int room_id = *(u_int*)room_id_str;
			printf("room %u created, you've already joined it.", room_id);
		}
		else if (choice == 'j') {
			printf("room id: ");
			u_int room_id; scanf("%d", &room_id);
			char room_id_str[MSG_LEN] = {0};
			*(u_int*)room_id_str = htonl(room_id);
			SEND(conn, room_id_str, MSG_LEN);
			char join_result[MSG_LEN] = {0};
			RECV(conn, join_result, MSG_LEN);
			if (strcmp(join_result, ROOM_JOINED) == 0) {
				printf("join room %u success\n", room_id);
				break;
			}
			else if (strcmp(join_result, ROOM_NO_ROOM) == 0) {
				printf("no such room\n");
				continue;
			}
			else if (strcmp(join_result, ROOM_ALREADY_IN) == 0) {
				printf("you've already in room %u\n", room_id);
				break;
			}
			else {
				printf("internal error\n");
				exit(1);
			}
		}
		else printf("invalid choice, please try again.\n\n");
	}
}

int main(int argc, char *argv[]) {

	Connection ServerConnection = connet_to_server();
	Account UserAccount = login_or_register(ServerConnection);
	create_join_room(ServerConnection);

	return 0;
}

/* 
	Process		||		Behavior[server]	||		Behavior[client]
=========================================================================
	1. connect	||		send welcome msg	||		recv welcome msg

	2. login/register
				||		send operation		||		recv operation
	2.1 login	||		recv account info	||		send account info
				||		send login result	||		recv login result
	2.2 reg		||		recv account info	||		send account info
				||		send reg result		||		recv reg result

	3. list/create/join room
				||		send operation		||		recv operation
	3.1 list	||		send list			||		recv list
	3.2 create	||		send create			||		recv room id
	3.3 join	||		send join			||		recv join result
*/
