#include <stdio.h>
#include<string>
#include <openssl/evp.h>
#include<sstream>
#include"Hash.h"

using namespace std;

unsigned char* Hash (void *data, size_t length)
{
  EVP_MD_CTX *mdctx;
  const EVP_MD *md;
  unsigned char* md_value = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
  unsigned int md_len;

  OpenSSL_add_all_digests();
  md = EVP_get_digestbyname("MD5");

  if(!md) {
    cerr <<"Unknown message digest";
    return NULL;
  }

  mdctx = EVP_MD_CTX_create();
  EVP_DigestInit_ex(mdctx, md, NULL);
  EVP_DigestFinal_ex(mdctx, md_value, &md_len);
  EVP_MD_CTX_destroy(mdctx);

  md_value[md_len] = '\0';
  return md_value;
}
