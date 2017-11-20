#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


//networks
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 41046
#define MAXLINE 256

using namespace std;

int main(int argc, char *argv[]) {
  struct hostent *hp;
  struct sockaddr_in sin;
  char * host, *user;
  string username;
  int port;
  char buff[MAXLINE], msg[MAXLINE], password[MAXLINE];

  //string buff, msg;
  int len, s;

  //check arguments
  if(argc == 4){
    host = argv[1]; //host
    port = atoi(argv[2]); //port
    user = argv[3]; //user
  } else {
    fprintf(stderr, "ERROR: error in arguments provided");
    exit(1);
  }

  //translate host name into ip
  hp = gethostbyname(host);
  if(!hp) {
    fprintf(stderr, "ERROR: unknown host\n");
    exit(1);
  }

  //build address data structure
  bzero(&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, &sin.sin_addr, hp->h_length);
  sin.sin_port = htons(port);

  //create socket
  if( (s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    perror("ERROR creating socket\n");
    exit(1);
  }
  cout << "Welcome\n" << endl;

  //connect created socket to remote server
  if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
    perror("Error connnecting socket\n");
    exit(1);
  }

  //send username to server
  if(send(s, user, strlen(user), 0 ) == -1){
    perror("Client send error\n");
    exit(1);
  }
  //recieve request to register or log in, will continue to loop until ack is recieved
  if((len = recv(s, buff, sizeof(buff), 0)) == -1){
    perror("Client receive error\n");
    exit(1);
  }
  cout << buff << endl;
  //I know that what is being requested is the password, no matter if user
  //registers or logs in
  cin >> password;
  if(send(s, password, strlen(password), 0 ) == -1){
    perror("Client send error\n");
    exit(1);
  }
  //recieve ack or error
  if((len = recv(s, buff, sizeof(buff), 0)) == -1){
    perror("Client receive error\n");
    exit(1);
  }
  while(strncmp(buff, "ACK", 3)){
    cout << buff << endl;
    cin >> password;
    if(send(s, password, strlen(password), 0 ) == -1){
      perror("Client send error\n");
      exit(1);
    }
    //recieve ack or error
    if((len = recv(s, buff, sizeof(buff), 0)) == -1){
      perror("Client receive error\n");
      exit(1);
    }
  }
  //logged in
  cout << "LOGGED IN!\n";



}