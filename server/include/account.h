// include/account.h

#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
#pragma warning(disable: 4996)

#define MAX_USERNAME_LENGTH 50
#define SHA256_BLOCK_SIZE 32

typedef int LOGIN_TYPE;
#define LOGIN 0
#define REGISTER 1

typedef struct Account {
    char username[MAX_USERNAME_LENGTH];
    unsigned char passwd_hash[SHA256_BLOCK_SIZE];
} Account;

typedef int ACCOUNT_TYPE;
#define ACCOUNT_CORRECT 0
#define ACCOUNT_NOT_EXIST 1
#define ACCOUNT_WRONG_PASSWD 2
#define ACCOUNT_ALREADY_EXIST 3

typedef int FILE_TYPE;
#define FILE_OK 0
#define FILE_OPEN_ERROR 1
typedef struct AccountPool {
    std::map<char*, Account> accounts;
    ACCOUNT_TYPE add_account(Account account);
    ACCOUNT_TYPE check_account(Account account);
    AccountPool();
    FILE_TYPE load_from_file(char* filename);
    FILE_TYPE save_to_file(char* filename);
} AccountPool;