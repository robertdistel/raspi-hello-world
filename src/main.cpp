/*
 * test1.cpp
 *
 *  Created on: 11 Jul. 2017
 *      Author: robert
 */

#include "BaseReaderWriter.h"
#include "OpenSSL_Wrapper.h"
#include "UdpSocketReaderWriter.h"
#include "AlsaReaderWriter.h"
#include "StdioReaderWriter.h"
#include "OpenSSL_ReaderWriter.h"

#include "ObjectFactory.h"
#include "ReaderWriterThread.h"

#include <iostream>
#include <vector>
#include <boost/program_options.hpp>




unsigned int global_block_size = 0; //in bytes
unsigned int global_block_rate = 0; //in blocks per second
std::string global_log_file_name;

namespace po = boost::program_options;






ObjectFactory object_factory =
    ObjectFactory () (ObjectFactory::Item<Stdio> ("stdio"))
	(ObjectFactory::Item<CryptoWrapper<Stdio, Encryptor> > ("encrypt-stdio"))
	(ObjectFactory::Item<CryptoWrapper<Stdio, Decryptor> > ("decrypt-stdio"))
	(ObjectFactory::Item<File> ("file"))
	(ObjectFactory::Item<CryptoWrapper<File, Encryptor> > ("encrypt-file"))
	(ObjectFactory::Item<CryptoWrapper<File, Decryptor> > ("decrypt-file"))
	(ObjectFactory::Item<UdpSocket> ("udp"))
	(ObjectFactory::Item<CryptoWrapper<UdpSocket, Encryptor> > ("encrypt-udp"))
	(ObjectFactory::Item<CryptoWrapper<UdpSocket, Decryptor> > ("decrypt-udp"))
	(ObjectFactory::Item<Alsa> ("alsa"))
	(ObjectFactory::Item<CryptoWrapper<Alsa, Encryptor> > ("encrypt-alsa"))
	(ObjectFactory::Item<CryptoWrapper<Alsa, Decryptor> > ("decrypt-alsa"));




int
main (int argc, char** argv)
{


  OpenSSL openssl_singleton;

  po::options_description description ("MyTool Usage");
  std::vector<std::string> reader_command_line;
  std::vector<std::string> writer_command_line;
  unsigned int reporting_interval;
  unsigned int report_time;
  unsigned int sample_rate;

  description.add_options () ("reader", po::value<std::vector<std::string> > (&reader_command_line), "reader and reader options ") (
      "writer", po::value<std::vector<std::string> > (&writer_command_line), "writer and writer options ") (
      "block-size,b", po::value<unsigned int> (&global_block_size)->default_value (256), "size of blocks to read/write") (
      "sample-rate,s", po::value<unsigned int> (&sample_rate), "data rate in k bytes per second") (
      "block-rate,r", po::value<unsigned int> (&global_block_rate)->default_value (100), "blocks per second") (
      "report-interval,i", po::value<unsigned int> (&reporting_interval)->default_value (1), "interval between report updates in seconds") (
      "report-time,t", po::value<unsigned int> (&report_time)->default_value (10), "interval between report updates in seconds") (
      "tx-log", po::value<std::string> (&global_log_file_name), "log file to write actual input data into") ("help,h", "Display this help message") (
      "version,v", "Display the version number");
  po::variables_map vm;
  po::store (po::command_line_parser (argc, argv).options (description).run (), vm);
  po::notify (vm);

  if (vm.count ("sample-rate") == 1)
    {
      global_block_size = (1000 * sample_rate) / global_block_rate;
    }

  if (vm.count ("writer") == 0)
    {
      std::cerr << "At least one writer is required" << std::endl;
      return -1;
    }

  if (vm.count ("reader") != vm.count ("writer"))
    {
      std::cerr << "equal numbers of readers and writers required" << std::endl;
      return -1;
    }

  if (vm.count ("help"))
    {
      std::cout << description;
      return -1;

    }

  if (vm.count ("version"))
    {
      std::cout << "MyTool Version 1.0" << std::endl;
      return -1;
    }

  std::cout << "Run with block size " << global_block_size << std::endl;
  std::cout << "         block rate " << global_block_rate << std::endl;
  std::cout << "    report interval " << reporting_interval << std::endl;
  std::cout << "        report time " << report_time << std::endl;

  std::vector<ReaderWriterThread> rwt;
  for (auto i = reader_command_line.begin (), j = writer_command_line.begin (); i != reader_command_line.end () && j != writer_command_line.end (); i++, j++)
    {
      std::cout << "Starting pipe " << *i << " -> " << *j << std::endl;
      rwt.push_back (ReaderWriterThread (object_factory.make<BaseReaderWriter> (*i), object_factory.make<BaseReaderWriter> (*j)));
      rwt.back ().start_thread ();
    }

  for (unsigned int i = 0; i < report_time && !ReaderWriterThread::global_halt_threads; i++)
    {
      sleep (reporting_interval);
      std::cout << "Input 	Packets " << rwt.front ().packet_count () << " Bytes " << rwt.front ().byte_count ();
      std::cout << "|Output	Packets " << rwt.back ().packet_count () << " Bytes " << rwt.back ().byte_count () << std::endl;
    };

  ReaderWriterThread::global_halt_threads = 1;

  for (auto j = rwt.begin (); j != rwt.end (); j++)
    {
      j->join ();
    }

  std::cout << "Input 	" << rwt.front ().packet_count () / report_time << " pps. " << rwt.front ().byte_count () / (1024 * report_time) << "KBps" << std::endl;
  std::cout << "Output	 " << rwt.back ().packet_count () / report_time << " pps. " << rwt.back ().byte_count () / (1024 * report_time) << "KBps" << std::endl;

  return 0;
}

