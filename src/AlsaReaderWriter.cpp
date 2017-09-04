/*
 * AlsaReaderWriter.cpp
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */


#include "AlsaReaderWriter.h"
#include "GlobalConfig.h"
#include <iostream>

  int Alsa::configure_alsa_audio (snd_pcm_t *device)
  {
    snd_pcm_hw_params_t *hw_params;
    int err;
    unsigned int tmp;

    /* allocate memory for hardware parameter structure */
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
      {
	fprintf (stderr, "cannot allocate parameter structure (%s)\n", snd_strerror (err));
	return 1;
      }
    /* fill structure from current audio parameters */
    if ((err = snd_pcm_hw_params_any (device, hw_params)) < 0)
      {
	fprintf (stderr, "cannot initialize parameter structure (%s)\n", snd_strerror (err));
	return 1;
      }

    /* set access type, sample rate, sample format, channels */
    if ((err = snd_pcm_hw_params_set_access (device, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
      {
	fprintf (stderr, "cannot set access type: %s\n", snd_strerror (err));
	return 1;
      }
    // bits = 16
    if ((err = snd_pcm_hw_params_set_format (device, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
      {
	fprintf (stderr, "cannot set sample format: %s\n", snd_strerror (err));
	return 1;
      }
    tmp = sample_rate;
    if ((err = snd_pcm_hw_params_set_rate_near (device, hw_params, &tmp, 0)) < 0)
      {
	fprintf (stderr, "cannot set sample rate: %s\n", snd_strerror (err));
	return 1;
      }
    if (tmp != sample_rate)
      {
	fprintf (stderr, "Could not set requested sample rate, asked for %d got %d\n", sample_rate, tmp);
	sample_rate = tmp;
      }
    if ((err = snd_pcm_hw_params_set_channels (device, hw_params, channels)) < 0)
      {
	fprintf (stderr, "cannot set channel count: %s\n", snd_strerror (err));
	return 1;
      }

    std::cerr << "period time requested = " << latency << " us." << std::endl;
    if ((err = snd_pcm_hw_params_set_period_time_near (device, hw_params, &latency, 0)) < 0)
      {
	fprintf (stderr, "snd_pcm_hw_params_set_period_time_near %d: %s\n", latency, snd_strerror (err));
	return 1;
      }

    std::cerr << "buffer time requested = " << max_time_to_xflow << " us." << std::endl;
    if ((err = snd_pcm_hw_params_set_buffer_time_near (device, hw_params, &max_time_to_xflow, 0)) < 0)
      {
	fprintf (stderr, "snd_pcm_hw_params_set_buffer_time_near %d: %s\n", max_time_to_xflow, snd_strerror (err));
	return 1;
      }

    /*
     //divides up the buffer into a number of fragments
     if ((err = snd_pcm_hw_params_set_periods_near(device, hw_params, &fragments, 0)) < 0)
     {
     fprintf(stderr, "Error setting # fragments to %d: %s\n", fragments,
     snd_strerror(err));
     return 1;
     }

     frames = buffer_size / (frame_size * fragments);
     if ((err = snd_pcm_hw_params_set_buffer_size_near(device, hw_params, &frames)) < 0)
     {
     std::cerr << "Error setting buffer_size " << frames << " frames: " << snd_strerror(err) << std::endl;
     return 1;
     }
     if (buffer_size != frames * frame_size * fragments)
     {
     std::cerr << "Could not set requested buffer size, asked for " << buffer_size << " got " << frames * frame_size * fragments << std::endl;
     buffer_size = frames * frame_size / fragments;
     }
     */

    if ((err = snd_pcm_hw_params (device, hw_params)) < 0)
      {
	fprintf (stderr, "Error setting HW params: %s\n", snd_strerror (err));
	return 1;
      }

    unsigned int t;
    if ((err = snd_pcm_hw_params_get_period_time (hw_params, &t, 0)) < 0)
      {
	fprintf (stderr, "snd_pcm_hw_params_get_period_time %d: %s\n", latency, snd_strerror (err));
	return 1;
      }

    std::cerr << "Period time set = " << t << " us." << std::endl;
    if ((err = snd_pcm_hw_params_get_buffer_time (hw_params, &t, 0)) < 0)
      {
	fprintf (stderr, "snd_pcm_hw_params_get_buffer_time %d: %s\n", t, snd_strerror (err));
	return 1;
      }

    std::cerr << "Buffer time set = " << t << " us." << std::endl;

    snd_pcm_uframes_t frames;
    if ((err = snd_pcm_hw_params_get_period_size (hw_params, &frames, 0)) < 0)
      {
	fprintf (stderr, "snd_pcm_hw_params_set_period_time_near %d: %s\n", latency, snd_strerror (err));
	return 1;
      }
    else
      {
	std::cerr << "Period size = " << frames << " frames (" << snd_pcm_frames_to_bytes (device, frames) << " bytes )" << std::endl;
      }

    if ((err = snd_pcm_hw_params_get_buffer_size (hw_params, &frames)) < 0)
      {
	fprintf (stderr, "snd_pcm_hw_params_set_period_time_near %d: %s\n", latency, snd_strerror (err));
	return 1;
      }
    else
      {
	std::cerr << "Buffer size = " << frames << "frames (" << snd_pcm_frames_to_bytes (device, frames) << " bytes )" << std::endl;
      }

    snd_pcm_sw_params_t *swparams;
    snd_pcm_sw_params_alloca(&swparams); //allocated on the stack
    err = snd_pcm_sw_params_current (device, swparams);
    if (err < 0)
      {
	printf ("Unable to determine current swparams %s\n", snd_strerror (err));
	return err;
      }
    if ((err = snd_pcm_hw_params_get_period_size (hw_params, &frames, 0)) < 0)
      {
	fprintf (stderr, "snd_pcm_hw_params_get_period_size %s\n", snd_strerror (err));
	return 1;
      }


//    if ((err = snd_pcm_sw_params_set_start_threshold (device, swparams,  frames )) < 0)
//      {
//	std::cout << "snd_pcm_sw_params_set_start_threshold "<<   frames << ":" <<  snd_strerror (err) << std::endl;
//	return 1;
//      }

//    snd_pcm_nonblock(device, 0); //we run in blocking mode

    return 0;
  }


  Alsa::Alsa () :
      capture_device_name (), playback_device_name (), capture_device_handle (nullptr), playback_device_handle (nullptr),  sample_rate (
	  global_block_size * global_block_rate / (channels * bits / 8)), max_time_to_xflow (8 * sec / global_block_rate), latency ( sec / (2*global_block_rate)), underrun_count (
	  0), overrun_count (0),log_f(nullptr)
  {
  }

  Alsa::Alsa (std::string device_name_) :
      capture_device_name (device_name_), playback_device_name (device_name_), capture_device_handle (nullptr), playback_device_handle (nullptr), sample_rate (
	  global_block_size * global_block_rate / (channels * bits / 8)), max_time_to_xflow (8 * sec / global_block_rate), latency ( sec / (2*global_block_rate)), underrun_count (
	  0), overrun_count (0),log_f(nullptr)
  {
  }



  Alsa::~Alsa ()
  {
    if (capture_device_handle)
      {
	snd_pcm_drop (capture_device_handle);
	snd_pcm_close (capture_device_handle);
	std::cerr << "Overrun count = " << overrun_count << std::endl;
      }
    if (playback_device_handle)
      {
	snd_pcm_drop (playback_device_handle);
	snd_pcm_close (playback_device_handle);
	std::cerr << "Underrun count = " << underrun_count << std::endl;
      }
  }

  pt_buffer  Alsa::Read (void)
  {
    if (!capture_device_handle)
      {
	int err;

	if (((err = snd_pcm_open (&capture_device_handle, capture_device_name.c_str (), SND_PCM_STREAM_CAPTURE, 0)) < 0)
	    || configure_alsa_audio (capture_device_handle) != 0)
	  {
	    fprintf (stderr, "cannot open audio capture device %s: %s\n", capture_device_name.c_str (), snd_strerror (err));
	    exit (1);
	  }
	snd_pcm_prepare (capture_device_handle);
	if (!global_log_file_name.empty ())
	  {
	    log_f = fopen (global_log_file_name.c_str (), "w");
	    if (!log_f)
	      {
		std::cerr << "Could not open log " << global_log_file_name << std::endl;
		exit (-1);

	      }
	  }
      }

    pt_buffer buff = std::shared_ptr<std::vector<char> > (new std::vector<char> (global_block_size * 2)); //buffersize may be reset during initialisation based on the capabilities of the soundcard

    unsigned int frames = snd_pcm_bytes_to_frames (capture_device_handle, global_block_size);
    int inframes;
    while ((inframes = snd_pcm_readi (capture_device_handle, buff->data (), frames)) < 0)
      {
	if (inframes == -EAGAIN)
	  continue;
	else if (inframes == -EPIPE)
	  {
	    //this is an over run - we flooded the device by not reading quickly enough
	    //fprintf(stderr, "Output buffer underrun\n");
	    //restarting = 1;
	    snd_pcm_prepare (capture_device_handle);
	    overrun_count++;
	  }
	else
	  {
	    std::cerr << "Unknown read error : " << snd_strerror (inframes) << std::endl;
	  }
      }
    if (static_cast<snd_pcm_uframes_t> (inframes) != frames)
      std::cerr << "Short read from capture device: " << inframes << ", expecting " << frames << std::endl;

    buff->resize (snd_pcm_frames_to_bytes (capture_device_handle, inframes));

    if (log_f)		//and record what we readin
      fwrite (buff->data (), sizeof(char), buff->size (), log_f);

    return buff;

  }

   void Alsa::Write (pt_buffer buff)
  {
    if (!playback_device_handle)
      {
	int err;

	if (((err = snd_pcm_open (&playback_device_handle, playback_device_name.c_str (), SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	    || configure_alsa_audio (playback_device_handle) != 0)
	  {
	    fprintf (stderr, "cannot open audio playback device %s: %s\n", playback_device_name.c_str (), snd_strerror (err));
	    exit (1);
	  }
	  snd_pcm_prepare (playback_device_handle);
      }

    unsigned int frames = snd_pcm_bytes_to_frames (playback_device_handle, buff->size ());
    //assert(snd_pcm_frames_to_bytes (playback_device_handle, frames) == buff->size ());
//    snd_pcm_status_t* status;
//    snd_pcm_status_alloca(&status);
//
//    snd_pcm_status(playback_device_handle,status);
//    std::cerr << "snd_pcm_status_get_avail " << snd_pcm_status_get_avail(status) ;
//    std::cerr << "   snd_pcm_status_get_state" << snd_pcm_status_get_state( status ) << std::endl;


    int outframes;
    while ((outframes = snd_pcm_writei (playback_device_handle, buff->data (), frames)) < 0)
      {
	if (outframes == -EAGAIN)
	  {
	    continue;
	  }
	else if (outframes == -EPIPE)
	  {
	    //this is an under run - we starved the device
	    //fprintf(stderr, "Output buffer underrun\n");
	    //restarting = 1;
	    snd_pcm_prepare (playback_device_handle);
	    underrun_count++;
	  }
	else
	  {
	    std::cerr << "Unknown write error : " << snd_strerror (outframes) << std::endl;
	  }
      }
    if (static_cast<size_t> (outframes) != frames)
      std::cerr << "Short write to playback device: " << outframes << ", expecting, " << frames << std::endl;

  }





