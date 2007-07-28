FLAGS=-g -O0
CC=g++
LD=g++

%.o : %.cpp
	${CC} ${FLAGS} -c $<

LIBS=-lcurl
OBJS=testmain.o logger.o resource_server_api.o xml.o
testmain.x : ${OBJS}
	${LD} ${FLAGS} -o $@ ${OBJS} ${LIBS}

testmain.o : testmain.cpp logger.h resource_server_api.h xml.h
logger.o : logger.cpp logger.h
xml.o : xml.cpp xml.h
resource_server_api.o : resource_server_api.cpp resource_server_api.h

clean :
	rm *.o; rm *.x; rm *.log
