#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	char	sendline[MAXLINE], recvline[MAXLINE];

	if (argc != 3)
		err_quit("usage: ./tcp_client <IPaddress> <port>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(strtoul (argv[2], NULL, 10)); 
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	while (1) {
     if  (Fgets(sendline, MAXLINE, stdin) != NULL) {
        Writen(sockfd, sendline, strlen(sendline));
     }
     if  (Readline(sockfd, recvline, MAXLINE) > 0) {
        Fputs(recvline, stdout);
     }
  }


	exit(0);
}
