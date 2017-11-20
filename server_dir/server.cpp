#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include <iostream>
#include "users.h"
#include <pthread.h>
#include <unistd.h>
#include <errno.h>


//networks
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

int PORT = 41046;
int MAX_PENDING = 5;
int MAXLINE = 256;
int NUM_THREADS = 0;

struct thread_args {
  int s;
  int client_s;
  user_list reg_users;
};

void *client_interact(void *ptr);

//**********************************************/
//**********************************************/
//**********************************************/
//**********************************************/

int main() {
  struct sockaddr_in sin;
  int s, client_s, opt;
  unsigned int len;
  user_list reg_users;



  //build address data structure
  bzero(&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(PORT);

  //set up passive open
  if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    perror("ERROR creating socket");
    exit(1);
  }

  //setup socket option
  if((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)& opt, sizeof(int))) < 0){
    perror("ERROR creating set socket option");
    exit(1);
  }

  //bind created socket to specified address
  if((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("ERROR binding socket");
    exit(1);
  }

  //listening
  if((listen(s, MAX_PENDING)) < 0){
    perror("ERROR listenign");
    exit(1);
  }
  cout << "waiting for connection\n" << endl;
  //wait for connection
  while((client_s = accept(s, (struct sockaddr *)&sin, &len)) > 0){

    if(NUM_THREADS == 10) {
      cout << "Connection refused: Max clients online.\n";
      continue;
    }
    cout << "Connection accepted.\n";
    //load registered users
    if(reg_users.read_from_file() == 1)
      cout << "No users registered\n";

    pthread_t thread;
    thread_args args;
    args.s = s;
    args.client_s = client_s;
    args.reg_users = reg_users;

    NUM_THREADS++;
    if(pthread_create(&thread, NULL, client_interact, &args) < 0) {
      perror("Error creating thread\n");
      exit(1);
    }
  }
  close(client_s);


}

//**********************************************/
//**********************************************/
//Helper functions
//**********************************************/
//**********************************************/
void *client_interact(void *ptr) {
  thread_args *data;
  data = (thread_args *) ptr;
  int s = data->s;
  int client_s = data->client_s;
  user_list reg_users = data->reg_users;

  char buff[MAXLINE], msg[MAXLINE];
  unsigned int len;
  string username, password;




  //receive username
  if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
    perror("Server recieve error");
    exit(1);
  }
  if(len==0)
    exit(1);
  username = buff;
  cout << "username trying to connect: " << username << endl;
  //check whether username already registered
  if(reg_users.search(username) == 0){ //if exists ask for password
    cout << "user registered. asking for password\n";
    bzero((char *)&msg, sizeof(msg));
    strcat(msg, "username exists. enter password: \n");
    //send password request
    if(send(client_s, msg, strlen(msg), 0) == -1){
      perror("Server send error\n");
      exit(1);
    }
    //receive password
    if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
      perror("Server recieve error");
      exit(1);
    }
    if(len==0)
      exit(1);
    password = buff;
    while(reg_users.validate_user(username, password) == 1) { //password incorrect
      //reset msg buffer and request password again
      bzero((char *)&msg, sizeof(msg));
      strcat(msg,"password incorrect. try again: \n");
      cout << msg << endl;
      if(send(client_s, msg, strlen(msg), 0) == -1){
        perror("Server send error\n");
        exit(1);
      }

      if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
        perror("Server recieve error");
        exit(1);
      }
      if(len==0)
        exit(1);
      password = buff;
    }
  } else { //if user doesnt exist, register user
    cout << "registering user\n";
    bzero((char *)&msg, sizeof(msg));
    strcat(msg, "username doesn't exist. enter a password to register: \n");
    //send password request
    if(send(client_s, msg, strlen(msg), 0) == -1){
      perror("Server send error\n");
      exit(1);
    }
    //receive password
    if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
      perror("Server recieve error");
      exit(1);
    }
    if(len==0)
      exit(1);
    password = buff;
    reg_users.add_user(username, password);
    //cout << "user logged in\n";


  }
  cout << "user logged in\n";
  //send ack to client
  bzero((char *)&msg, sizeof(msg));
  strcat(msg, "ACK");
  if(send(client_s, msg, strlen(msg), 0) == -1){
    perror("Server send error\n");
    exit(1);
  }
  //update reg_users file
  reg_users.write_to_file();



}
