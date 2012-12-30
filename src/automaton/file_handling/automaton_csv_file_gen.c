/*
 * automaton_csv_file_gen.c
 *
 * Function for generating blank csv files for the automaton data structure.
 *
 *  Created on: 2 Jan 2010
 *      Author: David
 */
#include "../../dt_logger.h"

#include <stdio.h>
#include "automaton_csv_file_gen.h"
#include "../data_structures/automaton.h"
#include "../data_structures/automaton_state.h"
#include "../data_structures/automaton_event.h"

/*
 * generate_automaton_csv_file
 *
 * Generates a blank csv file from an automaton.
 *
 * Parameters: filename - The full path + filename of the file to create. Will
 *                        not be affected if it already exists.
 *             automaton - The automaton for which we want a blank csv file.
 *
 * Returns: A return code from CSV_FILE_GEN_RET_CODES.
 */
int generate_automaton_csv_file(char *filename, AUTOMATON *automaton)
{
  /*
   * Local Variables.
   */
  FILE *csv_file;
  FILE *test_file;
  int ret_code = CSV_FILE_GEN_OK;
  int num_columns = automaton->num_events;
  int num_rows = automaton->num_states;
  char *csv_line;
  int ii;

  /*
   * Allocate memory for the csv_file. It is a dynamic sized array based on 
   * the number of columns and we cannot guarantee c99 extensions so must
   * allocate on the heap.
   */
  csv_line = (char *) DT_MALLOC((num_columns + 1) * sizeof(char));

  /*
   * If the file already exists then do not overwrite it, let the caller know
   * so that they can handle it.
   */
  test_file = fopen(filename, "r");
  if (test_file != NULL)
  {
    fclose(test_file);
    ret_code = CSV_FILE_GEN_ALREADY_EXISTS;
    goto EXIT_LABEL;
  }

  /*
   * Attempt to open the output file for writing. This may still fail even if
   * the file did not exist (e.g. no access rights).
   */
  csv_file = fopen(filename, "w");
  if (csv_file == NULL)
  {
    ret_code = CSV_FILE_GEN_NOT_CREATED;
    goto EXIT_LABEL;
  }

  /*
   * Create the comma seperated line to be written out on all rows. This will
   * be prefixed by the player state that corresponds to that row.
   */
  for (ii = 0; ii < num_columns - 1; ii++)
  {
    csv_line[ii] = ',';
  }
  csv_line[num_columns] = '\0';

  /*
   * Write the header line out. We could do this in the below code but have
   * taken it out to make for fewer special cases.
   */
  for (ii = 0; ii < num_columns; ii++)
  {
    fprintf(csv_file, ",%s", automaton->events[ii]->name);
  }
  fprintf(csv_file, "\n");

  /*
   * Write the whole file out line by line. The first row is the name of the
   * possible events (comma seperated) and the first element of each line
   * thereafter is the state that the row corresponds to.
   */
  for (ii = 0; ii < num_rows; ii++)
  {
    fprintf(csv_file, "%s,%s\n", automaton->states[ii]->name, csv_line);
  }

  /*
   * Close the file.
   */
  fclose(csv_file);

EXIT_LABEL:

  /*
   * csv_line was allocated on the heap so we need to free it
   * regardless of what else occurred.
   */
  DT_FREE(csv_line);

  return(ret_code);
}
