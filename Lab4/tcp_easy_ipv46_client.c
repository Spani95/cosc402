/**
 * @file   tcp_easy_ipv4_client.c
 * @author haibo zhang <haibo@cs.otago.ac.nz>
 * 
 * @brief  TCP_client, easily connect to a TCP server. (IPv4 addresses only)
 * 
 */

#include "unp.h"
#include "tcp_easy_ipv46.h"


int                       /* connected socket or -1 on error */
tcp_easy_connect(
    const char *host,           /* string of IP */
    const char *service)        /* string of port or serv name */
{

    /* START SKELETON 
     * COMPLETE_ME
     *
     * initialise and fill in an addrinfo structure for the hints:
     *   .ai_flags = 0
     *   .ai_family = AF_UNSPEC     // v4 or v6
     *   .ai_socktype = SOCK_STREAM // tcp for these families
     *   .ai_protocol = IPPROTO_TCP
     *
     * call getaddrinfo to lookup the host and service, giving `results'
     *   upon failure: return -result
     *
     * for each result in results:
     *    Create a socket using data from result.
     *      upon failure: continue.
     *    Connect using data from result.
     *      upon failure: continue;
     *      upon success: freeaddrinfo and return the socket.
     * freeaddrinfo
     * return -1    (nothing was able to be connected to)
     *
     * END SKELETON */
    
    int results, fd_client;
    
    struct addrinfo hints, *res, *ressave;
    hints.ai_flags = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if ((results = getaddrinfo(host, service, &hints, &res)) != 0)
        return -results;
    ressave = res;
    do {
        if((fd_client = socket(res->ai_family, res->ai_socktype,
                               res->ai_protocol)) < 0)
            continue;
        if(connect(fd_client, res->ai_addr, res->ai_addrlen) != 0) {
            continue;
        } else {
            freeaddrinfo(ressave);
            return fd_client;
        }
    } while((res = res->ai_next) != NULL);

    freeaddrinfo(ressave);
    return -1;
}

/**
 * @brief [TEST] Read from a socket until connection close, write on stdout.
 *
 * Simply read from socket and write to stdout until the socket
 * is closed or some error occurs.
 *
 * @param sock_fd Connected socket to read from.
 *
 * @note Can you spot the bug?
 *
 * @internal
 */

static int
dump_socket_to_stdout(
    int sock_fd)
{
  char buf[512];
  ssize_t num_bytes;
  
  while ((num_bytes = read(sock_fd, buf, 512)) > 0) {
    fwrite(buf, 1, num_bytes, stdout);
  }
  if (num_bytes <= 0)
    return -1;
  else
    return 0;
}

/**
 *
 * <pre>
 * <i>In one window...</i>
 * <i>$</i> <b>tcp_easy-ipv4addr passive 0.0.0.0 4321 hello.txt</b>
 * <i>And in another window...</i>
 * <i>$</i> <b>tcp_easy-ipv46addr active 127.0.0.1 4321</b>
 * <i>Contents of hello.txt is output</i>
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



