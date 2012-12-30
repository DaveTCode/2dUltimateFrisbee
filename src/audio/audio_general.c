/*
 * audio_general.c
 *
 *  Created on: 14 Apr 2010
 *      Author: David
 */
#include "../dt_logger.h"

#include <stddef.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "audio_general.h"
#include <strmap\strmap.h>

/*
 * audio_system_error
 *
 * Converts alc errors into strings for printing to debug logs. Retrieves the
 * most recent value from alcGetError to do this.
 *
 * Returns: A string containing error information.
 */
char *audio_system_error(ALCdevice *device)
{
  /*
   * Local Variables.
   */
  ALCenum error = alcGetError(device);

  switch(error)
  {
    case ALC_NO_ERROR:
      return("OpenAL error check: No error");
    case ALC_INVALID_DEVICE:
      return("OpenAL error check: Invalid device");
    case ALC_INVALID_CONTEXT:
      return("OpenAL error check: Invalid context");
    case ALC_INVALID_ENUM:
      return("OpenAL error check: Invalid enum");
    case ALC_INVALID_VALUE:
      return("OpenAL error check: Invalid value");
    case ALC_OUT_OF_MEMORY:
      return("OpenAL error check: Out of memory");
    default:
      return("OpenAL error check: Unknown error");
  }

  return("");
}

/*
 * init_audio_subsystem
 *
 * Sets up the program for playing audio over SDL_Mixer.
 *
 * Parameters: config_table - We pass in the config so that the frequency and
 *                            number of channels can be set by config file.
 *
 * Returns: One of INIT_AUDIO_SYSTEM_RET_CODES.
 */
int init_audio_subsystem(StrMap *config_table)
{
  /*
   * Local Variables.
   */
  int ret_code = INIT_AUDIO_SYSTEM_OK;
  ALCdevice *audio_device;
  ALCcontext *context;

  /*
   * Set up the device and context for openal.
   */
  audio_device = alcOpenDevice(NULL);
  if (NULL == audio_device)
  {
    DT_DEBUG_LOG("Failed to set up openal audio device: %s",
                 audio_system_error(NULL));
    ret_code = INIT_AUDIO_SYSTEM_DEVICE_FAILED;
    goto EXIT_LABEL;
  }
  context = alcCreateContext(audio_device, NULL);
  if (NULL == context)
  {
    DT_DEBUG_LOG("Failed to create openal context: %s",
                 audio_system_error(NULL));
    ret_code = INIT_AUDIO_SYSTEM_CONTEXT_FAILED;
    goto EXIT_LABEL;
  }
  alcMakeContextCurrent(context);

EXIT_LABEL:

  return(ret_code);
}

/*
 * close_audio_subsystem
 *
 * Use this on program exit to close down the audio subsystem gracefully.
 *
 * TODO: Error handling for close audio system
 */
int close_audio_subsystem()
{
  ALCcontext *context = alcGetCurrentContext();
  ALCdevice *device = alcGetContextsDevice(context);
  alcDestroyContext(context);
  alcCloseDevice(device);

  return 0;
}
