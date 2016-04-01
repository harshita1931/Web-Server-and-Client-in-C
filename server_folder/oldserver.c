/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>


#define clientnum 30 

void error(char *msg)
{
    perror(msg);
    exit(1);              
}
 
int main(int argc, char *argv[])
{
     int sockfd, /*newsockfd,*/ portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
 
     /* create socket */
 
     sockfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);           
     if (sockfd < 0) 
        error("ERROR opening socket");
 
     /* fill in port number to listen on. IP address can be anything (INADDR_ANY) */
 
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
 
     /* bind socket to this port number on this machine */
 
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");


    int newsockfd[clientnum];
    int i;
    for(i=0 ;i<clientnum; ++i)
        newsockfd[i] = -1;
      
     /* listen for incoming connection requests */

    listen(sockfd,clientnum);                  //sockfd is the listening socket
    clilen = sizeof(cli_addr);

    while(1==1){
 
     /* accept a new request, create a newsockfd */


     int j;
    for(j=0; j<clientnum; ++j){ 
     if(newsockfd[j]<0){  
        newsockfd[j] = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd[j]>=0)
            fcntl(newsockfd[j], F_SETFL, O_NONBLOCK);
        break;
      }
    }
    
     /* read message from client */
     
     for(j=0; j<clientnum; ++j){
        if(newsockfd[j]>=0){
            bzero(buffer,256);
            n = read(newsockfd[j],buffer,255);
            if(n>=0){
                printf("Here is the message: %s\n",buffer);
                 /* send reply to client */
                  n = write(newsockfd[j],"I got your message",18);  
                  if(buffer[0]=='b' && buffer[1]=='y' && buffer[2]=='e'){
                    close(newsockfd[j]); 
                    newsockfd[j] = -1; 
                  }            
            }

        }
     } 


 }
    close(sockfd);
     return 0; 
}