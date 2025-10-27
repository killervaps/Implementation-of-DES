#ifndef DES_CRYPTO_H
#define DES_CRYPTO_H

#include <iostream>
#include <string>
#include <bitset>

using namespace std;

class DESCrypto {
private:
    string round_keys[16];
    string pt;

    string shift_left_once(string key_chunk);
    string shift_left_twice(string key_chunk);
    string Xor(string a, string b);
    string DES();
    string textToBinary(const string& text);
    string binaryToText(const string& binary);
    string addPadding(string binary);
    string removePadding(string binary);

public:
    void generate_keys(string key);
    string encrypt(const string& plaintext, const string& iv);
    string decrypt(const string& ciphertext, const string& iv);
};

#endif