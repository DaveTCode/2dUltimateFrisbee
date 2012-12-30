/*
 * automaton_file_output.h
 *
 *  Created on: 6 Mar 2010
 *      Author: David
 */

#ifndef AUTOMATON_FILE_OUTPUT_H_
#define AUTOMATON_FILE_OUTPUT_H_

/*
 * DUMP_AUTOMATON_TO_CSV_RET_CODES
 *
 * Return codes from dump_automaton_to_csv.
 *
 * DUMP_AUTOMATON_TO_CSV_OK - Function was successful.
 * DUMP_AUTOMATON_TO_CSV_FILE_EXISTS - Function failed as the file exists
 *   and we were not allowed to overwrite it.
 * DUMP_AUTOMATON_TO_CSV_FILE_WRITE_FAIL - Function failed as we were not able
 *   to open the file for writing.
 */
#define DUMP_AUTOMATON_TO_CSV_OK              0
#define DUMP_AUTOMATON_TO_CSV_FILE_EXISTS     1
#define DUMP_AUTOMATON_TO_CSV_FILE_WRITE_FAIL 2

#endif /* AUTOMATON_FILE_OUTPUT_H_ */
