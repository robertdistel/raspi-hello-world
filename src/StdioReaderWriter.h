/*
 * StdioReaderWriter.h
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#ifndef STDIOREADERWRITER_H_
#define STDIOREADERWRITER_H_

#include "BaseReaderWriter.h"

class File : public BaseReaderWriter
{
  static const int nanoseconds_per_second = 1000000000;

public:
  File (void);
  File (std::string filename_);

  virtual void  Write (pt_buffer buff);

  virtual pt_buffer  Read ();
  virtual ~File ();
private:
  std::string filename;
  FILE* f;
  FILE* log_f;
  struct timespec ts;

};

class Stdio : public BaseReaderWriter
{
public:
  Stdio (void);

  Stdio (std::string);

  virtual void  Write (pt_buffer buff);

  virtual pt_buffer  Read ();

  virtual ~Stdio ();

};




#endif /* STDIOREADERWRITER_H_ */
