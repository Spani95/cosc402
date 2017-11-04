include ../Make.defines

PROGS = tcp_sender tcp_receiver tcp_easy_ipv46_server tcp_easy_ipv46_client

all:	${PROGS}

tcp_sender:	tcp_sender.o
		            ${CC} ${CFLAGS} -o $@ tcp_sender.o ${LIBS}

tcp_receiver:	    tcp_receiver.o
		            ${CC} ${CFLAGS} -o $@ tcp_receiver.o ${LIBS}

tcp_easy_ipv46_server:	tcp_easy_ipv46_server.o
		            ${CC} ${CFLAGS} -o $@ tcp_easy_ipv46_server.o ${LIBS}

tcp_easy_ipv46_client:	tcp_easy_ipv46_client.o
		            ${CC} ${CFLAGS} -o $@ tcp_easy_ipv46_client.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
