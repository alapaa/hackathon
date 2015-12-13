#include <memory>
#include <random>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdio> // only __FILE__ __LINE__ macros
#include <map>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

#include <cassert>

#include "utils.h"

using std::unique_ptr;
using std::string;
using std::cout;
using std::cerr;
using std::stringstream;
using std::ifstream;
using std::exception;
using std::runtime_error;
using std::map;

using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
using RSA_ptr = std::unique_ptr<RSA, decltype(&::RSA_free)>;
using EVP_KEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
using BIO_FILE_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

enum class TokenStatus {
    GIVE_TOKEN = 1,
    DENY_TOKEN = 2
};

string seed_random_number_gen()
{
    const int SEED_SZ = 32;
    int rc;

    char buf[SEED_SZ];
    int fd = open("/dev/random", O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Err opening /dev/random" FILE_LINE);
    }
    int n = read(fd, buf, sizeof(buf)-1);
    if (n == -1) {
        throw std::runtime_error("Err reading /dev/random" FILE_LINE);
    }
    close(fd);
    cout << "Seeding SSL RNG from /dev/random...\n";
    RAND_add(buf, sizeof(buf), n);

    buf[n] = '\0';
    cout << "Generated random seed:\n";
    cout << buf << '\n';

    rc = RAND_status();
    if (rc != 1)
        throw std::runtime_error("Open SSL (RAND_status()): " FILE_LINE);

    return string(buf);
}

void generate_key_pair(string& seed)
{
    int rc;

    RSA_ptr rsa(RSA_new(), ::RSA_free);
    BN_ptr bn(BN_new(), ::BN_free);

    BIO_FILE_ptr pem1(BIO_new_file("rsa-public-1.pem", "w"), ::BIO_free);

    BIO_FILE_ptr pem3(BIO_new_file("rsa-private-1.pem", "w"), ::BIO_free);


    rc = BN_set_word(bn.get(), RSA_F4);
    if (rc != 1)
        throw std::runtime_error("Open SSL: " FILE_LINE);



    // Generate key
    rc = RSA_generate_key_ex(rsa.get(), 2048, bn.get(), NULL);
    if (rc != 1)
        throw std::runtime_error("Open SSL: " FILE_LINE);

    // Convert RSA to PKEY
    EVP_KEY_ptr pkey(EVP_PKEY_new(), ::EVP_PKEY_free);
    rc = EVP_PKEY_set1_RSA(pkey.get(), rsa.get());
    if (rc != 1)
        throw std::runtime_error("Open SSL: " FILE_LINE);

    // Write public key in PKCS PEM
    rc = PEM_write_bio_RSAPublicKey(pem1.get(), rsa.get());
    if (rc != 1)
        throw std::runtime_error("Open SSL: " FILE_LINE);

    // Write private key in PKCS PEM.
    rc = PEM_write_bio_PrivateKey(pem3.get(), pkey.get(), NULL, NULL, 0, NULL,
                                  NULL);
    if (rc != 1)
        throw std::runtime_error("Open SSL: " FILE_LINE);

}

int open_socket()
{
    int fd;
    int retval;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        throw std::runtime_error("Could not open socket" FILE_LINE);
    }

    unlink(SOCK_NAME);
    sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_NAME, sizeof(addr.sun_path)-1);
    retval = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (retval == -1) {
        perror(NULL);
        throw std::runtime_error("bind() " FILE_LINE);
    }

    retval = listen(fd, 5);
    if (retval == -1) {
        throw std::runtime_error("listen() " FILE_LINE);
    }
    return fd;
}

void close_socket(int fd)
{
    close(fd);

    /* remove the FIFO */
    unlink(SOCK_NAME);
}

string read_socket(int fd)
{
    char buf[MAX_BUF];

    ssize_t num = read(fd, buf, MAX_BUF);
    if (num == -1) {
        throw std::runtime_error("read() " FILE_LINE);
    }
    printf("Received: %s\n", buf);

    return string(buf);
}

void write_socket(int fd, TokenStatus status)
{

    string pubkey;
    if (status == TokenStatus::GIVE_TOKEN) {
        ifstream pubkey_file;
        pubkey_file.open("rsa-public-1.pem");

        string line;
        if (pubkey_file.is_open())
        {
            while (getline(pubkey_file, line))
            {
                cout << line << '\n';
                pubkey += line;
            }
            pubkey_file.close();
        }
    }
    else { // DENY_TOKEN
        pubkey = "WARNING: TOKEN DENIED, VOTER ALREADY RECEIVED TOKEN!";
    }

    ssize_t num = write(fd, pubkey.c_str(), pubkey.length()+1);
    if (num == -1) {
        throw std::runtime_error("write() " FILE_LINE);
    }
}



int main(int argc, char* argv[])
{
    cout << "Starting token (pubkey) server...\n";

    int fd = -1;

    fd = open_socket();
    std::map<string, time_t> voters;

    try {
        for (;;) {
            string seed = seed_random_number_gen();
            generate_key_pair(seed);

            int cli_fd = accept(fd, NULL, NULL);
            if (cli_fd == -1) {
                throw std::runtime_error("accept() " FILE_LINE);
            }

            string voter = read_socket(cli_fd);
            auto it = voters.find(voter);
            if (it == voters.end()) {
                cout << "Handing out token (pubkey) for voter " << voter << '\n';
                write_socket(cli_fd, TokenStatus::GIVE_TOKEN);
            }
            else {
                cout << "DENYING token (pubkey) for voter " << voter << '\n';
                write_socket(cli_fd, TokenStatus::DENY_TOKEN);
            }
            voters.insert(std::make_pair(voter, time(nullptr)));
            close(cli_fd);
        }
    }
    catch (exception& ex) {
        cerr << ex.what() << '\n';
    }
    catch (...)
    {
        cerr << "Unknown error!\n";
    }

    close_socket(fd);

    return 0;
}
