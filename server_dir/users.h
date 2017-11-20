#ifndef USERS_H
#define USERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <pthread.h>

using namespace std;

struct active {
  string username;
  //pthread_t id;
  int client_s;
};

struct User {
    string username;
    string password;
    struct User *nxt;
};

class user_list {
  public:
    User *head;
    user_list() { head = NULL; }
    void add_user(string user, string pass);
    void print_users();
    int search(string username);
    int validate_user(string username, string password);
    int write_to_file(string username, string password);
    int read_from_file();
};

#endif
