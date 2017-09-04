/*
 * UdpSocketReaderWriter.h
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#ifndef UDPSOCKETREADERWRITER_H_
#define UDPSOCKETREADERWRITER_H_

#include "BaseReaderWriter.h"
#include <string>


class UdpSocket : public BaseReaderWriter
{
public:
  UdpSocket (void);

  UdpSocket (std::string socket_details_);


  virtual pt_buffer  Read ();
  virtual void  Write (pt_buffer buff);
  virtual ~UdpSocket ();
private:
  int s;
  std::string socket_details;
};





#endif /* UDPSOCKETREADERWRITER_H_ */
