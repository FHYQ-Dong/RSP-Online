#include "../include/account.h"

AccountPool::AccountPool() {
    accounts = std::map<char*, Account>();
}

ACCOUNT_TYPE AccountPool::add_account(Account account) {
    if (accounts.find(account.username) != accounts.end()) {
        return ACCOUNT_ALREADY_EXIST;
    }
    accounts[account.username] = account;
    return ACCOUNT_CORRECT;
}

ACCOUNT_TYPE AccountPool::check_account(Account account) {
    if (accounts.find(account.username) == accounts.end())
        return ACCOUNT_NOT_EXIST;
    if (memcmp(accounts[account.username].passwd_hash, account.passwd_hash, SHA256_BLOCK_SIZE) != 0)
        return ACCOUNT_WRONG_PASSWD;
    return ACCOUNT_CORRECT;
}

int AccountPool::load_from_file(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return FILE_OPEN_ERROR;
    }
    char username[MAX_USERNAME_LENGTH];
    unsigned char passwd_hash[SHA256_BLOCK_SIZE];
    while (fscanf(fp, "%s %s", username, passwd_hash) != EOF) {
        Account account;
        strcpy(account.username, username);
        memcpy(account.passwd_hash, passwd_hash, SHA256_BLOCK_SIZE);
        accounts[username] = account;
    }
    fclose(fp);
    return FILE_OK;
}

int AccountPool::save_to_file(char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return FILE_OPEN_ERROR;
    }
    for (auto it = accounts.begin(); it != accounts.end(); it++) {
        fprintf(fp, "%s %s\n", it->second.username, it->second.passwd_hash);
    }
    fclose(fp);
    return FILE_OK;
}