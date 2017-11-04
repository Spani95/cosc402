/** 
 * @brief Get and print a socket's send and receive buffer size.
 * 
 * @param[in] description String to print before the output.
 * @param sockfd Socket to report on.
 */
#include	"unp.h"

void
print_buf_sizes(
    const char *description,    // hint for debugging
    int sockfd)
{
    int send, recv;
    socklen_t send_len, recv_len;
    
  /* START SKELETON 
  * COMPLETE_ME
  *    use getsockopt() to get the value for SO_RCVBUF and store it in recv
  *    use getsockopt() to get the value for SO_SNDBUF and store it in send
  *    check failure at each stage.
  * END SKELETON */
    if(getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recv, &recv_len) < 0)
        perror("rcv error:");
    if(getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &send, &send_len) < 0)
        perror("send error:");
    printf("%s: SO_RCVBUF = %d, SO_SNDBUF = %d\n", description, recv, send);
}
