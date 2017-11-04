include ../Make.defines

PROGS = chatserver chatclient tcp_client

all:	${PROGS}

chatserver:	        chatserver.o
		            ${CC} ${CFLAGS} -o $@ chatserver.o ${LIBS}

chatclient:	        chatclient.o
		            ${CC} ${CFLAGS} -o $@ chatclient.o ${LIBS}
		            
tcp_client:	        tcp_client.o
		            ${CC} ${CFLAGS} -o $@ tcp_client.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
