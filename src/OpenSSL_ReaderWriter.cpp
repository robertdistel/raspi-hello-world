/*
 * OpenSSL_ReaderWriter.cpp
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#include "OpenSSL_ReaderWriter.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <iostream>
#include <cstring>




static byte key[] = "12345678901234567890123456789012";


pt_buffer  Encryptor::exec (pt_buffer ptext) //encrypt a block
{
  evp_cipher_ctx_st * ctx = EVP_CIPHER_CTX_new ();

  int rc = EVP_EncryptInit_ex (ctx, EVP_aes_256_cbc (), NULL, key, iv);
  if (rc != 1)
    {
      std::cerr << ("EVP_EncryptInit_ex failed") << std::endl;
      exit (-1);
    }

  rc = EVP_CIPHER_CTX_set_padding (ctx, 1);
  if (rc != 1)
    {
      std::cerr << ("EVP_CIPHER_CTX_set_padding failed") << std::endl;
      exit (-1);
    }

  pt_buffer ctext (new std::vector<char> (ptext->size () + BLOCK_SIZE));
  // Cipher text expands upto BLOCK_SIZE
  int out_len1 = (int) ctext->size ();

  rc = EVP_EncryptUpdate (ctx, (byte*) ctext->data (), &out_len1, (const byte*) ptext->data (), (int) ptext->size ());
  if (rc != 1)
    {
      std::cerr << "EVP_EncryptUpdate failed" << std::endl;
      exit (-1);
    }

  int out_len2 = (int) ctext->size () - out_len1;
  rc = EVP_EncryptFinal_ex (ctx, (byte*) ctext->data () + out_len1, &out_len2);
  if (rc != 1)
    {
      std::cerr << "EVP_EncryptFinal_ex failed" << std::endl;
      exit (-1);
    }

  // Set cipher text size now that we know it
  ctext->resize (out_len1 + out_len2);
  //and set the iv to use next as the start of this cypher text
  memcpy (iv, ctext->data (), BLOCK_SIZE);

  EVP_CIPHER_CTX_free (ctx);

  return ctext;
}

Encryptor::Encryptor ()
{

  EVP_add_cipher (EVP_aes_256_cbc ());

  memcpy (iv, "1234567890123456", BLOCK_SIZE);

}

Encryptor::~Encryptor ()
{
}


pt_buffer  Decryptor::exec (pt_buffer ctext) //decrypt a block
{
  auto ctx = EVP_CIPHER_CTX_new ();

  int rc = EVP_DecryptInit_ex (ctx, EVP_aes_256_cbc (), NULL, key, iv);

  if (rc != 1)
    {
      std::cerr << "EVP_DecryptInit_ex failed" << std::endl;
      exit (-1);
    }

  rc = EVP_CIPHER_CTX_set_padding (ctx, 1);
  if (rc != 1)
    {
      std::cerr << ("EVP_CIPHER_CTX_set_padding failed") << std::endl;
      exit (-1);
    }

  // Recovered text contracts upto BLOCK_SIZE
  pt_buffer rtext (new std::vector<char> (ctext->size ()));
  int out_len1 = (int) rtext->size ();

  rc = EVP_DecryptUpdate (ctx, (byte*) rtext->data (), &out_len1, (const byte*) ctext->data (), (int) ctext->size ());
  if (rc != 1)
    {
      std::cerr << "EVP_DecryptUpdate failed" << std::endl;
      exit (-1);
    }

  int out_len2 = (int) rtext->size () - out_len1;
  rc = EVP_DecryptFinal_ex (ctx, (byte*) rtext->data () + out_len1, &out_len2);
  if (rc != 1)
    {
      std::cerr << "EVP_DecryptFinal_ex failed" << std::endl;
      exit (-1);
    }

  // Set recovered text size now that we know it
  rtext->resize (out_len1 + out_len2);

  memcpy (iv, ctext->data (), BLOCK_SIZE);

  return rtext;
}

Decryptor::Decryptor ()
{
  EVP_add_cipher (EVP_aes_256_cbc ());
  memcpy (iv, "1234567890123456", BLOCK_SIZE);
}

Decryptor::~Decryptor ()
{

}


