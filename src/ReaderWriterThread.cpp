/*
 * ReaderWriterThread.cpp
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */
#include "ReaderWriterThread.h"
#include <thread>

  class Context
  {
  public:
    Context () :
	packet_count (0), byte_count (0)
    {
    }

    Context (std::shared_ptr<BaseReaderWriter> Reader_, std::shared_ptr<BaseReaderWriter> Writer_) :
	ctx (), packet_count (0), byte_count (0), Reader (Reader_), Writer (Writer_)
    {
    }


    std::thread ctx;
    unsigned long packet_count;
    unsigned long byte_count;
    std::shared_ptr<BaseReaderWriter> Reader;
    std::shared_ptr<BaseReaderWriter> Writer;
    static void do_thread (std::shared_ptr<Context> pctx);
  };


bool ReaderWriterThread::global_halt_threads = false;

void Context::do_thread (std::shared_ptr<Context> pctx)
    {
      pt_buffer buf;
      do
	{
	  buf = pctx->Reader->Read ();  //we assume read blocks to limit the rate as necessary
	  pctx->Writer->Write (buf);
	  pctx->packet_count++;
	  pctx->byte_count += buf->size ();
	}
      while (buf->size () > 0 && !ReaderWriterThread::global_halt_threads);
    }



  ReaderWriterThread::ReaderWriterThread () :
      pcontext (new Context)
  {
  }


  ReaderWriterThread::ReaderWriterThread (std::shared_ptr<BaseReaderWriter> ReadFunctor_, std::shared_ptr<BaseReaderWriter> WriteFunctor_) :
      pcontext (new Context (ReadFunctor_, WriteFunctor_))
  {
  }


  void
  ReaderWriterThread::start_thread ()
  {
    pcontext->ctx = std::thread (Context::do_thread, pcontext);
  }


  void
  ReaderWriterThread::join (void)
  {
    return pcontext->ctx.join ();
  }

  unsigned long ReaderWriterThread::byte_count (void)
   {
     return pcontext->byte_count;
   }
   unsigned long ReaderWriterThread::packet_count (void)
   {
     return pcontext->packet_count;
   }



