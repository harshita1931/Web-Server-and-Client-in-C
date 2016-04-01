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

    int check=0;    //check == 1 when client writes "bye" 
    int check_readfile = 0; //check_readfile == 1 when client writes "getf"
    char filename[100];
 
    while(1){
        /* ask user for input */
        
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
     
        /* send user message to server */
     
        n = write(sockfd,buffer,strlen(buffer));
        if(buffer[0]=='b'&& buffer[1]=='y'&& buffer[2]=='e')    //implementing "bye"
            check=1;
        if(buffer[0]=='g'&&buffer[1]=='e'&&buffer[2]=='t'&&buffer[3]=='f'){     //implementing getf (getfile)
            check_readfile = 1;
            int bufflen = strlen(buffer);
                    printf("%d\n",bufflen );
            //filename[bufflen-5];

                    printf("hello1\n");
            int q;
            for(q=0; q<bufflen-6; ++q){
               //     printf("%d\n",q );
                filename[q] = buffer[q+5];                
            }
                    printf("hello2\n");
            filename[bufflen-6] = '\0';
                    printf("%s\n",filename );

        }

        if (n < 0) 
             error("ERROR writing to socket");
        bzero(buffer,256);
     
        /* read reply from server */
     
        n = read(sockfd,buffer,255);
        printf("%s\n", buffer );
        if (n < 0) 
             error("ERROR reading from socket");
        //printf("%s\n",buffer);

        if(check==1)
            break;

        if(check_readfile==1){
            /* Create file where data will be stored*/
            FILE *fp;
                printf("hello3\n");
            fp = fopen(filename,"w");
                printf("hello4\n");
            if(NULL == fp){
                printf("Error opening file\n");
                return 1;   //see
            }
                printf("hello5\n");

            while(n>0){
                printf("Data recieved\n");
            //    fwrite(buffer,1,n,fp);
                fprintf(fp, "%s", buffer);
                    printf("%s\n",buffer);
                    printf("hello5.5\n");
                bzero(buffer,256);
                n = read(sockfd,buffer,255);
                    printf("hello5.6\n");
            }
                printf("hello6\n");

            if(n<0)
                printf("Read error\n");
        }

    }
    close(sockfd);
    return 0;
}