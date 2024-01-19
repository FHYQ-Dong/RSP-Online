#include <stdio.h>
#include <stdlib.h>

#define DRAW 0
#define WIN 1
#define LOSE 2

#define ROCK 'r'+'o'+'c'+'k'
#define PAPER 'p'+'a'+'p'+'e'+'r'
#define SCISSORS 's'+'c'+'i'+'s'+'s'+'o'+'r'+'s'

typedef enum choice{rock=1,paper=2,scissors=3}ch;
typedef unsigned short lazy;

typedef struct player{
	char username[50];
	lazy score;
	ch round;
} player;

int compare(ch u,ch d){
	int boo=(int)u-(int)d+3;
	return boo%3;
}

ch read(){
	char buffer[9];
	while (1){
		short sum=0,i=0;
		gets(buffer);
		fflush(stdin);
		for (;i<9;i++){
			char cTemp=*(buffer+i);
			(cTemp<'a' && cTemp>='A')?(cTemp+=32):1;
			(cTemp>='a' && cTemp<='z')?sum+=cTemp:1;
		}
		switch(sum){
		case (ROCK):return rock;
		case (SCISSORS):return scissors;
		case (PAPER):return paper;
		default:printf("Wrong input. Try again:");
		}
	}
}

char save_read(player playing[],lazy player_num){
	lazy i=0;
	for (;i<player_num;i++){
		printf("%s's turn: ",(playing+i)->username);
		(playing+i)->round=read();
	}
	return 0;
}

lazy arrange(player playing[],lazy player_num){
	lazy i,j,same_score_num=0;
	/*Exchange*/
	for (i=0;i<player_num-1;i++){
		for (j=player_num-1;j>i;j--){
			if ((playing+j-1)->score<(playing+j)->score){
				player plTemp=*(playing+j-1);
				*(playing+j-1)=*(playing+j);
				*(playing+j)=plTemp;
			}
		}
	}
	/*Repeted times*/
	for (i=0;i<player_num-1;i++){
		if (i==0 || (playing+i-1)->score!=(playing+i)->score){
			if ((playing+i)->score==(playing+i+1)->score)
				same_score_num++;
		}
	}
	return same_score_num;
}

char process_score(player playing[],lazy player_num){
	lazy i,j;
	for (i=0;i<player_num;i++){
		for (j=0;j<player_num;j++)
			(playing+i)->score+=((compare((playing+i)->round,(playing+j)->round))==WIN);
	}
	return 0;
}

char print_score(player playing[],lazy player_num){
	lazy i=0;
	for (i=0;i<player_num;i++)
		printf("%s's temporary score: %hu\n",(playing+i)->username,(playing+i)->score);
	printf("\n");
	return 0;
}

lazy find_max(player playing[],lazy player_num){
	lazy maxscore=0,i=0;
	for (;i<player_num;i++){
		if (((playing+i)->score)>maxscore)
			maxscore=((playing+i)->score);
	}
	return maxscore;
}

char judge_end(player[],lazy,lazy);

char sudden(player playing[],lazy player_num){
	lazy i=1,upperscore=0;
	printf("%s",playing->username);
	for (;i<player_num;i++)
		printf(", %s",(playing+i)->username);
	printf(" are in a draw. Please compete again.\n");
	save_read(playing,player_num);
	process_score(playing,player_num);
	upperscore=find_max(playing,player_num);
	printf("\n");
	judge_end(playing,player_num,upperscore);
	return 0;
}

char judge_end(player playing[],lazy player_num,lazy upperscore){
	lazy repetition=arrange(playing,player_num);
	if (repetition){
		lazy i=0,rank_temp=0,same_temp=0;
		for (;i<player_num;i++){
			if ((playing+i)->score==(playing+i+1)->score)
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

char play(player playing[],lazy player_num,lazy max_score){
	lazy upperscore=0,i=0;
	printf("\nGame starts.\n\n");
	while (upperscore<max_score){
		save_read(playing,player_num);
		process_score(playing,player_num);
		upperscore=find_max(playing,player_num);
		print_score(playing,player_num);
	}
	judge_end(playing,player_num,upperscore);
	printf("\nResult:\n");
	for (;i<player_num;i++)
		printf("Rank %hu: %s\n",i+1,(playing+i)->username);
	return 0;
}

void main(){
	lazy player_num,max_score,i;
	player playing[50];
	printf("player_num:");
	scanf("%hu",&player_num);
	printf("max_score:");
	scanf("%hu",&max_score);
	getchar();
	for (i=0;i<player_num;i++){
		playing[i].score=0;
		printf("Name of No.%hu:",i+1);
		gets(playing[i].username);
	}
	play(playing,player_num,max_score);
	system("pause");
}