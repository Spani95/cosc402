include ../Make.defines

PROGS = tcp_fork_server tcp_client

all:	${PROGS}

tcp_fork_server:	tcp_fork_server.o
		            ${CC} ${CFLAGS} -o $@ tcp_fork_server.o ${LIBS}

tcp_easy_ipv4_client:	        tcp_easy_ipv4_client.o
		            ${CC} ${CFLAGS} -o $@ tcp_easy_ipv4_client.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
