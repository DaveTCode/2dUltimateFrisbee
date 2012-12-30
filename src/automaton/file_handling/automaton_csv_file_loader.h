/*
 * automaton_csv_file_loader.h
 *
 *  Created on: 6 Mar 2010
 *      Author: David
 */

#ifndef AUTOMATON_CSV_FILE_LOADER_H_
#define AUTOMATON_CSV_FILE_LOADER_H_

struct automaton;

/*
 * CSV_FILE_LOAD_RET_CODES
 *
 * Return codes from load_csv_file_to_automaton.
 *
 * CSV_FILE_LOAD_OK - Function passed.
 * CSV_FILE_LOAD_FAILED - Could not open the file for reading.
 * CSV_FILE_PARSE_FAILED - Failed while parsing the file.
 * CSV_FILE_LOAD_BAD_EVENT_LINE - The first line in the file depicting the
 *                                event names was invalid.
 */
#define CSV_FILE_LOAD_OK             0
#define CSV_FILE_LOAD_FAILED         1
#define CSV_FILE_PARSE_FAILED        2
#define CSV_FILE_LOAD_BAD_EVENT_LINE 3

/*
 * PARSE_CSV_LINE_RET_CODES
 *
 * Return codes from parse_csv_file_line.
 *
 * PARSE_CSV_LINE_OK - Function was successful.
 * PARSE_CSV_LINE_BAD_CELL - There was a bad cell in the line.
 * PARSE_CSV_LINE_BAD_STATE_NAME - The line began with a bad state name.
 * PARSE_CSV_LINE_TOO_MANY_CELLS - There were more cells in the line than events
 *                                 in the game.
 */
#define PARSE_CSV_LINE_OK             0
#define PARSE_CSV_LINE_BAD_CELL       1
#define PARSE_CSV_LINE_BAD_STATE_NAME 2
#define PARSE_CSV_LINE_TOO_MANY_CELLS 3

/*
 * PARSE_EVENTS_LINE_RET_CODES
 *
 * Return codes from parse_csv_event_line.
 *
 * PARSE_EVENTS_LINE_OK - Function was successful.
 * PARSE_EVENTS_LINE_BAD_CHAR - There was a bad character in the event line.
 * PARSE_EVENTS_LINE_BAD_EVENT - One of the events did not correspond with any
 *                               in game events.
 * PARSE_EVENTS_LINE_TOO_MANY_EVENTS - There were more events in the file than
 *                                     in the game.
 */
#define PARSE_EVENTS_LINE_OK              0
#define PARSE_EVENTS_LINE_BAD_CHAR        1
#define PARSE_EVENTS_LINE_BAD_EVENT       2
#define PARSE_EVENTS_LINE_TOO_MANY_EVENTS 3

/*
 * The maximum size of a line in the csv file. This is really related to the
 * maximum length of each the event strings but we can hard code it as a large
 * number.
 */
#define MAX_CSV_FILE_LINE_LEN 4096

int load_automaton_csv_file(char *, struct automaton *);

#endif /* AUTOMATON_CSV_FILE_LOADER_H_ */
