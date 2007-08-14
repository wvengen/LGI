FLAGS=
CC=g++
LD=g++

# default compile rune...
%.o : %.cpp
	${CC} ${FLAGS} -c $<

# some defines...
LIBS=-lcurl
OBJS=testmain.o logger.o resource_server_api.o xml.o csv.o binhex.o daemon_config.o daemon_jobclass.o

# default target...
testmain.x : ${OBJS}
	${LD} ${FLAGS} -o $@ ${OBJS} ${LIBS}

# dependancies...
testmain.o : testmain.cpp logger.h resource_server_api.h binhex.h csv.h xml.h daemon_config.h daemon_jobclass.h

daemon_jobclass.o : daemon_jobclass.cpp daemon_jobclass.h daemon_config.h xml.h binhex.h csv.h

daemon_config.o : daemon_config.cpp daemon_config.h xml.h binhex.h csv.h

resource_server_api.o : resource_server_api.cpp resource_server_api.h

logger.o : logger.cpp logger.h
xml.o : xml.cpp xml.h
csv.o : csv.cpp csv.h
binhex.o : binhex.cpp binhex.h

# clean up...
clean :
	rm *.o; rm *.x; rm *.log
