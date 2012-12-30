/*
 * ai_csv_file_gen.h
 *
 *  Created on: 3 Jan 2010
 *      Author: David
 */

#ifndef AI_CSV_FILE_GEN_H_
#define AI_CSV_FILE_GEN_H_

/*
 * This corresponds to the maximum length of the first row in the csv file
 * containing the ai automaton.
 *
 * It MUST be greater than the length of all the string names plus the number
 * of names + 1. this is because it will contain a string of the form:
 * name_1,name_2,name_3\n
 */
#define CSV_FILE_HEADER_ROW_LEN 1024

/*
 * CSV_FILE_GEN_RET_CODES
 *
 * Return codes from the function generate_ai_csv_file.
 *
 * CSV_FILE_GEN_OK
 * CSV_FILE_GEN_ALREADY_EXISTS - The file already exists so was not overwritten.
 * CSV_FILE_GEN_NOT_CREATED - Despite the file not existing we could not open it
 *                            for writing.
 */
#define CSV_FILE_GEN_OK             0
#define CSV_FILE_GEN_ALREADY_EXISTS 1
#define CSV_FILE_GEN_NOT_CREATED    2

#endif /* AI_CSV_FILE_GEN_H_ */
