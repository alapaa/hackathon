#include <memory>
#include <random>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdio> // only __FILE__ __LINE__ macros

#include <fcntl.h>
#include <sys/stat.h>
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
using std::exception;
using std::runtime_error;

using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
using RSA_ptr = std::unique_ptr<RSA, decltype(&::RSA_free)>;
using EVP_KEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
using BIO_FILE_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

string seed_random_number_gen()
{
    const int SEED_SZ = 128;
    std::uniform_int_distribution<int> d(0, 9);
    stringstream ss;

    std::random_device rd1; // uses RDRND or /dev/urandom

    for(int n = 0; n < SEED_SZ; ++n) {
        ss << d(rd1);
    }

    cout << "Generated random seed:\n";
    cout << ss.str() << '\n';
    return ss.str();
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


    // Seed the openssl RNG
    cout << "Seeding RNG from /dev/urandom...\n";
    RAND_add(seed.c_str(), seed.length(), seed.length());
    rc = RAND_status();
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

    //////////

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
    char * myfifo = "/tmp/myfifo";
    char buf[MAX_BUF];

    /* open, read, and display the message from the FIFO */
    fd = open(myfifo, O_RDWRITE);
    if (fd == -1) {
        throw std::runtime_error("Could not open socket" FILE_LINE);
    }

    return fd;
}

void close_socket(int fd)
{
    close(fd);
}

string read_socket(int fd)
{
    char buf[MAX_BUF];

    fd = open(myfifo, O_RDONLY);
    ssize_t num = read(fd, buf, MAX_BUF);
    if (num == -1) {
        throw std::runtime_error("read() " FILE_LINE)
    }
    printf("Received: %s\n", buf);

    return string(buf);
}

void write_socket(int fd)
{
    ofstream pubkey_file;
    pubkey_file.open("rsa-public-1.pem");

    string pubkey;
    if (pubkey_file.is_open())
    {
        while (getline(pubkey_file, line))
        {
            cout << line << '\n';
            pubkey += line;
        }
        pubkey_file.close();
    }

    fd = open(myfifo, O_RDWRITE);
    ssize_t num = write(fd, pubkey, pubkey.length());
    if (num == -1) {
        throw std::runtime_error("write() " FILE_LINE)
    }
}

int main(int argc, char* argv[])
{
    cout << "Starting token (pubkey) server...\n";

    int fd = -1;

    fd = open_socket();

    try {
        for (;;) {
            string seed = seed_random_number_gen();
            generate_key_pair(seed);

            voter = read_socket(fd);
            cout << "Handing out token (pubkey) for voter " << voter << '\n';
            write_socket(fd)
        }
    }
    catch (exception& ex) {
        cerr << ex.what() << '\n';
    }

    close_socket(fd);

    return 0;
}
