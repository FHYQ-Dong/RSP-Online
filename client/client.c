/*Main file for client*/

#include "include/account.h"
#include "include/hash.h"
#include "include/webconn.h"
#include <stdio.h>
#include <stdlib.h>

void print_guide(){
	/*Print user guide in the beginning*/
	printf("User guide:\n");
	printf("This is the home page of the Rock-Paper-Scissors game program.\n");
	printf("In each game, you can enter 'rock', 'paper' or 'scissors' to play the game.\n");
	printf("'Rock' wins over 'scissors', 'scissors' wins over 'paper', 'paper' wins over 'rock'.\n");
	printf("As this is a multi-functional game program, here are some additional instructions.\n");
	printf("This game allows three or more players in one game.\n");
	printf("In each round, players will earn more points,\n");
	printf("which is calculated to be how many players you've won over in the single round.\n");
	printf("You can decide the minimum point reached to win in the game,\n");
	printf("which means that if a player gets not less than this point after a round,\n");
	printf("players will be ranked and the game will end.\n");
	printf("You may also set that as zero, which means that a single round will determine the winner.\n");
	printf("Nevertheless, if a couple of players get the same points when ranked,\n");
	printf("to avoid ties, players in a tie will be given one more round to compete.\n");\
	printf("If there are still ties after the additional round, another additional round will be given.\n");
	printf("Players that are not included in the additional round will not be reranked,\n");
	printf("though other players' points may get over them.\n");
	printf("After each game, the credit of players will be changed accoreding to the rank in the game.\n");
	printf("You will see the rooms of players here and you can choose one to join.\n");
	printf("Of course, if you are not satisfied with the modes the current rooms are adopting,\n");
	printf("You can create a new room and decide your prefered mode.\n");
	printf("That's all. Wish you a happy playing experience.\n");
}

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