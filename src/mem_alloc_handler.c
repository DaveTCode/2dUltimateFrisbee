/*
 * File: mem_alloc_handler.c
 *
 * Purpose: Provides a mechanism to call into malloc and test that the result
 *          is not null without repeating condition code throughout.
 */
#include "dt_logger.h"

#include <stdlib.h>
#include <stddef.h>

/*
 * dt_malloc
 *
 * Allocate memory and exit gracefully if not enough memory found.
 *
 * Returns: A void pointer to the allocated memory.
 *
 * Parameters: size - The number of bytes to be allocated. e.g. sizeof(int)
 */
void *dt_malloc(size_t size)
{
  /*
   * Local Variables.
   */
  void *temp_object;

  /*
   * Allocate the memory required for the object passed in.
   */
  temp_object = malloc(size);
  if (temp_object == NULL)
  {
    /*
     * This just drops out of the game with a message. No allocated memory is
     * freed and no files are closed. This is fine on any modern os but note 
     * that it isn't the ideal solution!
     */
    game_exit("Insufficient memory to allocate object");
  }

  return(temp_object);
}

/*
 * dt_malloc_w_debug
 *
 * Wrapper for the dt_malloc function to be used when in debug mode so that we
 * can print out where each allocation came from.
 *
 * Prints a line to the standard debugger.
 *
 * Parameters: size - The number of bytes to allocate.
 *             file - The value of __FILE__ in whatever file called this func.
 *             line - The line number in that file.
 *
 * Returns: A pointer to the newly created memory.
 */
void *dt_malloc_w_debug(size_t size, char *file, int line)
{
  /*
   * Local Variables.
   */
  void *temp_obj = dt_malloc(size);

  DT_MEM_LOG("Allocated %i bytes: %s(%i)\n", size, file, line);

  return(temp_obj);
}

/*
 * dt_free
 *
 * Free memory gracefully. Placeholder to be used if we want to implement some
 * more careful memory checking.
 *
 * Parameters: object - The object to be freed.
 */
void dt_free(void *object)
{
  /*
   * Free the object passed in.
   */
  free(object);
}


/*
 * dt_free_w_debug
 *
 * Wrapper for the dt_free function used in debug mode to print out info on
 * what memory has been freed and from where.
 *
 * DT_FREE resolves to this when DEBUG is defined.
 *
 * Parameters: object - The object to be freed.
 *             file - The value of __FILE__ in the calling function.
 *             line - The line number of the calling function.
 */
void dt_free_w_debug(void *object, char *file, int line)
{
  dt_free(object);

  DT_MEM_LOG("Freed object: %s(%i)\n", file, line);
}
