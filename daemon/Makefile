FLAGS=
CC=g++
LD=g++

# default compile rune...
%.o : %.cpp
	${CC} ${FLAGS} -c $<

# some defines...
LIBS=-lcurl
OBJS=testmain.o logger.o resource_server_api.o xml.o binhex.o

# default target...
testmain.x : ${OBJS}
	${LD} ${FLAGS} -o $@ ${OBJS} ${LIBS}

# dependancies...
testmain.o : testmain.cpp logger.h resource_server_api.h xml.h
logger.o : logger.cpp logger.h
xml.o : xml.cpp xml.h
binhex.o : binhex.cpp binhex.h
resource_server_api.o : resource_server_api.cpp resource_server_api.h

# clean up...
clean :
	rm *.o; rm *.x; rm *.log
