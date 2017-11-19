#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;


struct User {
    string username;
    string password;
    struct User *nxt;
    //User(string user, string pass);
};
/*
User::User(string user, string pass){
  username = user;
  password = pass;
}*/

class user_list {
  public:
    User *head;
    user_list() { head = NULL; }
    void add_user(string user, string pass);
    void print_users();
};

void user_list::add_user(string user, string pass){
  struct User *tmp, *p;
  tmp = new(struct User);
  tmp->username = user;
  tmp->password = pass;
  if(head == NULL) {
    head = tmp;
    head->nxt = NULL;
  } else {
    p = head;
    head = tmp;
    head->nxt = p;
  }
}

void user_list::print_users(){
  struct User *tmp = head;

  while(tmp != NULL){
    cout << tmp->username << " " << tmp->password << endl;
    tmp = tmp->nxt;
  }
}


int main(){
  user_list users;// = user_list();
  users.add_user("alice", "password");
  users.add_user("bernie", "asdfa");
  users.print_users();
}
