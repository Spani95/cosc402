/**
 * @file   tcp_easy_ipv4_server.c
 * @author haibo zhang <haibo@cs.otago.ac.nz>
 * 
 * @brief  TCP server, easily accept TCP connections. (IPv4 addresses only)
 * 
 */

#include "unp.h"
#include <assert.h>

int size_recvbuf = 0; 
size_t option_writeamt = 32 * 1024; 


/** 
 * @brief Enable address-reuse on a socket prior to bind().
 *
 * This should generally be done on any socket that will be used by a server,
 * otherwise EADDRINUSE will likely result if a server is restarted; this
 * would be the case because of a previously connected socket which is still
 * in the TIME WAIT state.
 * 
 * @param sock_fd Socket that has yet to be passed to bind().
 * 
 * @return 0 or -1 on error, in which case errno will be set.
 */

static inline int
enable_reuse_address(
    int sock_fd)
{
#ifndef TCP_EASY_NO_REUSE_ADDRESS
  int on = 1;
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) < 0)
    return -1;
#endif
  return 0;
}

/** 
 * @brief called by the sever to listen for connections overa socket address.
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

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0)
    return -1;

  memset(&listen_on, 0, sizeof listen_on);
  listen_on.sin_family = AF_INET;
  if (inet_pton(AF_INET, interface, &listen_on.sin_addr) <= 0)
    return -1;

  listen_on.sin_port = htons(strtoul (service, NULL, 10));
  if (listen_on.sin_port == 0)
    return -1;

  if (enable_reuse_address(listen_fd) < 0)
    return -1;

  if (bind(listen_fd,
           (struct sockaddr *) &listen_on,
           sizeof listen_on) < 0)
    return -1;
    
    /* START SKELETON
     * COMPLETE_ME
     *    use set_buf_size() to change the size of the receive buffer according to the value of size_recvbuf;
     *    use print_buf_sizes() to check if the buffer size has been changed.
     * END SKELETON */

  /*
  set_buf_size(listen_fd, SO_RCVBUF, size_recvbuf);
  char *desc;
  print_buf_sizes(desc, listen_fd);
  */
  if (listen(listen_fd, listen_queue) < 0)
    return -1;
  
  return listen_fd;
  
}

/** 
 * @brief Read and discard data from a file descriptor until closed.
 * 
 * @param sockfd Connected socket to read from.
 * 
 * @return 0 on success, -1 on failure.
 */

int
read_and_ignore_till_end(
    int sockfd)
{
    ssize_t br;
    char *buf = malloc(option_writeamt);

    assert(buf);

    do
    {
        br = read(sockfd, buf, option_writeamt);
    } while (br > 0);

    free(buf);
    close(sockfd);

    return br;                         /* -1 or 0 */
}

/** 
 * @brief fork_server
 * using fork() function to create a new process for each connecdtion
 */

static int
tcp_receiver(int listen_fd)
{
    int connected_fd;
    
    connected_fd = accept(listen_fd, NULL, NULL);
    if (connected_fd < 0)
    {
        printf("Failed to accept a connection: ~m");
        exit(EXIT_FAILURE);
    }

    if (read_and_ignore_till_end(connected_fd) < 0)
        exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
   
}

/**
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
  
    size_recvbuf = strtoul(argv[3], NULL, 0) * 1024;

    sock_fd = tcp_easy_listen(argv[1], argv[2], 100);
    if (sock_fd < 0) {
      printf("Failed to create TCP listening socket");
      return -1;
    }
    tcp_receiver(sock_fd);

  return 0;
}



