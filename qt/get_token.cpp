#include <iostream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <cstdio>
#include <memory>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>


#include "../utils.h"

using std::cout;
using std::string;

string get_token(string personnr)
{
    int retval;

    int fd;
    char token[MAX_BUF];

    // send voter's 'personnr' to token server

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        throw std::runtime_error("Could not open socket" FILE_LINE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_NAME, sizeof(addr.sun_path)-1);
    retval = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (retval == -1) {
        throw std::runtime_error("connect() " FILE_LINE);
    }

    ssize_t num = write(fd, personnr.c_str(), personnr.length()+1);
    if (num == -1) {
        throw std::runtime_error("write() " FILE_LINE);
    }

    // receive voter's token from token server
    num = read(fd, token, MAX_BUF);
    if (num == -1) {
        throw std::runtime_error("read() " FILE_LINE);
    }

    cout << "Got token, length " << num << ":\n";
    cout << token << '\n';

    close(fd);

    return string(token);
}

std::string exec(const char* cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}
