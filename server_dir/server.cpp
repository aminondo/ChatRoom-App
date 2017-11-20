/*
Antonio Minondo
aminondo
server.cpp
this file contains the implementation too run a server that can handle
multiple users using concurrency.
*/




#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <cstring>
#include <string.h>
#include <iostream>
#include "users.h"
#include <pthread.h>
//#include <thread>
#include <unistd.h>
#include <errno.h>
#include <vector>

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
  //pthread_t thread_id;
};

//prototypes
void *client_interact(void *ptr);

vector<active> active_users;


int main() {
  struct sockaddr_in sin;
  int s, client_s, opt;
  unsigned int len;
  user_list reg_users;



  //build address data structure
  bzero(&sin, sizeof(sin));
  //memset(sin, '\0', sizeof sin);
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
    //args.client_s = thread;

    NUM_THREADS++;
    if(pthread_create(&thread, NULL, client_interact, &args) < 0) {
      perror("Error creating thread\n");
      exit(1);
    }
    //cout << "printing thread: " << thread << endl;
    //join threads
    //pthread_join(thread, NULL);
  }


  //stop main socket
  close(s);
} //main()

//**************************************************************/
//Helper functions
//**************************************************************/
void *client_interact(void *ptr) {
  thread_args *data;
  data = (thread_args *) ptr;
  int s = data->s;
  int client_s = data->client_s;
  //pthread_t thread_id = data->thread_id;
  user_list reg_users = data->reg_users;

  char buff[MAXLINE], msg[MAXLINE];
  unsigned int len;
  string username, password;

  //receive username
  memset(buff, '\0', sizeof buff);
  if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
    perror("Server recieve error");
    exit(1);
  }
  username = buff;
  cout << "username trying to connect: " << username << endl;
  //check whether username already registered
  if(reg_users.search(username) == 0){ //if exists ask for password
    memset(msg, '\0', sizeof msg);
    strcat(msg, "username exists. enter password: \n");
    //send password request
    if(send(client_s, msg, strlen(msg), 0) == -1){
      perror("Server send error\n");
      exit(1);
    }
    //receive password
    memset(buff, '\0', sizeof buff);
    if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
      perror("Server recieve error");
      exit(1);
    }
    password = buff;
    while(reg_users.validate_user(username, password) == 1) { //password incorrect
      cout << "password incorrect\n";
      //reset msg buffer and request password again
      memset(msg, '\0', sizeof msg);
      strcat(msg,"password incorrect. try again: \n");
      cout << msg << endl;
      if(send(client_s, msg, strlen(msg), 0) == -1){
        perror("Server send error\n");
        exit(1);
      }
      memset(buff, '\0', sizeof buff);
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
    memset(msg, '\0', sizeof msg);
    strcat(msg, "username doesn't exist. enter a password to register: \n");
    //send password request
    if(send(client_s, msg, strlen(msg), 0) == -1){
      perror("Server send error\n");
      exit(1);
    }
    //receive password
    memset(buff, '\0', sizeof buff);

    if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
      perror("Server recieve error");
      exit(1);
    }
    password = buff;
    cout << password << endl;
    //reg_users.add_user(username, password);
    reg_users.write_to_file(username, password);
    cout << "printing registered users\n";
    reg_users.read_from_file();
    reg_users.print_users();
    //cout << "user logged in\n";


  }
  cout << "user logged in\n";
  //update reg_users file
  //add user to active users
  active tmp = active();
  tmp.username = username;
  tmp.client_s = client_s;
  active_users.push_back(tmp);
  //send ack to client
  memset(msg, '\0', sizeof msg);
  strcat(msg, "ACK");
  if(send(client_s, msg, strlen(msg), 0) == -1){
    perror("Server send error\n");
    exit(1);
  }

  //waiting for command from user

  while(1) {
    //receive command from user
    memset(buff, '\0', sizeof buff);
    if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
      perror("Server recieve error");
      exit(1);
    }
    cout << "command received: " << buff << endl;
    if(!strncmp(buff, "E", 1)){ //if E
      //remove active user from vector
      for(vector<active>::iterator it = active_users.begin(); it != active_users.end(); ++it){
        if ((*it).username == username){
          cout << "removing active user\n";
          active_users.erase(it);
          cout << "removed active user\n";
          break; //stop for loop
        }
      }
      break; //stop while loop
    } else if(!strncmp(buff, "P", 1)){ //if P
      active recvr = active();
      memset(msg, '\0', sizeof msg);
      cout << msg << endl;
      strcat(msg, "C { ");

      for(vector<active>::iterator it = active_users.begin(); it != active_users.end(); ++it){
        strcat(msg, (*it).username.c_str());
        strcat(msg, " ");
      }
      strcat(msg, "}");
      cout << msg << endl;
      //send active users
      if(send(client_s, msg, strlen(msg), 0) == -1){
        perror("Server send error\n");
        exit(1);
      }
      //recieve username for private message
      memset(buff, '\0', sizeof buff);
      if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
        perror("Server recieve error");
        exit(1);
      }
      recvr.username = buff;
      cout << "printing username to private message to " << recvr.username << endl;


      //recieve message
      memset(buff, '\0', sizeof buff);
      if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
        perror("Server recieve error");
        exit(1);
      }

      //check whether username exists, if it exists send message and confirmation
      //back to user, otherwise send failed message back to user
      int found = 0;
      for(vector<active>::iterator it = active_users.begin(); it != active_users.end(); ++it){
        if((*it).username == recvr.username) {
          cout << "user found\n";
          found = 1;
          recvr.client_s = (*it).client_s;
        }
      }

      if(found) {
        //send confirmation back to user
        memset(msg, '\0', sizeof msg);
        strcat(msg, "D message sent successfully\n");
        if(send(client_s, msg, strlen(msg), 0) == -1){
          perror("Server send error\n");
          exit(1);
        }
        //format message and send to receipient
        memset(msg, '\0', sizeof msg);
        strcat(msg, "D ");
        strcat(msg, username.c_str());
        strcat(msg, ": ");
        string tmp(buff);
        cout << tmp << endl;
        strcat(msg, tmp.c_str());
        cout << msg << endl;
        if(send(recvr.client_s, msg, strlen(msg), 0) == -1){
          perror("Server send error\n");
          exit(1);
        }

      } else {
        cout << "user not found\n";
        //send confirmation back to user
        memset(msg, '\0', sizeof msg);
        strcat(msg, "D message send failed\n");
        cout << msg << endl;
        if(send(client_s, msg, strlen(msg), 0) == -1){
          perror("Server send error\n");
          exit(1);
        }
      }

    } else if(!strncmp(buff, "B", 1)){ //if B
      //wait for message
      //recieve message
      memset(buff, '\0', sizeof buff);
      cout << buff;
      if((len = recv(client_s, buff, sizeof(buff), 0)) == -1){
        perror("Server recieve error");
        exit(1);
      }
      cout << buff << endl;
      memset(msg, '\0', sizeof msg);
      strcat(msg, "D Broadcast:");
      strcat(msg, buff);

      //send message to all users
      for(vector<active>::iterator it = active_users.begin(); it != active_users.end(); ++it){
        if(send((*it).client_s, msg, strlen(msg), 0) == -1){
          perror("Server send error\n");
          exit(1);
        }
      }



    } else {
      cout << "command not understood\n";
    }


  }

  cout << "connection closed\n";
  close(client_s);
  //thread_exit();
}
