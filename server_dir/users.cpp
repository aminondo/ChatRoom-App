#include "users.h"
using namespace std;



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

//O(n)
int user_list::search(string username){
  struct User *tmp = head;
  while( tmp != NULL){
    if(tmp->username == username)
      return 0; //user found
    tmp = tmp->nxt;
  }
  return 1; //user not found
}

int user_list::write_to_file(string username, string password){
  ofstream myfile;
  myfile.open("users.txt", ios_base::app);
  struct User *tmp = head;
  myfile << username << " " << password << "\n";

  //myfile.close();
  return 0;
}

int user_list::validate_user(string username, string password){
  struct User *tmp = head;
  while( tmp != NULL){
    if(tmp->username == username && tmp->password == password)
      return 0; //user found
    tmp = tmp->nxt;
  }
  return 1; //user not found
}

int user_list::read_from_file(){
  //struct User *tmp;
  head = new User();
  string user, pass;
  string line;
  ifstream myfile("users.txt");
  if(myfile.is_open()){
    while( getline(myfile, line) ){
      istringstream iss(line);
      iss >> user;
      iss >> pass;
      add_user(user, pass);
    }
    myfile.close();
    return 0;
  }
  return 1;
}

/*

int main(){
  user_list users;// = user_list();
  users.add_user("alice", "password");
  //cout << users.read_from_file() << endl;
  users.print_users();
  //users.add_user("antonoi", "fasdaf");
  cout << users.validate_user("alice", "password") << endl;
  //cout << users.search("alice") << endl;
  //users.write_to_file();
}
*/
