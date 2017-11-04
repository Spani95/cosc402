/**
 * @file   tcp_easy_ipv4_server.c
 * @author haibo zhang <haibo@cs.otago.ac.nz>
 * 
 * @brief  TCP server, easily accept TCP connections. (IPv4 addresses only)
 * 
 */

#include "unp.h"
#include "tcp_easy_ipv46.h"


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
 *
 * @note To demonstrate the effect of when address-reuse is @em not
 * set, a compile-time variable @c TCP_EASY_NO_REUSE_ADDRESS may be
 * defined, in which case this function returns 0 (success) without
 * doing anything.  This should not be done in production code.
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
 * @brief Disable IPv4-mapped IPv6 addresses on a socket.
 * 
 * @param sock_fd AF_INET6 socket that has not yet been passed to bind().
 * 
 * @return 0 or -1 on error, in which case errno will be set.
 *
 * This function disables (your system may default to on or off on
 * different systems) IPv4-mapped IPv6 addresses.
 *
 * It's generally preferable to handle IPv4 and IPv6 as seperate
 * things. Otherwise, if you ask a server to listen on :: then
 * additionally on 0.0.0.0, then you get an 'Address in use' error. It
 * also means you need to handle (or not, in which case your users
 * might get a surprise), the ::ffff:A.B.C.D format addresses.
 */

static inline int
disable_v4_mapping(
    int sock_fd
) {
    int on = 1;
    //trace(8, "disable_v4_mapping(~d)", sock_fd);
    return setsockopt(sock_fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof on);
}


int                               /* listening socket or -1 on errr */
tcp_easy_listen(
    const char *address,        /* interface to bind to */
    const char *service,          /* string of port or serv name */
    int listen_queue)             /* argument to listen(2) */
{
    /* START SKELETON */
    /* COMPLETE_ME
     *
     * fill in a hints structure for getaddrinfo
     * contents of hints structure similar to tcp_connect,
     * but now we add AI_PASSIVE.
     *
     * getaddrinfo
     *     on error: return translate_eai_to_tcp_error(result)
     *
     * Loop through the results, taking the first address that succeeds:
     *     enable address re-use (see function above)
     *         on error: continue onto next
     *     call bind(2) to bind listen_fd to listen_on
     *         on error: continue onto next
     *     call listen(2) to enable connections to be accepted on listen_fd
     *         on error: continue onto next
     *     freeaddrinfo
     *     return listen_fd
     * freeaddrinfo
     * return -1
     */
    /* END SKELETON */
    /* START TRIM */

    /* Note that AI_ADDRCONFIG will cause a bind to a v6 address to fail
     * if there are no non-link-local addresses present on the system. */
    int listen_fd, results;
    
    struct addrinfo hints, *res, *ressave;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if((results = getaddrinfo(address, service, &hints, &res)) != 0)
        //return translate_eai_to_tcp_error(results);
        return -results;

    ressave = res;
    do {
        if((listen_fd = socket(res->ai_family, res->ai_socktype, res->
                               ai_protocol)) < 0)
            continue;
        enable_reuse_address(listen_fd);

        if (bind(listen_fd, res->ai_addr, res->ai_addrlen) != 0)
            continue;
        if (listen(listen_fd, listen_queue) < 0)
            continue;
        freeaddrinfo(ressave);
        return listen_fd;
    } while ((res = res->ai_next) != NULL);
    freeaddrinfo(ressave);
    return -1;
}

static int
send_file_to_socket(
    int file_fd,
    int sock_fd)
{

  off_t len = 0;                /* means send until end */
  if (sendfile(file_fd, sock_fd, 0, &len, NULL, 0) < 0)
    return -1;

  return 0;

}


static void
server_loop(
    int listen_fd,
    const char *filename)
{
  int connected_fd;
  int file_fd;

  while (1) {
    printf("Awaiting connection...");
    connected_fd = accept(listen_fd, NULL, 0);
    if (connected_fd < 0) {
      printf("%s\n", strerror(errno));
      return;
    } else {
      printf("success\n");
    }


    file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
       printf("Failed to open file\n");
      return;
    }

    if (send_file_to_socket(file_fd, connected_fd) < 0) {
      printf("Failed to send (all) file\n");
      return;
    }

    close(file_fd);

    close(connected_fd);
  }
}


/**
 * @brief [TEST] Entry-point for test program.
 */

int
main(
    int argc,
    char *argv[])
{
  int sock_fd;

    sock_fd = tcp_easy_listen(argv[1], argv[2], 100);
    if (sock_fd < 0) {
      printf("Failed to create TCP listening socket");
      return -1;
    }
    server_loop(sock_fd, argv[3]);

  return 0;
}



