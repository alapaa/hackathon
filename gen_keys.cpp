#include <memory>
#include <random>
#include <string>
#include <iostream>
#include <sstream>

#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

#include <cassert>
#define ASSERT assert

using std::unique_ptr;
using std::string;
using std::cout;
using std::stringstream;

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
    ASSERT(rc == 1);

    // Seed the openssl RNG
    cout << "Seeding RNG from /dev/urandom...\n";
    RAND_add(seed.c_str(), seed.length(), seed.length());
    rc = RAND_status();
    ASSERT(rc == 1);

    // Generate key
    rc = RSA_generate_key_ex(rsa.get(), 2048, bn.get(), NULL);
    ASSERT(rc == 1);

    // Convert RSA to PKEY
    EVP_KEY_ptr pkey(EVP_PKEY_new(), ::EVP_PKEY_free);
    rc = EVP_PKEY_set1_RSA(pkey.get(), rsa.get());
    ASSERT(rc == 1);

    //////////

    // Write public key in PKCS PEM
    rc = PEM_write_bio_RSAPublicKey(pem1.get(), rsa.get());
    ASSERT(rc == 1);

    // Write private key in PKCS PEM.
    rc = PEM_write_bio_PrivateKey(pem3.get(), pkey.get(), NULL, NULL, 0, NULL,
                                  NULL);
    ASSERT(rc == 1);

}

int main(int argc, char* argv[])
{
    string seed = seed_random_number_gen();
    generate_key_pair(seed);

    return 0;
}
