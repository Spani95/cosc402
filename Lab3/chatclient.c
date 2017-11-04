#include	"unp.h"

#define BUFF_SIZE 10000 

void chatUser(char *hostname, char *aPort);
 
int main (int argc, char *argv[]) 
{ 
  
    if (argc != 3) {
      printf("usage:%s [IP] [PORT] \n" , argv[0]);
      return -1;
     }

    printf("\nStarting chat session: host(%s) port(%s)\n", 
        argv[1], argv[2]);    
    
    chatUser(argv[1], argv[2]);
    
    return 0; 
} 

void chatUser(char *hostname, char *aPort)
{  
    unsigned char buffer[BUFF_SIZE]; 
    struct sockaddr_in sin; 
    int nread; 
    int fd_chat  = -1; 
    int maxfd, iNumInputs; 
    fd_set fdCheck; 
    
    // Now we need to create a socket connection with the chat server

    if ((fd_chat  = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
         perror("client: socket()"); 
         exit(-1); 
    } 
    else 
    {    
        sin.sin_family = AF_INET; 
        sin.sin_port = htons(strtoul (aPort, NULL, 10)); 
        Inet_pton(AF_INET, hostname, &sin.sin_addr);
          
        if (connect(fd_chat , (struct sockaddr *)&sin, sizeof(sin)) < 0) 
        { 
            perror("client: connect()"); 
            exit(-1); 
        } 
    } 
    
    /* START SKELETON 
     COMPLETE_ME
     *
     * Refer to the chatLoop function in the chatserver.c to implement the code using select to control
      reading data from stdin and the socket.
     * 
     *
    END SKELETON */
    for(; ;) {
        FD_ZERO(&fdCheck);
        FD_SET(fd_chat, &fdCheck);
        FD_SET(STDIN_FILENO, &fdCheck);
        
        if (STDIN_FILENO < fd_chat){
            maxfd = fd_chat;
        } else {
            maxfd = STDIN_FILENO;
        }

        if((iNumInputs = Select(maxfd+1, &fdCheck, NULL, NULL, NULL)) < 0) {
            perror("CLIENT ERROR select()");
            exit(-1);
        }

        if(FD_ISSET(fd_chat, &fdCheck)) {
            if((nread = read(fd_chat, buffer, sizeof buffer)) > 0) {
                buffer[nread]=0;   
                if(write(STDOUT_FILENO, buffer, nread) < 0){
                    perror("CLIENT ERROR write()");
                    exit(-1);
                }
            }
        } else if(FD_ISSET(STDIN_FILENO, &fdCheck)) {
            if((nread = read(STDIN_FILENO, buffer, sizeof buffer)) > 0) {
                if(write(fd_chat, buffer, nread) < 0) {
                    perror("CLIENT ERROR write()");
                    exit(-1);
                }
            }
        }
    }
   
    if (fd_chat  >= 0) 
        close(fd_chat ); 
    exit(0); 

}
