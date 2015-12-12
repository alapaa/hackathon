#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#inclucd <stdexcept>

#include "utils.h"
using std::cout;



int main(int argc, char* argv[])
{
    if (argc != 2) {
        throw std::runtime_error("Usage: <progname> <Swedish personnr>");
    }

    int fd;
    char * myfifo = "/tmp/myfifo";
    char token[MAX_BUF];

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);

    /* write voter's 'personnr' to the FIFO */
    fd = open(myfifo, O_RDWRITE);
    ssize_t num = write(fd, argv[1], strlen(argv[1])+1);
    if (num == -1) {
        throw std::runtime_error("write() " FILE_LINE)
    }
    read(fd, token, MAX_BUF);
    if (num == -1) {
        throw std::runtime_error("read() " FILE_LINE)
    }

    cout << "Got token, length " << num << ":\n";
    cout << token;

    close(fd);

    /* remove the FIFO */
    unlink(myfifo);

    return 0;
}
