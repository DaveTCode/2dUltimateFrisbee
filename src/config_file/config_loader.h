/*
 * config_loader.h
 *
 *  Created on: 24 Dec 2009
 *      Author: David
 */

#ifndef CONFIG_LOADER_H_
#define CONFIG_LOADER_H_

/*
 * If a line in the config file runs over this length then it is ignored.
 */
#define MAX_CONFIG_FILE_LINE_LEN 64

/*
 * The starting size for the string map of config options.
 */
#define DEFAULT_CONFIG_TABLE_SIZE 10

/*
 * LOAD_CONFIG_RET_CODES
 *
 * Return codes from load_config_to_str_map.
 *
 * CONFIG_TABLE_ALLOCATION_FAILURE - Returned if there was not enough memory to
 * allocate a string map for the config table.
 * CONFIG_FILE_OPEN_FAILURE - Returned if the filename passed in could not be
 * opened for reading.
 */
#define CONFIG_LOAD_OK                  0
#define CONFIG_TABLE_ALLOCATION_FAILURE 1
#define CONFIG_FILE_OPEN_FAILURE        2

int load_config_to_str_map(char *, StrMap **);


#endif /* CONFIG_LOADER_H_ */
