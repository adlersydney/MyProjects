#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <openssl/evp.h>
#include <string>
#include <iostream>

using namespace std;
unsigned char* Hash (void *data, size_t length);

#endif
