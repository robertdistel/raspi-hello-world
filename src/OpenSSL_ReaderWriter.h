/*
 * OpenSSL_ReaderWriter.h
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#ifndef OPENSSL_READERWRITER_H_
#define OPENSSL_READERWRITER_H_

#include <string>
#include "BaseReaderWriter.h"


static const unsigned int BLOCK_SIZE = 16;
typedef unsigned char byte;




template<class T, class C>
  class CryptoWrapper : public T
  {
  public:
    CryptoWrapper (void) :
	T ()
    {
    }

    CryptoWrapper (std::string parameters) :
	T (parameters)
    {
    }

    virtual void
    Write (pt_buffer buff)
    {
      T::Write (ctx.exec (buff));
    }
    virtual pt_buffer
    Read (void)
    {
      return ctx.exec (T::Read ());
    }

    virtual
    ~CryptoWrapper ()
    {
    }

    C ctx;
  private:

  };

class Encryptor
{

public:
  pt_buffer  exec (pt_buffer ptext);
  Encryptor ();
  ~Encryptor ();
private:
  byte iv[BLOCK_SIZE];

};

class Decryptor
{
public:

  pt_buffer  exec (pt_buffer ctext) ;
  Decryptor ();
  ~Decryptor ();
private:
  byte iv[BLOCK_SIZE];

};




#endif /* OPENSSL_READERWRITER_H_ */
