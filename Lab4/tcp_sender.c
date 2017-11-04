/**
 * @file   tcp_easy_ipv4_client.c
 * @author haibo zhang <haibo@cs.otago.ac.nz>
 * 
 * @brief  TCP_client, easily connect to a TCP server. (IPv4 addresses only)
 * 
 */

#include "unp.h"
#include <assert.h>


int size_sendbuf = 0;   
int size_senddata = 0;
size_t option_writeamt = 32 * 1024; 


int                       /* connected socket or -1 on error */
tcp_easy_connect(
    const char *host,           /* string of IP */
    const char *service)        /* string of port or serv name */
{
  int sock_fd;
  struct sockaddr_in dest;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0)                                         return -1;

  memset(&dest, 0, sizeof dest);
  dest.sin_family = AF_INET;
  if (inet_pton(AF_INET, host, &dest.sin_addr) <= 0)       return -1;

  dest.sin_port = htons(strtoul (service, NULL, 10));
  if (dest.sin_port == 0)                                  return -1;
    
    
    /* START SKELETON *
    * COMPLETE_ME
     *    use set_buf_size() to change the size of the receive buffer according to the value of size_sendbuf;
     *    use print_buf_sizes() to check if the buffer size has been changed.
     * END SKELETON */
  
  //set_buf_size(sock_fd, SO_SNDBUF, size_sendbuf);
  //char *desc;
  //print_buf_sizes(desc, sock_fd);
    

  
  if (connect(sock_fd, 
              (struct sockaddr *) &dest,
              sizeof dest) < 0)                            return -1;
              
  return sock_fd;

}

/** 
 * @brief Writes the data to the peer.
 * 
 * The data which is sent is just the sequence of capital letters, repeated
 * over and over again.
 *
 * @param sockfd Connected socket to write the data to.
 * @param amount Number of bytes to send.
 * 
 * @return 0 on success, -1 on failure.
 */

int
send_data_to(
    int sockfd,
    size_t amount)
{
    size_t i;
    ssize_t tmp;
    char *buf = malloc(option_writeamt);

    assert(buf);
    for (i = 0; i < option_writeamt; i++)
        buf[i] = 'A' + (i % 26);

    do
    {
        tmp = write(sockfd, buf, option_writeamt);
        if (tmp <= 0)
            return -1;
        amount -= tmp;
    } while (amount > option_writeamt);

    return 0;
}


int
main(
    int argc,
    char *argv[])
{
  int connected_fd;
  
  size_sendbuf = strtoul(argv[3], NULL, 0) * 1024;
  size_senddata = strtoul(argv[4], NULL, 0) * 1024 * 1024;
 
  connected_fd = tcp_easy_connect(argv[1], argv[2]);
  if (connected_fd < 0) {
      printf("Failed to create TCP connected socket\n");
      return -1;
  }
  else {
   if (send_data_to(connected_fd, size_senddata) < 0)
        exit(EXIT_FAILURE);     
  }

  close(connected_fd);
  return 0;
}

