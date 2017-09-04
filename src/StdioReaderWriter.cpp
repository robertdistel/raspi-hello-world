/*
 * StdioReaderWriter.cpp
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */
#include "StdioReaderWriter.h"
#include <iostream>
#include "GlobalConfig.h"
#include <stdlib.h>

File::File (void) :
filename ("empty"), f (nullptr), log_f (nullptr)
{
}

File::File (std::string filename_) :
	      filename (filename_), f (nullptr), log_f (nullptr) //get the time now
{
}

void File::Write (pt_buffer buff)
{

  if (!f)
    {
      f = fopen (filename.c_str (), "w");
      if (!f)
	{
	  std::cerr << "File" << filename << " not found" << std::endl;
	  exit (-1);
	}
    }

  fwrite (buff->data (), sizeof(char), buff->size (), f);
}

pt_buffer File::Read ()
{
  pt_buffer buff = std::shared_ptr<std::vector<char> > (new std::vector<char> (global_block_size));

  if (!f)
    {
      f = fopen (filename.c_str (), "r");
      if (!f)
	{
	  std::cerr << "File" << filename << " not found" << std::endl;
	  exit (-1);
	}

      if (!global_log_file_name.empty ())
	{
	  log_f = fopen (global_log_file_name.c_str (), "w");
	  if (!log_f)
	    {
	      std::cerr << "Could not open log " << global_log_file_name << std::endl;
	      exit (-1);

	    }
	}
      clock_gettime (CLOCK_MONOTONIC, &ts);		//initalise time
    }
  long delay = nanoseconds_per_second / global_block_rate;
  ts.tv_nsec += delay;			//and add the desired delay
  if (ts.tv_nsec > (nanoseconds_per_second - 1))	//deal with the overflow
    {
      ts.tv_nsec -= nanoseconds_per_second;
      ts.tv_sec += 1;
    }
  buff->resize (fread (buff->data (), sizeof(char), buff->size (), f)); //try and read the chunk size - resize buffer to reflect what really happened

  if (log_f)		//and record what we readin
    fwrite (buff->data (), sizeof(char), buff->size (), log_f);

  clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr); //wait any remaining time
  return buff;
}

File::~File ()
{
  if (f)
    fclose (f);
}


Stdio::Stdio (void)
{
}

Stdio::Stdio (std::string)
{ };

void Stdio::Write (pt_buffer buff)
{

  fwrite (buff->data (), sizeof(char), buff->size (), stdout); //block until required number of bytes written or descriptor closed
}

pt_buffer Stdio::Read ()
{
  pt_buffer buff = std::shared_ptr<std::vector<char> > (new std::vector<char> (global_block_size));

  buff->resize (fread (buff->data (), sizeof(char), buff->size (), stdin)); //block untill required number of bytes read or descriptor closed
  return buff;
}

Stdio::~Stdio ()
{
  fflush (stdout);
}



