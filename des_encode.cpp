// 2018-03-02 09:22PM

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

#include "cryptopp/cryptlib.h"
#include "cryptopp/hex.h"
#include "cryptopp/filters.h"
#include "cryptopp/des.h"
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"

using namespace CryptoPP;

void des_encryption_8(unsigned char *input, unsigned char *key, unsigned char *output) {
//    copy(input, input + 8, output);
    DESEncryption desEncryptor;
    unsigned char xorBlock[8];
    memset(xorBlock,0,8);
    desEncryptor.SetKey(key,8);
    desEncryptor.ProcessAndXorBlock(input,xorBlock,output);
}

void des_encryption(unsigned char *plaintext, unsigned char *key, unsigned char *ciphertext, streampos file_size) {
    unsigned char subtext[9];
    unsigned char subcipher[9];
    
    memset(subtext, '\0', 9);
    memset(subcipher, '\0', 9);
    
    //cout << "plaintext: " << plaintext << endl;
        
    for(int i=0; i < file_size; i=i+8) {        
        int start = i;
        int end;
        
        if(i + 7 < file_size) {
            end = start + 8;
            copy(plaintext + start, plaintext + end, subtext);
        } else {
            end = file_size;
            int size = 8 - (end - start);
            memset(subtext,size,9);
            copy(plaintext + start, plaintext + end, subtext);
        }
                       
        des_encryption_8(subtext, key, subcipher);
        copy(subcipher, subcipher + 8, ciphertext + start);
                
        //cout << i << "p: " << subtext << endl;        
        //cout << i << "c: " << subcipher << endl;
    }
    
    //cout << "ciphertext: " << ciphertext << endl;
}

void read_key(char *keystring, unsigned char *key) {
    //cout << "read_key -> keystring = " << keystring << endl;
    
    key = new unsigned char[8];
    memset(key, 0, 8);
    
    for (int i = 0; i < 8; i++) {
        if (keystring[i] != '\0') {
            key[i] = (unsigned char)keystring[i];
        } else {
            break;
        }
    }
    
    //cout << "read_key -> key = " << key << endl;
}

int main(int argc, char * argv[]) {

    ifstream infile;
    ofstream outfile;
    streampos size;
    streampos ciphersize;
    unsigned char *plaintext;
    unsigned char *ciphertext;
    unsigned char *key;

    if (argc != 4) {
        cout << "usage:des_encode infile outfile key" << endl;
    } else {
        infile.open(argv[1], ios::in | ios::binary | ios::ate);
        outfile.open(argv[2], ios::out | ios::binary);

        if (infile.is_open()) {
            size = infile.tellg();
            
            if(size % 8 == 0) {
                ciphersize = size;
            } else {
                ciphersize = size + 8 - (size % 8);
            }
            
            plaintext = new unsigned char[size];
            ciphertext = new unsigned char[ciphersize];
            infile.seekg(0, ios::beg);
            infile.read((char*)plaintext, size);
            
            read_key(argv[3], key);
            
            des_encryption(plaintext, key, ciphertext, size);

            outfile.write((char*)ciphertext, ciphersize);
            cout << "cipher text stored in: " << argv[2] << endl;
        } else {
            cout << "Unable to open file " << argv[1] << endl;
        }

        infile.close();
        outfile.close();
    }
}