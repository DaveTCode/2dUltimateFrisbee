/*
 * dt_logger.h
 *
 *  Created on: 24 Oct 2009
 *      Author: David
 */
#ifndef DT_LOGGER_H_
#define DT_LOGGER_H_

#include <assert.h>
#include <stdio.h>

/*
 * Common prototypes that need to be accessible in all files.
 */
void game_exit(char *);
void *dt_malloc_w_debug(size_t , char *, int);
void dt_free_w_debug(void *, char *, int);
FILE *init_logger(char *);
void kill_logger(FILE *);

#define bool  _Bool
#define true  1
#define false 0

#define LOG_FILENAME "full_log.txt"
#define MEM_LOG_FILENAME "mem_log.txt"
#define AI_LOG_FILENAME "ai_log.txt"

FILE *g_log_file;
FILE *g_mem_log_file;
FILE *g_ai_log_file;
#define DT_INIT_LOG g_log_file = init_logger(LOG_FILENAME); \
                    g_mem_log_file = init_logger(MEM_LOG_FILENAME); \
                    g_ai_log_file = init_logger(AI_LOG_FILENAME);

/*
 * TODO: DAT - There doesn't appear to be a consistent way across all
 * compilers to do variadic macros. Since I want these for logging and in all
 * conceivable cases this code will be compiled in either MSVC or GCC the
 * following is fine.
 *
 * In particular, the ## modifier is required for GCC but produces warnings
 * with -pedantic set. It would be good if we could find a way around this.
 *
 * Long term perhaps replacing all of this with a proper logging library that
 * I didn't write is the solution to aim for.
 */
#define DT_DEBUG_LOG(fmt, ...) fprintf(g_log_file, fmt, ## __VA_ARGS__); \
                           fflush(g_log_file);
#define DT_MEM_LOG(fmt, ...) fprintf(g_mem_log_file, fmt, ## __VA_ARGS__); \
                         fflush(g_mem_log_file);
#define DT_AI_LOG(fmt, ...) fprintf(g_ai_log_file, fmt, ## __VA_ARGS__); \
                        fflush(g_ai_log_file);
#define DT_KILL_LOG kill_logger(g_log_file); \
                    kill_logger(g_mem_log_file); \
                    kill_logger(g_ai_log_file);
#define DT_ASSERT(x) assert(x)
#define DT_MALLOC(x) dt_malloc_w_debug(x, __FILE__, __LINE__)
#define DT_FREE(x) dt_free_w_debug(x, __FILE__, __LINE__)

#endif /* DT_LOGGER_H_ */
