/*
 * automaton_file_output.c
 *
 *  Created on: 6 Mar 2010
 *      Author: David
 */

#include "../../dt_logger.h"

#include <stdbool.h>
#include <stdio.h>
#include "automaton_file_output.h"
#include "../data_structures/automaton.h"
#include "../data_structures/automaton_event.h"
#include "../data_structures/automaton_state.h"
#include "../data_structures/automaton_transition.h"

/*
 * write_event_line
 *
 * Internal function to this file. Write the event line to a csv file. This is
 * a comma seperated list of events. It has a starting comma so that the events
 * begin on column 1 NOT 0.
 *
 * Parameters: csv_file - The file to write to. Must be opened already.
 *             automaton - The automaton from which events are taken.
 */
void write_event_line(FILE *csv_file, AUTOMATON *automaton)
{
  /*
   * Local Variables.
   */
  int ii;

  for (ii = 0; ii < automaton->num_events; ii++)
  {
    fprintf(csv_file, ", %s", automaton->events[ii]->name);
  }
  fprintf(csv_file, "\n");
}

/*
 * write_state_line
 *
 * Write a single line out to the csv file containing the state transitions
 * for a single state. The first column is the state name and each following
 * are either empty (if there is no transition) or has the transition name if
 * there is a transition.
 *
 * Parameters: csv_file - Must already be open.
 *             state - The automaton state that we are using.
 *             automaton - Required for knowing the number of events to print.
 */
void write_state_line(FILE *csv_file,
                      AUTOMATON_STATE *state,
                      AUTOMATON *automaton)
{
  /*
   * Local Variables.
   */
  int ii;

  fprintf(csv_file, "%s", state->name);
  for (ii = 0; ii < automaton->num_events; ii++)
  {
    if (NULL == state->transitions[ii])
    {
      fprintf(csv_file, ",");
    }
    else
    {
      fprintf(csv_file, ",%s", state->transitions[ii]->name);
    }
  }
  fprintf(csv_file, "\n");
}

/*
 * dump_automaton_to_csv
 *
 * Debug function that writes out a fully formed automaton to a csv file. It
 * will only overwrite the file given if requested.
 *
 * Parameters: filename - The name of the file to write.
 *             automaton - The automaton to dump to file.
 *             overwrite - Set to true if any existing file is to be
 *                         overwritten.
 */
int dump_automaton_to_csv(char *filename, AUTOMATON *automaton, bool overwrite)
{
  /*
   * Local Variables.
   */
  int ret_code = DUMP_AUTOMATON_TO_CSV_OK;
  FILE *csv_file;
  int ii;

  /*
   * If we can open the file for reading then it already exists. So if overwrite
   * is set to false then do not attempt to write the csv file out.
   */
  csv_file = fopen(filename, "r");
  if ((NULL != csv_file) && !overwrite)
  {
    DT_DEBUG_LOG("File %s already exists. Automaton dump failed.\n", filename);
    ret_code = DUMP_AUTOMATON_TO_CSV_FILE_EXISTS;
    goto EXIT_LABEL;
  }
  else if (NULL != csv_file)
  {
    fclose(csv_file);
  }  

  /*
   * Even if the file does not exist there is no guarantee that we have
   * sufficient permission to write a file there.
   */
  csv_file = fopen(filename, "w");
  if (NULL == csv_file)
  {
    DT_DEBUG_LOG("File %s could not be opened for writing.\n", filename);
    ret_code = DUMP_AUTOMATON_TO_CSV_FILE_WRITE_FAIL;
    goto EXIT_LABEL;
  }

  /*
   * Write out the event line first.
   */
  write_event_line(csv_file, automaton);

  /*
   * For each state write a new line out to the file.
   */
  for (ii = 0; ii < automaton->num_states; ii++)
  {
    write_state_line(csv_file, automaton->states[ii], automaton);
  }

EXIT_LABEL:

  return(ret_code);
}
