/*
 * config_map.h
 *
 *  Created on: 4 Apr 2010
 *      Author: David
 */

#ifndef CONFIG_MAP_H_
#define CONFIG_MAP_H_


#include <strmap/strmap.h>

/*
 * The maximum length of a value in the config file. Arbitrary character array
 * size.
 */
#define MAX_CONFIG_VALUE_LEN 1024

/*
 * cv_max_fps - The desired fps.
 * cv_animation_ms_per_frame - The default ms per animation frame.
 * TODO: Fix comment with extra config values.
 */
typedef enum config_value_int_enum
{
  cv_max_fps,
  cv_animation_ms_per_frame,
  cv_audio_freq,
  cv_audio_channels
} CONFIG_VALUE_INT_ENUM;

/*
 * cv_music_directory - The location where music files for the game are found.
 * cv_o_xml_file - The location of the offensive automaton xml file.
 * cv_d_xml_file - The location of the defensive autoamton xml file.
 * cv_disc_graphic - The path and filename for the disc graphic.
 * cv_grass_tile_filename - The path and filename for the grass tile graphic.
 */
typedef enum config_value_str_enum
{
  cv_music_directory,
  cv_o_xml_file,
  cv_d_xml_file,
  cv_disc_graphic,
  cv_grass_tile_filename
} CONFIG_VALUE_STR_ENUM;

/*
 * CONFIG_VALUE_INT
 *
 * Corresponds to a single value in the config file which has type int.
 */
typedef struct config_value_int
{
  char key[MAX_CONFIG_VALUE_LEN];
  int min_value;
  int max_value;
  int default_value;
} CONFIG_VALUE_INT;

/*
 * CONFIG_VALUE_STR
 *
 * Corresponds to a single value in the config file which has type string.
 */
typedef struct config_value_str
{
  char key[MAX_CONFIG_VALUE_LEN];
  char default_value[MAX_CONFIG_VALUE_LEN];
} CONFIG_VALUE_STR;

bool create_config_value_int(CONFIG_VALUE_INT_ENUM, CONFIG_VALUE_INT *);
bool create_config_value_str(CONFIG_VALUE_STR_ENUM, CONFIG_VALUE_STR *);
bool get_config_value_int(StrMap *, CONFIG_VALUE_INT_ENUM, int *);
void set_config_value_int(StrMap *, CONFIG_VALUE_INT_ENUM, int);
bool get_config_value_str(StrMap *, CONFIG_VALUE_STR_ENUM, char *);
void set_config_value_str(StrMap *, CONFIG_VALUE_STR_ENUM, char *);

#endif /* CONFIG_MAP_H_ */
