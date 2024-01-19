// main file for client

#include "include/account.h"
#include "include/hash.h"
#include "include/webconn.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	Account UserAccount = new_Account();
	UserAccount.register_account(&UserAccount);
	printf("The username is: %s\n", UserAccount.username);
	printf("The password is: %s\n", UserAccount.passwd);
	printf("The hash is: ");
	for (int i = 0; i < 32; i++)
		printf("%02x", UserAccount.passwd_hash[i]);
	printf("\n");

	Connection conn = new_Connection();
    conn.connect(conn, "127.0.0.1", 1234);
    char *msg = UserAccount.passwd_hash;
    conn.send(conn, msg, 32);
    char buf[1024];
    conn.recv(conn, buf, 5);
	printf("The server says: %s\n", buf);
	conn.close(conn);
	return 0;
}