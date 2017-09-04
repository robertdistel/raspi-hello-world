/*
 * UdpSocketReaderWriter.cpp
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#include "UdpSocketReaderWriter.h"
#include "GlobalConfig.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>          /* hostent struct, gethostbyname() */
#include <unistd.h>

#include <regex>
#include <iostream>
#include <stdlib.h>
#include <string>

UdpSocket:: UdpSocket (void) :
s (0), socket_details ()
{
}

UdpSocket::UdpSocket (std::string socket_details_) :
	  s (0), socket_details (socket_details_)
{
}


pt_buffer
UdpSocket::Read ()
{
  pt_buffer buff = std::shared_ptr<std::vector<char> > (new std::vector<char> (2*global_block_size )); //allocate extra recv space incase encryption has made it grow

  if (!s)
    {
      struct sockaddr_in reader_socket_addr;

      if ((s = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
	  std::cerr << "socket failed" << std::endl;
	  exit (-1);
	}
      /* Construct the server sockaddr_in structure */
      memset (&reader_socket_addr, 0, sizeof(reader_socket_addr)); /* Clear struct */
      reader_socket_addr.sin_family = AF_INET; /* Internet/IP */
      reader_socket_addr.sin_addr.s_addr = htonl (INADDR_ANY); /* Incoming addr */
      reader_socket_addr.sin_port = htons (atoi (socket_details.c_str ())); /* server port */

      if (bind (s, (struct sockaddr *) &reader_socket_addr, sizeof(reader_socket_addr)) < 0)
	{
	  std::cerr << "bind failed" << std::endl;
	  exit (-1);
	}
    }

  if (!s)
    {
      std::cerr << "Couldn't open recv udp port on port " << socket_details << std::endl;
      exit (-1);
    }

  int recieved_bytes (0);

  if ((recieved_bytes = recv (s, buff->data (), buff->size (), 0)) < 0)
    {
      recieved_bytes = 0;
      //most likely we got a SIG_INTR to unblock waiting for data.
    }
  buff->resize (recieved_bytes); //and resize buffer to reflect how many bytes were really read
  return buff;
}

void
UdpSocket::Write (pt_buffer buff)
{
  if (!s)
    {
      std::smatch sm;
      std::regex rex ("([^:]*):(.*)");
      std::regex_match (socket_details, sm, rex);
      auto destination_name = std::string (sm[1]);
      auto port = atoi (std::string (sm[2]).c_str ());

      struct sockaddr_in writer_destination_addr;

      if ((s = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
	  std::cerr << "open socket failed" << std::endl;
	  exit (-1);
	}

      struct hostent *host; /* host information */
      if ((host = gethostbyname (destination_name.c_str ())) == NULL)
	{
	  std::cerr << "name lookup failed on '" << destination_name << ":" << port << "'" << std::endl;
	  exit (1);
	}

      /* Construct the server sockaddr_in structure */
      memset (&writer_destination_addr, 0, sizeof(writer_destination_addr)); /* Clear struct */
      writer_destination_addr.sin_family = AF_INET; /* Internet/IP */
      writer_destination_addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]); /* Incoming addr */
      writer_destination_addr.sin_port = htons (port); /* server port */

      if (connect (s, (struct sockaddr *) &writer_destination_addr, sizeof(writer_destination_addr)) < 0)
	{
	  std::cerr << "connect failed" << std::endl;
	  exit (-1);
	}

    }

  if (!s)
    {
      std::cerr << "Couldn't open tx udp port on '" << socket_details << "'" << std::endl;
      exit (-1);
    }
  ssize_t rc=0;
  if ((rc = send (s, buff->data (), buff->size (), 0)) != int (buff->size ()))
    {
      if (rc<0)
	std::cerr << "Send failed with erno " << strerror(errno) << std::endl;
      else
	std::cerr << "Send sent to little data. sent "<< rc << " expected " <<  buff->size () << std::endl;
    }
}

UdpSocket::~UdpSocket ()
{
  if (s)
    close (s);
}


