/*
 * automaton_transition_file_loader.h
 *
 *  Created on: 5 Mar 2010
 *      Author: David
 */

#ifndef AUTOMATON_TRANSITION_FILE_LOADER_H_
#define AUTOMATON_TRANSITION_FILE_LOADER_H_

#include "ezxml/ezxml.h"

struct automaton;
struct automaton_set;

/*
 * The maximum length of a line in the transition file. If this is exceeded
 * then we abort the file.
 */
#define MAX_TRANSITION_FILE_LINE_LEN 1024

/*
 * The number of different fields described in a transition block. Used to store
 * all the lines for post processing.
 */
#define NUM_TRANSITION_FIELDS 4

/*
 * The maximum length of a filename describing where the lua, csv files are in
 * an automaton.
 */
#define MAX_INT_FILENAME_LEN 1024

/*
 * PARSE_SINGLE_AUTOMATON_RET_CODES
 *
 * PARSE_SINGLE_AUTOMATON_OK - If the function was successful.
 * PARSE_SINGLE_AUTOMATON_PARSE_FAIL - If any part of the file could not be
 *                                     parsed.
 * PARSE_SINGLE_AUTOMATON_LOAD_MISSING_CSV_FILENAME - xml missing csv_filename
 *                                                    tag.
 * PARSE_SINGLE_AUTOMATON_MISSING_LUA_FILENAME - xml missing lua_filename tag.
 * PARSE_SINGLE_AUTOMATON_LOAD_MISSING_START_STATE - xml missing start_state
 *                                                   tag.
 * PARSE_SINGLE_AUTOMATON_LOAD_INVALID_START_STATE - start_state tag doesn't match
 *                                                   match any states in
 *                                                   automaton.
 */
#define PARSE_SINGLE_AUTOMATON_OK                        0
#define PARSE_SINGLE_AUTOMATON_PARSE_FAIL                1
#define PARSE_SINGLE_AUTOMATON_LOAD_MISSING_CSV_FILENAME 2
#define PARSE_SINGLE_AUTOMATON_LOAD_MISSING_LUA_FILENAME 3
#define PARSE_SINGLE_AUTOMATON_LOAD_MISSING_START_STATE  4
#define PARSE_SINGLE_AUTOMATON_LOAD_INVALID_START_STATE  5

/*
 * AUTOMATON_XML_FILE_RET_CODES
 *
 * AUTOMATON_XML_FILE_OK - Function successful.
 * AUTOMATON_XML_FILE_LOAD_FAIL - Failed to load the xml file.
 * AUTOMATON_XML_FILE_PARSE_FAIL - Failed to parse the links for one of the
 *                                 automatons.
 * AUTOMATON_XML_FILE_START_AUTOMATON_BAD - The automaton set does not have a
 *                                          starting automaton.
 */
#define AUTOMATON_XML_FILE_OK                  0
#define AUTOMATON_XML_FILE_LOAD_FAIL           1
#define AUTOMATON_XML_FILE_LINKS_FAIL          2
#define AUTOMATON_XML_FILE_START_AUTOMATON_BAD 3

int parse_single_automaton(ezxml_t,
                           struct automaton *,
                           struct automaton_set *, char *, char *);

#endif /* AUTOMATON_TRANSITION_FILE_LOADER_H_ */
