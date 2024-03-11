#include <iostream>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

const int AES_KEY_SIZE = 256;
const int AES_IV_SIZE = 12; // GCM recommended IV size is 12 bytes
const int TAG_SIZE = 16; // GCM authentication tag size is typically 16 bytes

std::string encryptAES_GCM(const std::string& plaintext, const std::string& key) {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Generate random IV
    unsigned char iv[AES_IV_SIZE];
    if (RAND_bytes(iv, AES_IV_SIZE) != 1) {
        std::cerr << "Error generating random IV" << std::endl;
        return "";
    }

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);

    if (!EVP_EncryptInit_ex(&ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
        std::cerr << "Error initializing encryption" << std::endl;
        return "";
    }

    if (!EVP_CIPHER_CTX_ctrl(&ctx, EVP_CTRL_GCM_SET_IVLEN, AES_IV_SIZE, NULL)) {
        std::cerr << "Error setting IV length" << std::endl;
        return "";
    }

    if (!EVP_EncryptInit_ex(&ctx, NULL, NULL, reinterpret_cast<const unsigned char*>(key.c_str()), iv)) {
        std::cerr << "Error initializing key and IV" << std::endl;
        return "";
    }

    int len;
    std::string ciphertext(plaintext.size() + AES_BLOCK_SIZE, '\0');

    if (!EVP_EncryptUpdate(&ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.size())) {
        std::cerr << "Error encrypting message" << std::endl;
        return "";
    }

    int ciphertext_len = len;

    if (!EVP_EncryptFinal_ex(&ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]) + len, &len)) {
        std::cerr << "Error finalizing encryption" << std::endl;
        return "";
    }

    ciphertext_len += len;

    // Get the authentication tag
    unsigned char tag[TAG_SIZE];
    if (!EVP_CIPHER_CTX_ctrl(&ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag)) {
        std::cerr << "Error getting authentication tag" << std::endl;
        return "";
    }

    // Append tag to ciphertext
    ciphertext.append(reinterpret_cast<char*>(tag), TAG_SIZE);

    EVP_CIPHER_CTX_cleanup(&ctx);

    return ciphertext;
}

int main() {
    std::string key = "01234567890123456789012345678901";
    std::string plaintext = "Hello, World!";

    std::string encrypted = encryptAES_GCM(plaintext, key);
    if (!encrypted.empty()) {
        std::cout << "Encrypted: " << encrypted << std::endl;
    } else {
        std::cerr << "Encryption failed" << std::endl;
    }

    return 0;
}
