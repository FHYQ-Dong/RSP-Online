#include "../include/account.h"

Account::Account() {
    memset(username, 0, sizeof(username));
    memset(passwd_hash, 0, sizeof(passwd_hash));
    credit = 0;
}

Account::Account(char u[MAX_USERNAME_LENGTH], unsigned char p[SHA256_BLOCK_SIZE]) {
    strcpy(username, u);
    memcpy(passwd_hash, p, SHA256_BLOCK_SIZE);
    credit = 0;
}

Account::Account(char u[MAX_USERNAME_LENGTH], unsigned char p[SHA256_BLOCK_SIZE], int c) {
    strcpy(username, u);
    memcpy(passwd_hash, p, SHA256_BLOCK_SIZE);
    credit = c;
}

AccountPool::AccountPool(const char* f) {
    accounts = std::map<std::string, Account>();
    filename = f;
    if (load_from_file(filename) == FILE_OPEN_ERROR) 
        printf("No account datafile, start blankly.\n");
}

ACCOUNT_TYPE AccountPool::add_account(Account account) {
    std::string username(account.username);
    if (accounts.find(username) != accounts.end()) {
        return ACCOUNT_ALREADY_EXIST;
    }
    accounts[username] = account;
    save_to_file(filename);
    return ACCOUNT_CORRECT;
}

ACCOUNT_TYPE AccountPool::check_account(Account account) {
    std::string username(account.username);
    if (accounts.find(username) == accounts.end())
        return ACCOUNT_NOT_EXIST;
    if (memcmp(accounts[username].passwd_hash, account.passwd_hash, SHA256_BLOCK_SIZE) != 0)
        return ACCOUNT_WRONG_PASSWD;
    return ACCOUNT_CORRECT;
}

Account AccountPool::get_account(const char* username) {
    std::string u(username);
    return accounts[u];
}

int AccountPool::load_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return FILE_OPEN_ERROR;
    }
    fclose(fp);
    struct stat buf;
    stat(filename, &buf);
    int cnt = buf.st_size / sizeof(Account);
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return FILE_OPEN_ERROR;
    }
    for (int i=0; i<cnt; ++i) {
        Account account;
        fread((char*)&account, sizeof(Account), 1, fp);
        std::string username(account.username);
        accounts[username] = account;
    }
    fclose(fp);
    return FILE_OK;
}

int AccountPool::save_to_file(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return FILE_OPEN_ERROR;
    }
    for (auto it = accounts.begin(); it != accounts.end(); it++) 
        fwrite((char*)&(it->second), sizeof(Account), 1, fp);
    fclose(fp);
    return FILE_OK;
}

void AccountPool::update_account(Account account) {
    std::string username(account.username);
    accounts[username] = account;
    save_to_file(filename);
}