ALL: gen_keys client_writer

CC = g++
LIBS = -lcrypto -lssl

gen_keys: gen_keys.cpp utils.h
	$(CC) --std=c++11 -g -Wall gen_keys.cpp $(LIBS)  -o $@

client_writer: client_writer.cpp utils.h
	$(CC) --std=c++11 -g -Wall client_writer.cpp $(LIBS)  -o $@

clean:
	rm -f *.o gen_keys client_writer *~ *.pem
