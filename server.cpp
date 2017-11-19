#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
//#include <vector>
//#include <utility>

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
  //users
  //vector<pair<string, string> > users;

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

    while(1) {
      if((len = recv(new_s, buff, sizeof(buff), 0)) == -1){
        perror("Server recieve error");
        exit(1);
      }
      if(len==0)
        break;
      cout << "connected to " << buff << endl;
    }
  }


}
