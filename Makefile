ALL: genkeys client_writer

CC = g++
LIBS = -lcrypto -lssl

genkeys: genkeys.cpp utils.h
	$(CC) --std=c++11 -g -Wall gen_keys.cpp $(LIBS)  -o $@

client_writer: client_writer.cpp utils.h
	$(CC) --std=c++11 -g -Wall client_writer.cpp $(LIBS)  -o $@
