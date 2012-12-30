/*
 * config_loader.c
 *
 *  Created on: 23 Dec 2009
 *      Author: David
 */

#include "../dt_logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strmap/StrMap.h>
#include <strmap/StrMap.c>
#include "config_loader.h"

/*
 * parse_config_file_line
 *
 * This function is responsible for taking a line from a config file and
 * turning it into a key value pair if possible. If the line is badly formed it
 * just silently drops it.
 *
 * Parameters: config_table - The string map that contains the config key value
 *                            pairs.
 *             line - The line to be parsed. Must be null terminated and end
 *                    with a newline character.
 *
 * Returns: Nothing.
 */
void parse_config_file_line(StrMap *config_table, char *line)
{
  /*
   * Local Variables.
   */
  char key[MAX_CONFIG_FILE_LINE_LEN];
  char value[MAX_CONFIG_FILE_LINE_LEN];
  bool parsing_key = true;
  bool parsing_value = false;
  int current_key_len = 0;
  int current_value_len = 0;
  int ii;

  /*
   * We have a valid line so parse out the key and value pair. If there are
   * any non alphanumeric characters then discard them but continue
   * processing.
   */
  for (ii = 0; ii < (int) strlen(line); ii++)
  {
    if (((line[ii] >= 'A' && line[ii] <= 'Z') || line[ii] == '_')
        && parsing_key)
    {
      key[current_key_len] = line[ii];
      current_key_len++;
    }
    else if ((line[ii] >= '0' && line[ii] <= '9') && parsing_value)
    {
      value[current_value_len] = line[ii];
      current_value_len++;
    }
    else if (line[ii] == '=')
    {
      parsing_value = true;
      parsing_key = false;
    }
    else
    {
      /*
       * Ignore bad characters.
       */
    }
  }

  /*
   * When building strings manually, we need to null terminate them in order to
   * be able to use string manipulation functions (such as strlen) on them.
   */
  key[current_key_len] = '\0';
  value[current_value_len] = '\0';

  /*
   * Add the key value pair to the map. Do not replace old values. Before doing
   * so check to make sure that the key and value are not empty strings.
   */
  if (strlen(key) > 0 && strlen(value) > 0)
  {
    if (!strmap_exists(config_table, key))
    {
      DT_DEBUG_LOG("key:%s value:%s added\n", key, value);
      strmap_put(config_table, key, value);
    }
  }
}

/*
 * load_config_to_str_map
 *
 * Takes a config file on disk and parses it into a string map. Can fail if the
 * file doesn't exist or doesn't have the correct permissions.
 *
 * Parameters: filename - Filename and path of the config file.
 *             new_config_table - Will contain the new string map.
 *
 * Returns: One of LOAD_CONFIG_RET_CODES.
 */
int load_config_to_str_map(char *filename, StrMap **new_config_table)
{
  /*
   * Local Variables
   */
  int rc = CONFIG_LOAD_OK;
  FILE *config_file = NULL;
  char line[MAX_CONFIG_FILE_LINE_LEN];
  int discard_char;
  StrMap *config_table;

  /*
   * Create a new string map. This returns null on allocation failure.
   */
  config_table = strmap_new(DEFAULT_CONFIG_TABLE_SIZE);
  if (config_table == NULL)
  {
    rc = CONFIG_TABLE_ALLOCATION_FAILURE;
    goto EXIT_LABEL;
  }

  /*
   * Attempt to open the file. If this fails then return an error code.
   */
  config_file = fopen(filename, "r");
  if (config_file == NULL)
  {
    rc = CONFIG_FILE_OPEN_FAILURE;
    goto EXIT_LABEL;
  }

  while(fgets(line, MAX_CONFIG_FILE_LINE_LEN, config_file))
  {
    /*
     * If the line was too long to fit in the buffer then fgets will only read
     * some of the line. In order to skip the line we test for this and if it
     * is the case then we read all remaining characters until we find either
     * an EOF or a newline character.
     */
    if (line[strlen(line) - 1] != '\n' && !feof(config_file))
    {
      DT_DEBUG_LOG("Long(%i) line found: %s\n", strlen(line), line);
      do
      {
        discard_char = fgetc(config_file);
      } while (discard_char != '\n' && discard_char != EOF);
    }
    else
    {
      DT_DEBUG_LOG("Good config file line found: %s\n", line);
      parse_config_file_line(config_table, line);
    }
  }

EXIT_LABEL:

  /*
   * It is feasible that this function failed without opening the config file.
   * In that case we don't want to make a call to close it.
   */
  if (config_file != NULL)
  {
    fclose(config_file);
  }

  *new_config_table = config_table;

  return(rc);
}
