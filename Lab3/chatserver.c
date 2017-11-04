#include	"unp.h"

#define NAME_SIZE 40
#define MAX_CHATS 40
#define BUFF_SIZE 100000

#define CHAT_FULL  "Max chats achieved, NO SEAT available :-("
#define WELCOME_MSG "Welcome to chatter: Enter your name with a command like: -name=Haibo\n"
#define CHANGE_NAME "-name="

// Array to contain the sockets for all chat users
int fd_chats[MAX_CHATS];
struct name_rec
{
    char name[NAME_SIZE+1];
 };
typedef struct name_rec NAME_REC;

// Array to hold the names for all chat users
NAME_REC name_chats[MAX_CHATS];
int numChats = 0;

static inline int enable_reuse_address( int sock_fd);
int  tcp_easy_listen(const char *interface, const char *service, int listen_queue);  
void chatLoop(int fd_listen);
void processChat(int instance, char * buffer, int nread);
void addChatter(int newConnect);
void remove_chatter(int fd_toRemove);

int main (int argc, char **argv) 
{ 
    int                 sock_fd;
    
    if (argc != 3) {
      printf("usage:%s [IP] [PORT] \n" , argv[0]);
      return -1;
     }
    
    sock_fd = tcp_easy_listen(argv[1], argv[2], 100);

	chatLoop(sock_fd);
    
    return 0; 
} 


/** 
 * @brief Enable address-reuse on a socket prior to bind().
 *
 * @param sock_fd Socket that has yet to be passed to bind().
 * 
 * @return 0 or -1 on error, in which case errno will be set.
 */

static inline int
enable_reuse_address(
    int sock_fd)
{
    /* START SKELETON */
    /* COMPLETE_ME
     
       use setsockopt to enable address resuse.*/
    int on = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof &on) < 0){
        perror("SERVER ERROR setsockopt()");
        return -1;
    }
     
    /*END SKELETON */
    
    return 0;
}

/** 
 * @brief called by the sever to listen for connections over a socket address.
 * bind server address and port 
 */

int                               /* listening socket or -1 on errr */
tcp_easy_listen(
    const char *interface,        /* interface to bind to */
    const char *service,          /* string of port or serv name */
    int listen_queue)             /* argument to listen(2) */
{
  int listen_fd;
  struct sockaddr_in listen_on;
  char ip_str[INET_ADDRSTRLEN];

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    perror("server: socket()"); 
    return -1;
  }

  memset(&listen_on, 0, sizeof listen_on);
  listen_on.sin_family = AF_INET;
  if (inet_pton(AF_INET, interface, &listen_on.sin_addr) <= 0) {
    perror("server: inet_pton()"); 
    return -1;
  }

  listen_on.sin_port = htons(strtoul (service, NULL, 10));
  if (listen_on.sin_port == 0) {
    perror("server: htons()"); 
    return -1;
  }
  
  if (enable_reuse_address(listen_fd) < 0) {
    perror("server: enable_reuse_address()"); 
    return -1;
  }

  if (bind(listen_fd, (struct sockaddr *) &listen_on, sizeof listen_on) < 0) {
    perror("server: bind()"); 
    return -1;
  }
  
  if (listen(listen_fd, listen_queue) < 0) {
    perror("server: listen()"); 
    return -1;
  }
    
  if (inet_ntop(AF_INET, (const void *) &listen_on.sin_addr, ip_str, INET_ADDRSTRLEN) != NULL) {
       printf("ChatServer %s is ready to accept chatters via port %u.\n", ip_str, ntohs(listen_on.sin_port));
       printf("..................\n");
  }
   
  return listen_fd;
  
}

/*   The main loop for processing the communciation between the server and the chatters
 *   using select() to control reading data from differnt sockets
 *
*/

void chatLoop(int fd_listen)
{ 
    char buffer[BUFF_SIZE];    
    struct sockaddr_in fsin; 
    socklen_t fromlen = sizeof fsin;
    int i; 
    int maxfd, iNumInputs; 
    int newConnect, nread;
    fd_set fdCheck; 
    char ip_str[INET_ADDRSTRLEN];
    
    // The select statement is the key call in this routine. 
    
    /* START SKELETON 
     COMPLETE_ME
    
    for(; ;) 
    { 
        clear all bits in fdCheck;
        turn on the bit for fd_accept in fdCheck;
        initilize maxfd with fd_accept;
        
        turn on the bit for each chat user's socket in fdCheck, and update maxfd;
   
        check the readibility of each socket using select(), and check failure of select();

        check the readibility of the listen socket fd_listen using FD_ISSET();
        If the listen socket is ready for reading, it means that a new chatter is requesting to join. 
           (1) Accept this new chatter using accept(). 
           (2) Get the new chatter info using getpeername(), and print the followng messgae on the server:
               "Chatter ** connected to the server via port **".
           (3) check if the number of existing chatters is no smaller than MAX_CHATS. If yes, send the message
                in CHAT_FULL the new chatter using write(), and close the new connection; otherwise call the addChatter function
                to add this new chatter.
        
        
         for each chatter recorded in the fd_chats[], check the readbility of its socket
           If it is ready to read, receive the data from the socket using read()
              if the number of bytes read is larger than 0, 
                  (1)  add the NULL terminator '\n'
                  (2)  call the processChat() function to process the received data
              else 
                  (1) printf("Time to close out chat file descriptor: %d\n", fd_chats[i]);
                  (2) call the remove_chatter() function to remove this chatter.
    }
    
   END SKELETON */
    for(; ;) {
        
        FD_ZERO(&fdCheck);
        FD_SET(fd_listen, &fdCheck);
        maxfd = fd_listen;

        for(i = 0; i < numChats; i++) {
            FD_SET(fd_chats[i], &fdCheck);
            if(fd_chats[i] > maxfd)
                maxfd = fd_chats[i];
        }
        if((iNumInputs = Select(maxfd+1, &fdCheck, NULL, NULL, NULL)) < 0)
            perror("SERVER ERROR select()");

        if(FD_ISSET(fd_listen, &fdCheck)) {
            newConnect = Accept(fd_listen, (SA *) &fsin, &fromlen);
            if (getpeername(newConnect, (struct sockaddr *) &fsin, &fromlen) < 0)
                perror("SERVER ERROR getpeername()");
        
            if (!inet_ntop(AF_INET, &fsin.sin_addr, ip_str, sizeof ip_str))
                perror("SERVER ERROR inet_ntop()");
        
            printf("Chatter %s connected to the server via port %d\n",
               ip_str, ntohs(fsin.sin_port));

            if (numChats > MAX_CHATS){
                if(write(newConnect, CHAT_FULL, sizeof CHAT_FULL) < 0)
                    perror("SERVER ERROR write()");
                close(newConnect);
            } else {
                addChatter(newConnect);
            }
        }

        for (i = 0; i < numChats; i++) {
            if (FD_ISSET(fd_chats[i], &fdCheck)) {
                if ((nread = read(fd_chats[i], buffer, sizeof buffer)) > 0 ) {
                    buffer[nread-1]=0;
                    processChat(i, buffer, nread); 
                } else {
                    printf("Time to close out chat file descriptor: %d\n", fd_chats[i]);
                    remove_chatter(i);
                }
                
            }
        }
    }
}

//************************************************
// processChat called to deal with each chat message
 
void processChat(int instance, char * buffer, int nread)
{
   char outbuff[BUFF_SIZE]; 
   int i, len;
  
   len = strlen(CHANGE_NAME);
   if (strncmp(CHANGE_NAME, buffer, len)== 0)
    {        
    // Special message of  chatter requesting a name something like:
    // -name=Haibo
    // In this case we copy Haibo into the name_chats table

        strcpy((char *)&name_chats[instance], &buffer[len]);
        
        /* START SKELETON 
         COMPLETE_ME
         
         Send the following info to the new chatter
         "Your name has been changed to: ****"
         
         END SKELETON */
        len = sprintf(outbuff, "Your name has been changed to: %s\n",
                      (char *)&name_chats[instance]);
        if(write(fd_chats[instance], outbuff, len) < 0)
            perror("SERVER ERROR write()");
        return;
    }
    
   // Prefix buffer with name of user who sent it
   len = sprintf(outbuff, "%s:\n%s\n", 
       (char *)&name_chats[instance], buffer);
   
   /* START SKELETON 
      COMPLETE_ME
   
      send the data in outbuff to all the other chatter except the sender using write();
    
   END SKELETON */
   for(i = 0; i < numChats; i++) {
       if (i == instance)
           continue;
       if(write(fd_chats[i], outbuff, len) < 0)
           perror("SERVER ERROR write()");
   }
    
}

// *******************************************************
 // addChatter called to deal with a new Chatter connect

void addChatter(int newConnect)
{
  char buffer[BUFF_SIZE];  
  int i,total = 0;  

  sprintf(buffer,"unknown:%d", numChats);
  strcpy((char *)&name_chats[numChats], buffer);

// Create welcome message along with a list of who is currently
// connected to the chat server
 
  total = sprintf(buffer,"%s", WELCOME_MSG);
  if (numChats <= 0)
    total +=sprintf(&buffer[total], "You are the first chatter!\n");
  else
    {
        total += sprintf(&buffer[total], " Current Chatters: ");
        for ( i=0; i < numChats; i++)
        {
          total += sprintf(&buffer[total], "%s ", (char *) &name_chats[i]);  
        }
    }
    
     total += sprintf(&buffer[total], "\n");
    
    
    /* START SKELETON 
    COMPLETE_ME
     
     Send message back to new connecting chatter use write();
     Add Chatter to fd_chat table
     
    END SKELETON */
     if(write(newConnect, buffer, total) < 0)
         perror("SERVER ERROR write()");
     fd_chats[numChats++] = newConnect;
}


void remove_chatter(int fd_toRemove)
{
   int k, fd_save; 
   // close out terminated chat user and compress the chat tables
   fd_save = fd_chats[fd_toRemove];
   for (k=fd_toRemove; k < numChats -1; k++)
     {
        fd_chats[k] = fd_chats[k+1];
        strcpy((char *)&name_chats[k], (char *)&name_chats[k+1]);
     }
   numChats -= 1;

    /* START SKELETON 
     COMPLETE_ME
     
     Get the leaving chatter info using getpeername(), and print the followng messgae on the server:
     "Chatter IP/PORT leaves the chat".
     
    END SKELETON */
   struct sockaddr_in addr;
   char ip_str[INET_ADDRSTRLEN];
   socklen_t len = sizeof addr;
   
   if(getpeername(fd_save, (struct sockaddr *) &addr, &len) < 0)
       perror("SERVER ERROR getpeername()");
   
   if(!inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof ip_str))
       perror("SERVER ERROR inet_ntop()");
       
   printf("Chatter %s/%d leaves the chat\n", ip_str, ntohs(addr.sin_port));
   close(fd_save);
}

