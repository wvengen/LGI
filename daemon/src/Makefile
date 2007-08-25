FLAGS=-O2
CC=g++
LD=g++

# default compile rune...
%.o : %.cpp
	${CC} ${FLAGS} -c $<

# some defines...
LIBS=-lcurl
OBJS=logger.o resource_server_api.o xml.o csv.o hash.o binhex.o daemon_configclass.o daemon_jobclass.o daemon_mainclass.o daemon_main.o

# default target...
LGI_daemon.x : ${OBJS}
	${LD} ${FLAGS} -o $@ ${OBJS} ${LIBS}

# dependancies...
daemon_main.o : daemon_main.cpp logger.h resource_server_api.h binhex.h hash.h csv.h xml.h daemon_configclass.h daemon_jobclass.h daemon_mainclass.h

daemon_mainclass.o : daemon_mainclass.cpp daemon_mainclass.h daemon_jobclass.h daemon_configclass.h resource_server_api.h xml.h binhex.h csv.h hash.h

daemon_jobclass.o : daemon_jobclass.cpp daemon_jobclass.h daemon_configclass.h resource_server_api.h xml.h binhex.h csv.h hash.h

daemon_configclass.o : daemon_configclass.cpp daemon_configclass.h xml.h binhex.h csv.h hash.h

resource_server_api.o : resource_server_api.cpp resource_server_api.h

logger.o : logger.cpp logger.h
xml.o : xml.cpp xml.h
csv.o : csv.cpp csv.h
binhex.o : binhex.cpp binhex.h
hash.o : hash.cpp hash.h

# clean up...
clean :
	rm ${OBJS} LGI_daemon.x
