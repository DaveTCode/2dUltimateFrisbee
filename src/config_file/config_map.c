/*
 * config_map.c
 *
 *  Created on: 4 Apr 2010
 *      Author: David
 */

#include "../dt_logger.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "config_map.h"
#include "strmap/StrMap.h"

/*
 * create_config_value_int
 *
 * Each possible integer value from the config file should be contained in this
 * function with min, max and defaults.
 */
bool create_config_value_int(CONFIG_VALUE_INT_ENUM cv,
                             CONFIG_VALUE_INT *config_value)
{
  /*
   * Local Variables.
   */
  bool value_exists = true;

  switch(cv)
  {
    case cv_max_fps:
      config_value->default_value = 60;
      strncpy(config_value->key, "MAX_FPS", MAX_CONFIG_VALUE_LEN);
      config_value->min_value = 30;
      config_value->max_value = 100;
      break;
    case cv_animation_ms_per_frame:
      config_value->default_value = 100;
      strncpy(config_value->key, "ANIMATION_MS_PER_FRAMES", MAX_CONFIG_VALUE_LEN);
      config_value->min_value = 10;
      config_value->max_value = 10000;
      break;
    case cv_audio_freq:
      config_value->default_value = 44100;
      strncpy(config_value->key, "AUDIO_FREQ", MAX_CONFIG_VALUE_LEN);
      config_value->min_value = 44100;
      config_value->max_value = 44100;
      break;
    case cv_audio_channels:
      config_value->default_value = 2;
      strncpy(config_value->key, "AUDIO_CHANNELS", MAX_CONFIG_VALUE_LEN);
      config_value->min_value = 1;
      config_value->max_value = 2;
      break;
    default:
      DT_DEBUG_LOG("Request made for config value that does not exist: %i\n",
                   cv);
      value_exists = false;
      break;
  }

  return(value_exists);
}

/*
 * create_config_value_str
 *
 * Each possible string value in the string config enum should be included in
 * here with key name and default value.
 */
bool create_config_value_str(CONFIG_VALUE_STR_ENUM cv,
                             CONFIG_VALUE_STR *config_value)
{
  /*
   * Local Variables.
   */
  bool cv_exists = true;

  switch(cv)
  {
    case cv_music_directory:
      strncpy(config_value->key, "MUSIC_DIRECTORY", MAX_CONFIG_VALUE_LEN);
      strncpy(config_value->default_value,
              "resources//music",
              MAX_CONFIG_VALUE_LEN);
      break;
    case cv_o_xml_file:
      strncpy(config_value->key, "O_AUTOMATON", MAX_CONFIG_VALUE_LEN);
      strncpy(config_value->default_value,
              "resources//automaton//o_automaton.xml",
              MAX_CONFIG_VALUE_LEN);
      break;
    case cv_d_xml_file:
      strncpy(config_value->key, "D_AUTOMATON", MAX_CONFIG_VALUE_LEN);
      strncpy(config_value->default_value,
              "resources//automaton//d_automaton.xml",
              MAX_CONFIG_VALUE_LEN);
      break;
    case cv_disc_graphic:
      strncpy(config_value->key, "DISC_GRAPHIC_FILENAME", MAX_CONFIG_VALUE_LEN);
      strncpy(config_value->default_value,
              "resources//images//disc.png",
              MAX_CONFIG_VALUE_LEN);
      break;
    case cv_grass_tile_filename:
      strncpy(config_value->key, "GRASS_TILE_FILENAME", MAX_CONFIG_VALUE_LEN);
      strncpy(config_value->default_value,
              "resources//images//grass_tile.png",
              MAX_CONFIG_VALUE_LEN);
      break;
    default:
      DT_DEBUG_LOG("Request made for config value that does not exist: %i\n",
                   cv);
      cv_exists = false;
      break;
  }

  return(cv_exists);
}

/*
 * get_config_value_int
 *
 * Retrieves the current value of the config mapping for a given integer config
 * value. Clamped between minimum and maximum and uses the default if nothing
 * exists in the config file.
 *
 * Parameters: table - The config table as loaded from a file.
 *             cv - The value we are looking for.
 *             value - Will contain the return value.
 *
 * Returns: False if the function fails for any reason. This should only be due
 *          to programmer error in filling in the create_config_value_int
 *          function.
 */
bool get_config_value_int(StrMap *table, CONFIG_VALUE_INT_ENUM cv, int *value)
{
  /*
   * Local Variables.
   */
  CONFIG_VALUE_INT config_value;
  char config_value_str[MAX_CONFIG_VALUE_LEN];
  int temp_value;
  int rc;
  bool value_exists = true;

  /*
   * create_config_value returns a config value with INVALID_VALUE as the key
   * if the value is not a valid constant.
   */
  value_exists = create_config_value_int(cv, &config_value);
  if (value_exists)
  {
    /*
     * If the value doesn't exist in the config file then just use whatever
     * default already exists.
     */
    rc = strmap_get(table,
                    config_value.key,
                    config_value_str,
                    MAX_CONFIG_VALUE_LEN);
    if (0 == rc)
    {
      *value = config_value.default_value;
    }
    else
    {
      /*
       * Silently clamp the possible values of the config between the minimum
       * and maximum allowed for that constant.
       */
      temp_value = atoi(config_value_str);
      if (temp_value < config_value.min_value)
      {
        *value = config_value.min_value;
      }
      else if (temp_value > config_value.max_value)
      {
        *value = config_value.max_value;
      }
      else
      {
        *value = temp_value;
      }
    }
  }

  return value_exists;
}

/*
 * set_config_value_int
 *
 * Sets the value of the config key passed in. This can be used to write new
 * values back out to file.
 *
 * Fails silently when attempting to set a key that doesn't exist.
 *
 * Parameters: table - The config map.
 *             cv - The config value enum that we are writing.
 *             new_value - The new value to write.
 */
void set_config_value_int(StrMap *table,
                          CONFIG_VALUE_INT_ENUM cv,
                          int new_value)
{
  /*
   * Local Variables.
   */
  char config_str[MAX_CONFIG_VALUE_LEN];
  CONFIG_VALUE_INT config_value;
  bool valid_key;

  valid_key = create_config_value_int(cv, &config_value);
  if (valid_key)
  {
    /*
     * Clamp the new value to the min/max range of that config value.
     */
    if (new_value < config_value.min_value)
    {
      new_value = config_value.min_value;
    }
    else if (new_value > config_value.max_value)
    {
      new_value = config_value.max_value;
    }

    /*
     * Write the new value back to the StrMap object. Note that strmap_put
     * overwrites any previous value in the table.
     *
     * Fail silently.
     */
    sprintf(config_str, "%i", new_value);
    if (0 == strmap_put(table, config_value.key, config_str))
    {
      DT_DEBUG_LOG("Failed to write to config table: key=%s, value=%s",
                   config_value.key,
                   config_str);
    }
  }
}

/*
 * get_config_value_str
 *
 * Retrieves the value of a config table item. It uses defaults if the value
 * cannot be found and so this should be used as the external entrance point.
 *
 * Parameters: table - The preloaded hash table of config values.
 *             cv - Enum indicating which value we want.
 *             value - Will have the returning string copied into it.
 *
 * Returns: False if the config enum is missing from the code. True otherwise.
 */
bool get_config_value_str(StrMap *table,
                          CONFIG_VALUE_STR_ENUM cv,
                          char *value)
{
  /*
   * Local Variables.
   */
  CONFIG_VALUE_STR config_value;
  char config_value_str[MAX_CONFIG_VALUE_LEN];
  int rc;
  bool value_exists = true;

  /*
   * create_config_value returns a config value with INVALID_VALUE as the key
   * if the value is not a valid constant.
   */
  value_exists = create_config_value_str(cv, &config_value);
  if (value_exists)
  {
    /*
     * If the value doesn't exist in the config file then just use whatever
     * default already exists.
     */
    rc = strmap_get(table,
                    config_value.key,
                    config_value_str,
                    MAX_CONFIG_VALUE_LEN);
    if (0 == rc)
    {
      strncpy(value,
              config_value.default_value,
              MAX_CONFIG_VALUE_LEN);
    }
    else
    {
      strncpy(value,
              config_value_str,
              MAX_CONFIG_VALUE_LEN);
    }

    /*
     * strncpy doesn't copy over a null terminator for the string so we have to
     * do it manually.
     */
    value[strlen(value)] = '\0';
  }

  return value_exists;
}

/*
 * set_config_value_str
 *
 * Sets the value of the config key passed in. This can be used to write new
 * values back out to file.
 *
 * Fails silently when attempting to set a key that doesn't exist.
 *
 * Parameters: table - The config map.
 *             cv - The config value enum that we are writing.
 *             new_value - The new value to write.
 */
void set_config_value_str(StrMap *table,
                          CONFIG_VALUE_STR_ENUM cv,
                          char *new_value)
{
  /*
   * Local Variables.
   */
  CONFIG_VALUE_STR config_value;
  bool valid_key;

  valid_key = create_config_value_str(cv, &config_value);
  if (valid_key)
  {
    /*
     * Verify that the string is within the constant length guidelines. If not
     * then we would fail trying to read it back in.
     */
    if (strlen(new_value) > MAX_CONFIG_VALUE_LEN)
    {
      DT_DEBUG_LOG("Attempted to write new config value that is too long:" \
                   " key=%s, value=%s",
                   config_value.key,
                   new_value);
      return;
    }

    /*
     * Write the new value back to the StrMap object. Note that strmap_put
     * overwrites any previous value in the table.
     *
     * Fail silently.
     */
    if (0 == strmap_put(table, config_value.key, new_value))
    {
      DT_DEBUG_LOG("Failed to write to config table: key=%s, value=%s",
                   config_value.key,
                   new_value);
      return;
    }
  }
}
