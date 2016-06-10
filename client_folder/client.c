#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *msg)
{
    perror(msg);
    exit(0);
}
 
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
 
    struct sockaddr_in serv_addr;
    struct hostent *server;
 
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
 
    /* create socket, get sockfd handle */
 
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
 
    /* fill in server address in sockaddr_in datastructure */
 
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
 
    /* connect to server */
 
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    int check_readfile = 0; //check_readfile == 1 when client writes "getf"
    char filename[100];
 
    while(1){
        /* ask user for input */
        
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
     
        /* send user message to server */
     
        n = write(sockfd,buffer,strlen(buffer));
        if(buffer[0]=='g'&&buffer[1]=='e'&&buffer[2]=='t'&&buffer[3]=='f'){     //implementing getf (getfile)
            check_readfile = 1;
            int bufflen = strlen(buffer);

            int q;
            for(q=0; q<bufflen-6; ++q){
                filename[q] = buffer[q+5];                
            }
            filename[bufflen-6] = '\0';

        }

        if (n < 0) 
             error("ERROR writing to socket");
        bzero(buffer,256);
     
        /* read reply from server */
     
        n = read(sockfd,buffer,255);
    
        if (n < 0) 
             error("ERROR reading from socket");


        if(check_readfile==1){
            /* Create file where data will be stored*/
            FILE *fp;
            fp = fopen(filename,"w");
            if(NULL == fp){
                printf("Error opening file\n");
                return 1;   
            }

            while(n>0){
                printf("Data recieved\n");
                fprintf(fp, "%s", buffer);
                bzero(buffer,256);
                n = read(sockfd,buffer,255);
            }
            fclose(fp);

            if(n<0)
                printf("Read error\n");
        }

    }
    close(sockfd);
    return 0;
}