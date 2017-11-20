#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include "users.h"

//networks
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

int PORT = 41046;
int MAX_PENDING = 5;
int MAXLINE = 256;





int main() {
  struct sockaddr_in sin;
  char buff[MAXLINE], msg[MAXLINE], path[MAXLINE];
  int s, new_s, opt;
  unsigned int len;
  string username, password;
  user_list reg_users;

  //load registered users
  if(reg_users.read_from_file() == 1)
    cout << "No users registered\n";

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
  //listening
  if((listen(s, MAX_PENDING)) < 0){
    perror("ERROR listenign");
    exit(1);
  }
  cout << "waiting for connection\n" << endl;
  //wait for connection
  while(1){
    if((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0){
      perror("ERROR accpeting connection");
      exit(1);
    }

    //receive username
    if((len = recv(new_s, buff, sizeof(buff), 0)) == -1){
      perror("Server recieve error");
      exit(1);
    }
    if(len==0)
      break;
    username = buff;
    cout << "username trying to connect: " << username << endl;
    //check whether username already registered
    if(reg_users.search(username) == 0){ //if exists ask for password
      cout << "user registered. asking for password\n";
      bzero((char *)&msg, sizeof(msg));
      strcat(msg, "username exists. enter password: \n");
      //send password request
      if(send(new_s, msg, strlen(msg), 0) == -1){
        perror("Server send error\n");
        exit(1);
      }
      //receive password
      if((len = recv(new_s, buff, sizeof(buff), 0)) == -1){
        perror("Server recieve error");
        exit(1);
      }
      if(len==0)
        break;
      password = buff;
      while(reg_users.validate_user(username, password) == 1) { //password incorrect
        //reset msg buffer and request password again
        bzero((char *)&msg, sizeof(msg));
        strcat(msg,"password incorrect. try again: \n");
        cout << msg << endl;
        if(send(new_s, msg, strlen(msg), 0) == -1){
          perror("Server send error\n");
          exit(1);
        }

        if((len = recv(new_s, buff, sizeof(buff), 0)) == -1){
          perror("Server recieve error");
          exit(1);
        }
        if(len==0)
          break;
        password = buff;
      }
    } else { //if user doesnt exist, register user
      cout << "registering user\n";
      bzero((char *)&msg, sizeof(msg));
      strcat(msg, "username doesn't exist. enter a password to register: \n");
      //send password request
      if(send(new_s, msg, strlen(msg), 0) == -1){
        perror("Server send error\n");
        exit(1);
      }
      //receive password
      if((len = recv(new_s, buff, sizeof(buff), 0)) == -1){
        perror("Server recieve error");
        exit(1);
      }
      if(len==0)
        break;
      password = buff;
      reg_users.add_user(username, password);
      //cout << "user logged in\n";


    }
    cout << "user logged in\n";
    //send ack to client
    bzero((char *)&msg, sizeof(msg));
    strcat(msg, "ACK");
    if(send(new_s, msg, strlen(msg), 0) == -1){
      perror("Server send error\n");
      exit(1);
    }

  }


}
