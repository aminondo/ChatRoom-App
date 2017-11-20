#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


//networks
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 41046
#define MAXLINE 256
int len, s;
bool ACTIVE = 1;

using namespace std;

void *handle_messages(void *);


int main(int argc, char *argv[]) {
  struct hostent *hp;
  struct sockaddr_in sin;
  char * host, *user;
  string username;
  int port;
  char buff[MAXLINE], op[MAXLINE], password[MAXLINE];

  //string buff, msg;

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
    cout << "password was incorrect.trying again\n";
    cout << buff << endl;
    cin >> password;
    cout << password;
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

  pthread_t thread;
  int rc = pthread_create(&thread, NULL, handle_messages, NULL);
  if(rc) {
    cout << "ERROR: unable to create thread\n";
    exit(-1);
  }

  while(1){
    //prompt user for operation
    //pthread_join(thread, NULL);
    cout << "press B for broadcasting.\n";
    cout << "press P for private messaging.\n";
    cout << "press E for exit.\n";
    cout << ">> ";
    bzero((char *)&op, sizeof(op));
    cin >> op;
    if(!strncmp(op, "E", 1)){ //quit command
      cout << "Goodbye!\n";
      //send server E command so that server removes user from online list
      if(send(s, op, strlen(op), 0 ) == -1){
        perror("Client send error\n");
        exit(1);
      }
      ACTIVE = 0;
      break;
    }
    if(!strncmp(op, "P", 1)){ //private message
      //private_message();
      cout << "Sending private message!\n";
      //send request for active users
      if(send(s, op, strlen(op), 0 ) == -1){
        perror("Client send error\n");
        exit(1);
      }
      //cout << "sent request for active users\n";
      //recieve active users, handled by separate thread
      cout << "select user: ";
      sleep(1);
      cout << "\n>> ";
      bzero((char *)&buff, sizeof(buff));
      cin >> buff;
      if(send(s, buff, strlen(buff), 0 ) == -1){
        perror("Client send error\n");
        exit(1);
      }
      //send message back to server
      cout << "write message: \n";
      cout << ">> ";
      bzero((char *)&buff, sizeof(buff));
      cin >> buff;
      if(send(s, buff, strlen(buff), 0 ) == -1){
        perror("Client send error\n");
        exit(1);
      }


    }
    if(!strncmp(op, "B", 1)){ //broadcast message
      //send operation to server
      if(send(s, op, strlen(op), 0 ) == -1){
        perror("Client send error\n");
        exit(1);
      }
      //ask for message to broadcast
      cout << "write message: \n";
      cout << ">> ";
      bzero((char *)&buff, sizeof(buff));
      cin >> buff;
      if(send(s, buff, strlen(buff), 0 ) == -1){
        perror("Client send error\n");
        exit(1);
      }

    }
  }

  close(s);
  return 0;
}

void *handle_messages(void *) {
  char buff[MAXLINE];
  //bzero((char *)&op, sizeof(op));
  while(ACTIVE) {
    if((len = recv(s, buff, sizeof(buff), 0)) == -1){
      perror("Client receive error\n");
      exit(1);
    }
    //string message;
    //data message
    //cout << buff[0];
    if (!strncmp(buff, "D", 1)){
      cout << "receiving...\n";
      string tmp(buff);
      tmp.erase(0,2);
      cout << "\n\n************************************\n";
      cout << tmp << endl;
      cout << "************************************\n\n";

      cout << "press B for broadcasting.\n";
      cout << "press P for private messaging.\n";
      cout << "press E for exit.\n";
      cout << ">> ";



    }else {
      string tmp(buff);
      tmp.erase(0, 2);
      //handle command message
      //if first element is C
      //cout << "\ncommand message\n";
      cout << tmp << endl;
    }


  }

  //exit(1);

}
