/*Game program of server*/
#include <stdio.h>

/*Macro definitions for status of one player over another*/
#define DRAW 0
#define WIN 1
#define LOSE 2

/*Macro definitions for standard strings*/
#define ROCK 'r'+'o'+'c'+'k'
#define PAPER 'p'+'a'+'p'+'e'+'r'
#define SCISSORS 's'+'c'+'i'+'s'+'s'+'o'+'r'+'s'

/*New types*/
typedef enum choice {rock=1,paper=2,scissors=3} ch;/*Type for choices of each round*/
typedef unsigned short lazy;/*Type for writing convenience considering player number and points*/

/*Struct for player information*/
typedef struct player{
	char username[50];/*username*/
	lazy point_player;/*temporary point*/
	int credit;/*credit for the player in the program*/
	ch round;/*the choice of this round*/
} player;

/*function to decide who's going to win over another*/
//return value coordinates with macro definitions
int compare(ch u,ch d){
	int boo=(int)u-(int)d+3;
	return boo%3;
}

/*function to get the player's choice of the round*/
ch read(){
	char buffer[9];
	while (1){
		short sum=0,i=0;
		gets(buffer);
		fflush(stdin);
		/*Doing the upper-lower transition*/
		for (;i<9;i++){
			char cTemp=*(buffer+i);
			(cTemp<'a' && cTemp>='A')?(cTemp+=32):1;
			(cTemp>='a' && cTemp<='z')?sum+=cTemp:1;
		}
		/*Deciding the returning value*/
		switch(sum){
		case (ROCK):return rock;
		case (SCISSORS):return scissors;
		case (PAPER):return paper;
		default:printf("Wrong input. Try again:");
		}
	}
}

/*function to refresh the player struct's round value to allow comparison*/
char save_read(player playing[],lazy player_num){
	lazy i=0;
	for (;i<player_num;i++){
		printf("%s's turn: ",(playing+i)->username);
		(playing+i)->round=read();
	}
	return 0;
}

/*function to arrange players according to their points from highest to lowest*/
lazy arrange(player playing[],lazy player_num){
	lazy i,j,same_point_num=0;
	/*Exchange to coodinate with the order*/
	for (i=0;i<player_num-1;i++){
		for (j=player_num-1;j>i;j--){
			if ((playing+j-1)->point_player<(playing+j)->point_player){
				player plTemp=*(playing+j-1);
				*(playing+j-1)=*(playing+j);
				*(playing+j)=plTemp;
			}
		}
	}
	/*Returning how many ties are in the game*/
	for (i=0;i<player_num-1;i++){
		if (i==0 || (playing+i-1)->point_player!=(playing+i)->point_player){
			if ((playing+i)->point_player==(playing+i+1)->point_player)
				same_point_num++;
		}
	}
	return same_point_num;
}

/*function to calculate how many points will a player get after a round*/
char process_point(player playing[],lazy player_num){
	lazy i,j;
	for (i=0;i<player_num;i++){
		for (j=0;j<player_num;j++)
			(playing+i)->point_player+=((compare((playing+i)->round,(playing+j)->round))==WIN);
	}
	return 0;
}

/*function to make the output after each round to show the points of every player*/
char print_point(player playing[],lazy player_num){
	lazy i=0;
	for (i=0;i<player_num;i++)
		printf("%s's temporary point: %hu\n",(playing+i)->username,(playing+i)->point_player);
	printf("\n");
	return 0;
}

/*function to make the output after each round to show the choices of every player*/
lazy print_choice(player playing[],lazy player_num){
	lazy i=0;
	printf("\n");
	for (i=0;i<player_num;i++){
		switch((playing+i)->round){
			case 1: printf("%s's choice: %s\n",(playing+i)->username,"rock");break;
			case 2: printf("%s's choice: %s\n",(playing+i)->username,"paper");break;
			case 3: printf("%s's choice: %s\n",(playing+i)->username,"scissors");break;
		}
	}
	printf("\n");
	return 0;
}

/*function to find the maximum of points after each round*/
lazy find_max(player playing[],lazy player_num){
	lazy maxpoint=0,i=0;
	for (;i<player_num;i++){
		if (((playing+i)->point_player)>maxpoint)
			maxpoint=((playing+i)->point_player);
	}
	return maxpoint;
}

/*indirect recursion*/
char judge_end(player[],lazy,lazy);

/*funtion of offering additional round to play to process ties*/
char sudden(player playing[],lazy player_num){
	lazy i=1,upperpoint=0,repetition;
	printf("%s",playing->username);
	for (;i<player_num;i++)
		printf(", %s",(playing+i)->username);
	printf(" are in a tie. Please compete again.\n");
	save_read(playing,player_num);
	process_point(playing,player_num);
	print_choice(playing,player_num);
	print_point(playing,player_num);
	upperpoint=find_max(playing,player_num);
	printf("\n");
	repetition=arrange(playing,player_num);
	judge_end(playing,player_num,upperpoint,repetition);
	return 0;
}

/*function to check ties and introducing addtional round*/
char judge_end(player playing[],lazy player_num,lazy upperpoint,lazy repetition){
	if (repetition){
		lazy i=0,rank_temp=0,same_temp=0;
		for (;i<player_num;i++){
			if ((playing+i)->point_player==(playing+i+1)->point_player)
				same_temp++;
			else if (same_temp){
				sudden(playing+rank_temp,same_temp+1);
				same_temp=0;
			}
			else
				rank_temp=i+1;
		}
	}
	return 0;
}

/*main function of the game*/
char play(player playing[],lazy player_num,lazy max_point){
	lazy upperpoint=0,i,repetition;
	int rank_standard[50];
	printf("\nGame starts.\n\n");
	/*regular game processing*/
	while (upperpoint<max_point){
		save_read(playing,player_num);
		process_point(playing,player_num);
		upperpoint=find_max(playing,player_num);
		print_choice(playing,player_num);
		print_point(playing,player_num);
	}
	/*check the end and offer additional rounds*/
	repetition=arrange(playing,player_num);
	judge_end(playing,player_num,upperpoint,repetition);
	/*output the ranks of players and calculate credits*/
	if (player_num%2){/*odd players*/
		int lstd=-(((int)player_num)/2),l=-lstd,*rk=rank_standard;
		for (;l>=player_num/2;l--)
			*(rk++)=l;
	}
	else{/*even players*/
		int lstd=((int)player_num)/2,l=lstd,*rk=rank_standard;
		for (;l>0;l--)
			*(rk++)=l;
		for (l--;l>=-lstd;l--)
			*(rk++)=l;
	}
	printf("\nResult:\n");
	for (i=0;i<player_num;i++){/*save and output*/
		(playing+i)->credit+=*(rank_standard+i);
		printf("Rank %hu: %s now credit:%d\n",i+1,(playing+i)->username,(playing+i)->credit);
	}
	return 0;
}

void main(){
	lazy player_num,max_point,i;
	player playing[50];
	printf("player_num:");
	scanf("%hu",&player_num);
	printf("max_point:");
	scanf("%hu",&max_point);
	getchar();
	for (i=0;i<player_num;i++){
		playing[i].point_player=0;
		playing[i].credit=0;
		printf("Name of No.%hu:",i+1);
		gets(playing[i].username);
		printf("Credit of %s:",playing[i].username);
		scanf("%d",playing[i].credit);
		fflush(stdin);
	}
	play(playing,player_num,max_point);
}