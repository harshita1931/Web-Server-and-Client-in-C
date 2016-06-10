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
#include <arpa/inet.h>


#define clientnum 30 

void error(char *msg)
{
    perror(msg);
    exit(1);              
}
 
int main(int argc, char *argv[])
{
     int sockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     char filename[100];
    
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


    int newsockfd, childrenPID[clientnum];
      
     /* listen for incoming connection requests */

    listen(sockfd,clientnum);                  //sockfd is the listening socket
    clilen = sizeof(cli_addr);

    while(1){
 
     /* accept a new request, create a newsockfd */


     int j;
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     for(j=0; j<clientnum; ++j){
        childrenPID[j] = -1;
     }
    
    if(newsockfd>0)    {            //conection is made

        pid_t childPID = fork();


        //if in child
        if(childPID==0){
             /* read message from client */
            while(1){
                bzero(buffer,256);    
                n = read(newsockfd,buffer,255);
                if(n>=0){
                    printf("Here is the message: %s\n",buffer);

                      //file serving
                      if(buffer[0]=='g'&&buffer[1]=='e'&&buffer[2]=='t'&&buffer[3]=='f'){
                        int bufflen = strlen(buffer);
                        //filename[bufflen-5];
                        int q;
                        for(q=0; q<bufflen-6; ++q){
                            filename[q] = buffer[q+5];                
                        }
                        filename[bufflen-6] = '\0';

                        FILE *fp = fopen(filename,"rb");
                        if(fp == NULL){
                            printf("File open error\n");
                            return 1;       //see it
                        }

                        int sendbuff_size = 256;    //read and write to file in chunks of 256 chars
                        char sendbuff[sendbuff_size];
                        bzero(sendbuff, sendbuff_size);
                        int curr = 0;
                        char ch;
                        while((ch = fgetc(fp))!=EOF){
                            if(curr == sendbuff_size){
                                n = write(newsockfd, sendbuff, sendbuff_size);
                                bzero(sendbuff, sendbuff_size);
                                if(n<0){
                                    error("ERROR writing to socket");
                                    exit(1);
                                }
                                curr = 0;
                            }
                            sendbuff[curr] = ch;
                            curr++;
                        }
                        if(curr!=0){
                            n =write(newsockfd, sendbuff, sendbuff_size);
                            bzero(sendbuff, sendbuff_size);
                            if(n<0){
                                error("ERROR writing to socket");
                                exit(1);
                            }

                        }
                        fclose(fp);
                        close(newsockfd);

                      }

                      else{
                         n = write(newsockfd,"I got your message",18);
                      }
                       
                }
                }
        }

         //if in parent
         else{
            close(newsockfd);
                    //yha wala part and reaping ka code likhna h abhi
         }   

     }  


 }
    close(sockfd);
     return 0; 
}