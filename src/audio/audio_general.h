/*
 * audio_general.h
 *
 *  Created on: 14 Apr 2010
 *      Author: David
 */

#ifndef AUDIO_GENERAL_H_
#define AUDIO_GENERAL_H_

#include "AL/alc.h"
#include "strmap/strmap.h"

/*
 * Arbitrary limit on the number of music files that the game will load up.
 */
#define MAX_MUSIC_FILES 100

/*
 * Maximum size for the music filename length.
 */
#define MAX_MUSIC_FILENAME_LEN 1000

/*
 * INIT_AUDIO_SYSTEM_RET_CODES
 *
 * Return codes from the function init_audio_subsystem.
 *
 * INIT_AUDIO_SYSTEM_OK - Audio subsystem loaded ok.
 * INIT_AUDIO_SYSTEM_DEVICE_FAILED - Failed to set up audio device.
 * INIT_AUDIO_SYSTEM_CONTEXT_FAILED - Failed to create a new openal context.
 */
#define INIT_AUDIO_SYSTEM_OK             0
#define INIT_AUDIO_SYSTEM_DEVICE_FAILED  1
#define INIT_AUDIO_SYSTEM_CONTEXT_FAILED 2

char *audio_system_error(ALCdevice *);
int init_audio_subsystem(StrMap *);
int close_audio_subsystem();

#endif /* AUDIO_GENERAL_H_ */
