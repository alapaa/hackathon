ALL: gen_keys client_writer

CC = g++
CXXFLAGS = --std=c++11 -g -Wall
LDFLAGS =
LIBS = -lcrypto -lssl

gen_keys: gen_keys.cpp utils.h
	$(CC)  $(CXXFLAGS) gen_keys.cpp $(LIBS) $(LDFLAGS) -o $@

client_writer: client_writer.cpp utils.h
	$(CC) $(CXXFLAGS) client_writer.cpp $(LIBS) $(LDFLAGS) -o $@

clean:
	rm -f *.o gen_keys client_writer *~ *.pem
