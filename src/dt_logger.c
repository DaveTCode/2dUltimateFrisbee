/*
 * dt_logger.c
 *
 * These functions should only be used through the macros provided in
 * dt_logger.h
 *
 *  Created on: 12 Nov 2009
 *      Author: David
 */
#include "dt_logger.h"

#include <stddef.h>
#include <stdio.h>

/*
 * init_logger
 *
 * Starts the logging by creating a file based on the current time and opening
 * it. Fails and exits the program ungracefully if the file could not be
 * created for whatever reason.
 *
 * Should be closed with kill_logger before exiting.
 *
 * Parameters: filename - The filename to precede the current time with.
 */
FILE *init_logger(char *filename)
{
  FILE *new_file;

  /*
   * Open the log file to make sure that we can.
   */
  new_file = fopen(filename, "w");
  if (new_file == NULL)
  {
    exit(-1);
  }

  return(new_file);
}

/*
 * kill_logger
 *
 * Closes the log file used.
 *
 * Parameters: file - The file will be closed.
 */
void kill_logger(FILE *file)
{
  fclose(file);
}
