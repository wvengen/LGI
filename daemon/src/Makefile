FLAGS=-O2
CC=g++
LD=g++

# default compile run...
%.o : %.cpp
	${CC} ${FLAGS} -c $<

# some defines...
LIBS=-lcurl
OBJS=logger.o resource_server_api.o xml.o csv.o hash.o binhex.o daemon_configclass.o daemon_jobclass.o daemon_mainclass.o daemon_main.o xml_main.o binhex_main.o hexbin_main.o hash_main.o csv_main.o filetransfer_main.o interface_server_api.o qstat_main.o qdel_main.o qsub_main.o

# all targets...
all : LGI_daemon xml binhex hexbin hash csv LGI_filetransfer LGI_qstat LGI_qdel LGI_qsub

xml : xml.o csv.o xml_main.o
	${LD} ${FLAGS} -o $@ xml.o csv.o xml_main.o

binhex : binhex.o binhex_main.o
	${LD} ${FLAGS} -o $@ binhex.o binhex_main.o

hexbin : binhex.o hexbin_main.o
	${LD} ${FLAGS} -o $@ binhex.o hexbin_main.o

hash : hash.o binhex.o hash_main.o
	${LD} ${FLAGS} -o $@ hash.o binhex.o hash_main.o

csv : csv.o csv_main.o
	${LD} ${FLAGS} -o $@ csv.o csv_main.o

LGI_daemon : logger.o resource_server_api.o xml.o csv.o hash.o binhex.o daemon_configclass.o daemon_jobclass.o daemon_mainclass.o daemon_main.o
	${LD} ${FLAGS} -o $@ logger.o resource_server_api.o xml.o csv.o hash.o binhex.o daemon_configclass.o daemon_jobclass.o daemon_mainclass.o daemon_main.o ${LIBS}

LGI_filetransfer : logger.o resource_server_api.o xml.o csv.o hash.o binhex.o daemon_configclass.o daemon_jobclass.o filetransfer_main.o
	${LD} ${FLAGS} -o $@ logger.o resource_server_api.o xml.o csv.o hash.o binhex.o daemon_configclass.o daemon_jobclass.o filetransfer_main.o ${LIBS}

LGI_qstat : logger.o interface_server_api.o xml.o csv.o binhex.o qstat_main.o
	${LD} ${FLAGS} -o $@ logger.o interface_server_api.o xml.o csv.o binhex.o qstat_main.o ${LIBS}

LGI_qdel : logger.o interface_server_api.o xml.o csv.o binhex.o qdel_main.o
	${LD} ${FLAGS} -o $@ logger.o interface_server_api.o xml.o csv.o binhex.o qdel_main.o ${LIBS}

LGI_qsub : logger.o interface_server_api.o xml.o csv.o binhex.o hash.o resource_server_api.o daemon_jobclass.o daemon_configclass.o qsub_main.o
	${LD} ${FLAGS} -o $@ logger.o interface_server_api.o resource_server_api.o daemon_jobclass.o daemon_configclass.o xml.o csv.o binhex.o hash.o qsub_main.o ${LIBS}

# all dependancies...
xml_main.o : xml_main.cpp xml.h csv.h

binhex_main.o : binhex_main.cpp binhex.h

hexbin_main.o : hexbin_main.cpp binhex.h

hash_main.o : hash_main.cpp hash.h binhex.h

csv_main.o : csv_main.cpp csv.h

filetransfer_main.o : filetransfer_main.cpp logger.h resource_server_api.h binhex.h hash.h csv.h xml.h daemon_configclass.h daemon_jobclass.h

qstat_main.o : qstat_main.cpp logger.h interface_server_api.h binhex.h csv.h xml.h

qdel_main.o : qdel_main.cpp logger.h interface_server_api.h binhex.h csv.h xml.h

qsub_main.o : qsub_main.cpp logger.h interface_server_api.h resource_server_api.h daemon_jobclass.h daemon_configclass.h binhex.h csv.h xml.h hash.h

daemon_main.o : daemon_main.cpp logger.h resource_server_api.h binhex.h hash.h csv.h xml.h daemon_configclass.h daemon_jobclass.h daemon_mainclass.h

daemon_mainclass.o : daemon_mainclass.cpp daemon_mainclass.h daemon_jobclass.h daemon_configclass.h resource_server_api.h xml.h binhex.h csv.h hash.h

daemon_main.o : daemon_main.cpp logger.h resource_server_api.h binhex.h hash.h csv.h xml.h daemon_configclass.h daemon_jobclass.h daemon_mainclass.h

daemon_mainclass.o : daemon_mainclass.cpp daemon_mainclass.h daemon_jobclass.h daemon_configclass.h resource_server_api.h xml.h binhex.h csv.h hash.h

daemon_jobclass.o : daemon_jobclass.cpp daemon_jobclass.h daemon_configclass.h resource_server_api.h xml.h binhex.h csv.h hash.h

daemon_configclass.o : daemon_configclass.cpp daemon_configclass.h xml.h binhex.h csv.h hash.h

resource_server_api.o : resource_server_api.cpp resource_server_api.h

interface_server_api.o : interface_server_api.cpp interface_server_api.h

logger.o : logger.cpp logger.h

xml.o : xml.cpp xml.h

csv.o : csv.cpp csv.h

binhex.o : binhex.cpp binhex.h

hash.o : hash.cpp hash.h

# clean up...
clean :
	rm -rf ${OBJS} LGI_daemon xml binhex hexbin hash csv LGI_filetransfer LGI_qstat LGI_qdel LGI_qsub

# install...
uninstall :
	rm -rf ../bin

# install...
install : all
	mkdir ../bin; cp LGI_daemon xml binhex hexbin hash csv LGI_filetransfer LGI_qstat LGI_qsub LGI_qdel ../bin
