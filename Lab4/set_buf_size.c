/** 
 * @brief Set the send or receive socket buffer size.
 * 
 * This sets the send/receive buffer size inside the kernel. It impacts
 * how much data can be outstanding in a TCP sliding window.
 *
 * @param sockfd Socket to set the send or receive socket buffer size.
 * @param which @c SO_RCVBUF (receive) or @c SO_SNDBUF (send)
 * @param val New send or receive buffer size.
 */
 
#include	"unp.h"

int
set_buf_size(
    int sockfd,
    int which,
    int val)
{
    socklen_t val_len;
    
  /* START SKELETON *
  * COMPLETE_ME
  *    use setsockopt() to set the value in val to the socket option given in which.
  *    if the returned value is smaller than 0, return -1; otherwise return 0.
  * END SKELETON */
    if(setsockopt(sockfd, SOL_SOCKET, which, &val, sizeof val_len) < 0)
        return -1;
    return 0;

}
