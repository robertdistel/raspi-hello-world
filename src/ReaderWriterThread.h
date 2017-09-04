/*
 * ReaderWriterThread.h
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#ifndef READERWRITERTHREAD_H_
#define READERWRITERTHREAD_H_

#include <memory>

#include "BaseReaderWriter.h"


class Context;

class ReaderWriterThread
{
private:

  std::shared_ptr<Context> pcontext;

public:

  ReaderWriterThread () ;


  ReaderWriterThread (std::shared_ptr<BaseReaderWriter> ReadFunctor_, std::shared_ptr<BaseReaderWriter> WriteFunctor_) ;


  void start_thread ();

  unsigned long byte_count (void);
  unsigned long packet_count (void);
  void  join (void);

  static bool global_halt_threads;

};




#endif /* READERWRITERTHREAD_H_ */
