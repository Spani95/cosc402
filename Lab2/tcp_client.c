/**
 * @file   tcp_easy_ipv4_client.c
 * @author haibo zhang <haibo@cs.otago.ac.nz>
 * 
 * @brief  TCP_client, easily connect to a TCP server. (IPv4 addresses only)
 * 
 */

#include "unp.h"


int                       /* connected socket or -1 on error */
tcp_easy_connect(
    const char *host,           /* string of IP */
    const char *service)        /* string of port or serv name */
{
  int sock_fd;
  struct sockaddr_in dest;

  /* START SKELETON */
  /* COMPLETE_ME
   *
   * create a TCP socket sock_fd using socket(2)
   * initialise sockaddr_in{} using memset(3)
   * set dest.sin_family to AF_INET
   * set dest.sin_addr using inet_pton(3)
   * set dest.sin_port using strtoul(3) and htons(3)
   * call connect(2) to perform the connection
   * return sock_fd
   *
   * And at each stage, don't forget to check for failure.
*/
  /* END SKELETON */
  if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) perror("socket error\n");
  
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  if (inet_pton(AF_INET, host, &dest.sin_addr) <= 0)
      printf("inet_pton error for %s\n", host);
  
  dest.sin_port = htons(strtoul(service, NULL, 10));
  if (connect(sock_fd,(SA *)&dest, sizeof(dest)) < 0)
      perror("connect error\n");
  
  //remove the following line after you implement this function
  //return -1;
  return sock_fd;
}

/**
 * @brief [TEST] Read from a socket until connection close, write on stdout.
 *
 * Simply read from socket and write to stdout until the socket
 * is closed or some error occurs.
 *
 * @param sock_fd Connected socket to read from.
 *
 * @internal
 */

static int
dump_socket_to_stdout(
    int sock_fd)
{
  char buf[512];
  ssize_t num_bytes;
  
  /* START SKELETON */
  /* COMPLETE_ME
   *
   * continue reading data from the socket using read(2) if the number of bytes returned is larger than 0, 
   *      and output the received data on the standard output (stdout) using fwrite(3).
   *  if the number of bytes returned is no larger than 0, return -1; otherwise return 0;
   *
  */
  
  /* END SKELETON */
  for(;;) {
      num_bytes=read(sock_fd,buf,sizeof buf);
      if(num_bytes > 0) {
          fwrite(buf, 1, num_bytes, stdout);
          printf("data received\n");
      }
      if(num_bytes < 0) return -1;
      else return 0;
  }


  //remove the following line after you implement this function
  //return -1;
}

/**
 *
 * </pre>
 *
 * @param argc Number of arguments found in @c argv
 * @param argv Command-line arguments.
 *
 * @return 0 on success, non-0 (typically -1) on failure.
 *
 * @internal
 */

int
main(
    int argc,
    char *argv[])
{
  int sock_fd;

    sock_fd = tcp_easy_connect(argv[1], argv[2]);
    if (sock_fd < 0) {
      printf("Failed to create TCP connected socket\n");
      return -1;
    }
    else {
        dump_socket_to_stdout(sock_fd);
    }

    close(sock_fd);
    return 0;
}



