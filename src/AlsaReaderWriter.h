/*
 * AlsaReaderWriter.h
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#ifndef ALSAREADERWRITER_H_
#define ALSAREADERWRITER_H_

#include "BaseReaderWriter.h"
#include <alsa/asoundlib.h>


class Alsa : public BaseReaderWriter
{
private:
  const static unsigned int channels = 1;
  const static unsigned int bits = 16;
  const static unsigned int ms = 1000; //1000 us per ms
  const static unsigned int sec = 1000 * ms; //1000 us per ms

  std::string capture_device_name;
  std::string playback_device_name;
  snd_pcm_t* capture_device_handle;
  snd_pcm_t* playback_device_handle;

  unsigned int sample_rate;
  unsigned int max_time_to_xflow; //in us - this is how big the tx/rx buffer is, how long before it will underflow (transmitter)/ overflow (reciever)
  unsigned int latency; //in us - this is how frequently we get interrupts to poll the buffers - this sets the length of time between when data is received and when the subsystem can 'notice' it

  unsigned int underrun_count;
  unsigned int overrun_count;
  FILE* log_f;

  int  configure_alsa_audio (snd_pcm_t *device);

public:
  Alsa () ;

  Alsa (std::string device_name_) ;


  virtual  ~Alsa ();
  virtual pt_buffer  Read (void);
  virtual void  Write (pt_buffer buff);
};



#endif /* ALSAREADERWRITER_H_ */
