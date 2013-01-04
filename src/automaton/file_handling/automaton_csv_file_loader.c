/*
 * automaton_csv_file_loader.c
 *
 *  Created on: 6 Mar 2010
 *      Author: David
 */
#include "../../dt_logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "automaton_csv_file_loader.h"
#include "../data_structures/automaton.h"
#include "../data_structures/automaton_state.h"
#include "../data_structures/automaton_event.h"
#include "../data_structures/automaton_transition.h"

/*
 * parse_csv_event_line
 *
 * Takes the first line of the csv file (the one containing the event names)
 * and parses it. This extracts all the event names and converts them into the
 * appropriate enumeration value from AI_EVENTS. With this we can then get an
 * event based on which cell in the csv file we are looking at.
 *
 * If any of the events in the csv file could not be parsed then the function
 * fails with a return code.
 *
 * Parameters: line - The first line in the csv file.
 *             automaton - The automaton. Used to locate events by name.
 *             event_line - Will be returned as an array linking the ids of
 *                          the events to their column in the csv file.
 *
 * Returns: One of the returns codes from PARSE_EVENTS_LINE_RET_CODES.
 */
int parse_csv_event_line(char *line, AUTOMATON *automaton, int *event_line)
{
  /*
   * Local Variables.
   */
  char curr_str[MAX_CSV_FILE_LINE_LEN];
  int ret_code = PARSE_EVENTS_LINE_OK;
  int ii;
  int jj = 0;
  bool is_first_cell = true;
  int event_index = 0;
  AUTOMATON_EVENT *event;

  for (ii = 0; ii < (int) strlen(line); ii++)
  {
    /*
     * It is possible that the file may contain more events than the code. If
     * this is the case the we exit this function.
     */
    if (event_index >= automaton->num_events)
    {
      DT_DEBUG_LOG("Too many events in the event line: %s", line);
      ret_code = PARSE_EVENTS_LINE_TOO_MANY_EVENTS;
      goto EXIT_LABEL;
    }

    /*
     * The first cell will be empty (and even if it isn't we ignore it).
     */
    if (is_first_cell)
    {
      if (line[ii] == ',')
      {
        is_first_cell = false;
        curr_str[0] = '\0';
        jj = 0;
      }
    }
    else
    {
      if (line[ii] == ',' || line[ii] == '\n')
      {
        /*
         * Attempt to retrieve the event based on the string between the commas.
         *
         * If this returns an invalid event then return an error code from the
         * function and exit immediately.
         */
        event = find_automaton_event_by_name(automaton, curr_str);
        if (NULL == event)
        {
          DT_DEBUG_LOG("Found bad event (%s) while parsing the csv file: %s",
                       curr_str, line);
          ret_code = PARSE_EVENTS_LINE_BAD_EVENT;
          goto EXIT_LABEL;
        }

        /*
         * Store that event under the current event index in the events array
         * that we are going to return.
         */
        event_line[event_index] = event->id;
        event_index++;

        /*
         * Reset the string for the next cell.
         */
        curr_str[0] = '\0';
        jj = 0;
      }
      else if (line[ii] == '_' ||
               (line[ii] >= 'a' && line[ii] <= 'z') ||
               (line[ii] >= 'A' && line[ii] <= 'Z'))
      {
        curr_str[jj] = line[ii];
        curr_str[jj + 1] = '\0';
        jj++;
      }
      else if (line[ii] == ' ')
      {
        // Ignore spaces, they make the line more readable.
      }
      else
      {
        DT_DEBUG_LOG("Events line in csv file contained a bad character: %s",
                     line);
        ret_code = PARSE_EVENTS_LINE_BAD_CHAR;
        goto EXIT_LABEL;
      }
    }
  }

EXIT_LABEL:

  return(ret_code);
}

/*
 * parse_csv_file_line
 *
 * Takes a lines from the csv file and calls to parse_csv_file_cell for each
 * cell in the line apart from the first. It does this even for empty cells.
 *
 * Parameters: line - The line from the file including its new line character.
 *             automaton - This function needs access to the automaton as at
 *                         the parse_cell level we give the parser direct access
 *                         to add things to the automaton structure.
 *             event_line - Link between order of events in file and order of
 *                          events in the automaton array.
 *                          The first event in the file corresponds to the first
 *                          index in this array. The value in the array
 *                          corresponds to the index into the event array in
 *                          the automaton.
 *
 * Returns: One of PARSE_CSV_LINE_RET_CODES.
 */
int parse_csv_file_line(char *line,
                        AUTOMATON *automaton,
                        int *event_line)
{
  /*
   * Local Variables.
   */
  int ret_code = PARSE_CSV_LINE_OK;
  char curr_str[MAX_CSV_FILE_LINE_LEN] = "";
  bool is_first_cell = true;
  AUTOMATON_STATE *state = NULL;
  int event_index = 0;
  int jj = 0;
  AUTOMATON_TRANSITION *transition;
  int ii;

  for (ii = 0; ii < (int) strlen(line); ii++)
  {
    if (line[ii] == ',' || line[ii] == '\n')
    {
      /*
       * The first cell on the line is the string name of the state for which
       * that line represents. By retrieving we can associate this line with a
       * single state in the automaton.
       *
       * Every other cell in the line corresponds to one of the events so we
       * parse those separately.
       *
       * If the state name doesn't match any states then we cease processing
       * and return an error code allowing the caller to continue if they wish.
       */
      if (is_first_cell)
      {
        is_first_cell = false;

        state = find_automaton_state_by_name(automaton, curr_str);
        if (NULL == state)
        {
          DT_DEBUG_LOG("The csv file line (%s) had an invalid state: %s\n",
                       line, curr_str);
          ret_code = PARSE_CSV_LINE_BAD_STATE_NAME;
          goto EXIT_LABEL;
        }
      }
      else
      {
        /*
         * Three options:
         *
         * 1) The cell is empty: In this case the event has no effect on the
         *    state.
         * 2) The cell has a string in it that corresponds to a transition in
         *    the automaton.
         * 3) The cell has a string in it that is not valid. In this case we
         *    fail the function and allow the caller to continue if they wish.
         */
        if (0 == strlen(curr_str))
        {
          state->transitions[event_line[event_index]] = NULL;
        }
        else
        {
          /*
           * Attempt to retrieve the transition by name from the automaton. If
           * a transition with that name does not exist then simply skip this
           * cell.
           */
          transition = find_automaton_transition_by_name(automaton, curr_str);
          if (NULL == transition)
          {
            DT_DEBUG_LOG("Invalid transition reading in csv file: %s\n",
                         curr_str);
            state->transitions[event_line[event_index]] = NULL;
          }
          else
          {
            state->transitions[event_line[event_index]] = transition;
          }
        }

        /*
         * If there are more cells on this line than there are events then we
         * skip the rest of the line. This would have the result of only
         * skipping the cells that were after the end of the line. Reporting an
         * error code to the calling function will help with logging purposes
         * as well.
         */
        if (event_index >= automaton->num_events)
        {
          DT_DEBUG_LOG("Too many events in csv file line: %s\n", line);
          ret_code = PARSE_CSV_LINE_TOO_MANY_CELLS;
          goto EXIT_LABEL;
        }

        /*
         * The event index gets incremented even when the cell was not valid.
         */
        event_index++;
      }

      /*
       * Reset the current string.
       */
      curr_str[0] = '\0';
      jj = 0;
    }
    else
    {
      curr_str[jj] = line[ii];
      curr_str[jj + 1] = '\0';
      jj++;
    }
  }

EXIT_LABEL:

  return(ret_code);
}

/*
 * load_automaton_csv_file
 *
 * Loads the data from an automaton csv file into an already constructed
 * automaton object. This just fills in what state/event combinations give
 * which transitions.
 *
 * Parameters: filename - The location of the csv file.
 *             automaton - Must be created and have had the states, events and
 *                         transitions filled in before calling this function.
 *
 * Returns: One of CSV_FILE_LOAD_RET_CODES.
 */
int load_automaton_csv_file(char *filename, AUTOMATON *automaton)
{
  /*
   * Local Variables.
   */
  int ret_code = CSV_FILE_LOAD_OK;
  FILE *csv_file;
  char line[MAX_CSV_FILE_LINE_LEN];
  int discard_char;
  bool is_first_line = true;
  int *event_line;
  int rc;

  /*
   * Allocate the memory for the event line on the heap because we cannot
   * guarantee that c99 variable sized arrays are available to the
   * compiler.
   */
  event_line = (int *) DT_MALLOC(automaton->num_events * sizeof(int));

  /*
   * Attempt to open the file for reading. If it doesn't exist or cannot be
   * opened then there is nothing more we can do.
   */
  csv_file = fopen(filename, "r");
  if (csv_file == NULL)
  {
    ret_code = CSV_FILE_LOAD_FAILED;
    goto EXIT_LABEL;
  }

  /*
   * Read each of the remaining lines in the file and pass them off to be
   * processed.
   */
  while(fgets(line, MAX_CSV_FILE_LINE_LEN, csv_file))
  {
    /*
     * If the line was too long to fit in the buffer then fgets will only read
     * some of the line. In order to skip the line we test for this and if it
     * is the case then we read all remaining characters until we find either
     * an EOF or a newline character.
     */
    if (line[strlen(line) - 1] != '\n' && !feof(csv_file))
    {
      DT_DEBUG_LOG("Long(%i) line found: %s\n", strlen(line), line);
      do
      {
        discard_char = fgetc(csv_file);
      } while (discard_char != '\n' && discard_char != EOF);
    }
    else
    {
      /*
       * The first line in the file has the event names in it. In order that we
       * can match cells to a state and an event we parse these out first and,
       * after working out which events they correspond to, we store them in an
       * array.
       *
       * TODO DAT: If any of the event names are bad then we ignore the whole
       * file. This isn't 100% necessary but it seems pointless to just ignore
       * those columns when that would be much more code. Possibly for the
       * future.
       */
      if (is_first_line)
      {
        is_first_line = false;
        rc = parse_csv_event_line(line, automaton, event_line);
        if (rc != PARSE_EVENTS_LINE_OK)
        {
          DT_DEBUG_LOG("Serious error parsing ai event line. Exiting.\n");
          ret_code = CSV_FILE_LOAD_BAD_EVENT_LINE;
          goto EXIT_LABEL;
        }
      }
      else
      {
        DT_DEBUG_LOG("Good csv file line found: %s", line);
        rc = parse_csv_file_line(line, automaton, event_line);
        if (PARSE_CSV_LINE_OK != rc)
        {
          DT_DEBUG_LOG("Parsing error parsing csv file.\n");
          ret_code = CSV_FILE_PARSE_FAILED;
          goto EXIT_LABEL;
        }
      }
    }
  }

  /*
   * Close the file.
   */
  fclose(csv_file);

EXIT_LABEL:

  /*
   * Used the heap for the event line array so must free it here.
   */
  DT_FREE(event_line);

  return(ret_code);
}
