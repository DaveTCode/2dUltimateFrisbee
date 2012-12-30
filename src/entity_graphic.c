/*
 * entity_graphic.c
 *
 *  Created on: 26 Oct 2009
 *      Author: David
 */

#include "dt_logger.h"

#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include "entity_graphic.h"
#include "rendering/texture_loader.h"

/*
 * load_sprite_from_file
 *
 * Loads the image located at the filename and makes it an opengl texture
 * returning the result in the texture parameter.
 *
 * Parameters: texture - This will be the loaded texture if succesful.
 *             filename - The filename to be loaded.
 *             isSpriteTiled - Set to true if the sprite will tile and false
 *                             if the sprite will stretch.
 *             width - Will be returned as the width of the texture.
 *             height - Will be returned as the height of the texture.
 *
 * Return: One of LOAD_SPRITE_RET_CODES.
 */
int load_sprite_from_file(GLuint *texture,
                          char *filename,
                          bool isSpriteTiled,
                          int *width,
                          int *height)
{
  /*
   * Local Variables.
   */
  int ret_code = LOAD_SPRITE_OK;
  int rc;
  SDL_Surface *loaded_surface;

  /*
   * Attempt to load the sprite from the filename given.
   */
  loaded_surface = (SDL_Surface *) IMG_Load(filename);
  if (NULL == loaded_surface)
  {
    ret_code = LOAD_SPRITE_LOAD_FAIL;
    goto EXIT_LABEL;
  }

  /*
   * Retrieve the width and the height from the loaded surface.
   */
  (*width) = loaded_surface->w;
  (*height) = loaded_surface->h;

  /*
   * Convert the sdl surface to an opengl texture.
   */
  rc = SDL_GL_LoadTexture(loaded_surface, texture, isSpriteTiled);
  if (LOAD_TEXTURE_OK != rc)
  {
    DT_DEBUG_LOG("Failed to convert sprite: %s\n", filename);
    ret_code = LOAD_SPRITE_CONVERT_FAIL;
    goto EXIT_LABEL;
  }

EXIT_LABEL:

  /*
   * Free the sdl surface now that the object is as an opengl texture.
   */
  if (NULL != loaded_surface)
  {
    SDL_FreeSurface(loaded_surface);
  }

  return(ret_code);
}

/*
 * create_entity_graphic
 *
 * Sets up an entity graphic object that can be used by multiple different
 * entities.
 *
 * Parameters: filename - The filename with the graphic in it.
 *             entity_graphic - This will contain the returned object.
 *             isSpriteTiled - Set to true if the sprite will tile and false
 *                             if the sprite will stretch.
 *
 * Return: One of LOAD_SPRITE_RET_CODES.
 */
int create_entity_graphic(char *filename, 
                          ENTITY_GRAPHIC **entity_graphic,
                          bool isSpriteTiled)
{
  /*
   * Local Variables.
   */
  int ret_code;

  /*
   * Allocate the memory for the graphic object.
   */
  (*entity_graphic) = (ENTITY_GRAPHIC *) DT_MALLOC(sizeof(ENTITY_GRAPHIC));

  /*
   * Load the sprite pointed to by the filename into the entity graphic
   * object.
   */
  ret_code = load_sprite_from_file(&((*entity_graphic)->sprite),
                                   filename,
                                   isSpriteTiled,
                                   &((*entity_graphic)->width),
                                   &((*entity_graphic)->height));

  /*
   * Set the reference count to 0 as no entities yet reference this graphic.
   */
  (*entity_graphic)->ref_count = 0;

  return(ret_code);
}

/*
 * destroy_entity_graphic
 *
 * Destroy the entity graphic object and free the memory associated with it.
 *
 * Parameters: entity_graphic - The graphic to destroy.
 */
void destroy_entity_graphic(ENTITY_GRAPHIC *entity_graphic)
{
  /*
   * If a bitmap was loaded for this sprite then free the surface.
   */
  if (GL_TRUE == glIsTexture(entity_graphic->sprite))
  {
    glDeleteTextures(1, &(entity_graphic->sprite));
  }

  /*
   * Free the object itself.
   */
  DT_FREE(entity_graphic);
}

/*
 * add_reference_to_graphic
 *
 * Increases the reference count on the entity graphic object.
 *
 * Parameters: graphic - The graphic which is going to have its reference count
 * increased.
 *
 * Returns: Nothing.
 */
void add_reference_to_graphic(ENTITY_GRAPHIC *graphic)
{
  graphic->ref_count++;
}

/*
 * remove_reference_to_graphic
 *
 * Reduces the reference count of an entity graphic object and checks whether
 * this was the last object that referred to it. If so then it also frees the
 * memory allocated to the graphic object.
 *
 * Parameters: graphic - The entity graphic.
 *
 * Returns: True if the object was destroyed and false otherwise.
 */
bool remove_reference_to_graphic(ENTITY_GRAPHIC *graphic)
{
  /*
   * Local Variables.
   */
  bool destroyed = false;

  /*
   * Reduce the reference count of the graphic object.
   */
  graphic->ref_count--;

  /*
   * If nothing uses this entity graphic then destroy it and free the memory.
   */
  if (graphic->ref_count == 0)
  {
    destroy_entity_graphic(graphic);
    destroyed = true;
  }

  return(destroyed);
}
