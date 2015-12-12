#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <cstring>

#include "utils.h"
using std::cout;



int main(int argc, char* argv[])
{
    if (argc != 2) {
        throw std::runtime_error("Usage: <progname> <Swedish personnr>");
    }

    int fd;
    const char * myfifo = FIFO_NAME;
    char token[MAX_BUF];

    // send voter's 'personnr' to token server
    fd = open(myfifo, O_RDWR);
    ssize_t num = write(fd, argv[1], strlen(argv[1])+1);
    if (num == -1) {
        throw std::runtime_error("write() " FILE_LINE);
    }

    // receive voter's token from token server
    read(fd, token, MAX_BUF);
    if (num == -1) {
        throw std::runtime_error("read() " FILE_LINE);
    }

    cout << "Got token, length " << num << ":\n";
    cout << token;

    close(fd);

    /* remove the FIFO */
    unlink(myfifo);

    return 0;
}
