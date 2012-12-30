/*
 * draw_functions.c
 *
 * All of the objects that need to be drawn onto the screen have individual
 * functions that place them on the SDL surface that is behind the screen. This
 * is the file that contains all those functions and any other utility functions
 * for drawing on SDL surfaces.
 *
 *  Created on: 23 Oct 2009
 *      Author: David
 */

#include "../dt_logger.h"


#include <string.h>
#include <SDL/SDL_opengl.h>
#include <stdbool.h>
#include "draw_functions.h"
#include "font_structures.h"
#include "../screen.h"
#include "../animation/animation.h"
#include "../animation/animation_handler.h"
#include "../disc.h"
#include "../entity_graphic.h"
#include "../pitch.h"
#include "../player.h"

/*
 * draw_text
 *
 * Draws text in a given font at a specific location on the screen.
 */
void draw_text(char *text, int x, int y, FONT *font)
{
  /*
   * Local Variables.
   */
  int curr_char;
  int end_x;
  int end_y;
  int ii;

  /*
   * Render each character in turn using triangle strips.
   */
  for (ii = 0; ii < (int)strlen(text); ii++)
  {
    curr_char = (int)text[ii];
    end_x = x + font->glyphs[curr_char].maxx;
    end_y = y + font->glyphs[curr_char].maxy;

    glBindTexture(GL_TEXTURE_2D, font->glyphs[curr_char].texid);
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0.0f, 0.0f); glVertex2i(x, y);
      glTexCoord2f(1.0f, 0.0f); glVertex2i(end_x, y);
      glTexCoord2f(0.0f, 1.0f); glVertex2i(x, end_y);
      glTexCoord2f(1.0f, 1.0f); glVertex2i(end_x, end_y);
    glEnd();

    x += font->glyphs[curr_char].advance;
  }
}

/*
 * draw_vertical_line
 *
 * Draws a vertical line on the screen based on input data. The coordinates
 * passed in are in screen coordinates so it is the job of the calling function
 * to convert from world to screen coordinates.
 *
 * Parameters: screen - Provides us with information on the size of the screen.
 *             y1 - The starting point for the line.
 *             y2 - The ending point for the line.
 *             x - The x coordinate for the line.
 *             line_width - The width of the line.
 *             color - A pointer to an array containing the color values for
 *                     the line.
 */
void draw_vertical_line(SCREEN *screen,
                        int y1,
                        int y2,
                        int x,
                        int line_width,
                        GLfloat *color)
{
  /*
   * Local Variables.
   */
  bool draw_as_line = false;

  /*
   * Draw the line as a GL_LINE if the line width is 1.
   */
  if (line_width == 1)
  {
    draw_as_line = true;
  }

  /*
   * We use a color rather than a texture for the lines at the moment.
   */
  glColor4fv(color);

  /*
   * Only draw the line if it will be in the viewing window.
   */
  if ((x >= 0) && (screen->width >= x))
  {
    if (draw_as_line)
    {
      glBegin(GL_LINES);
        glVertex2i(x, y1);
        glVertex2i(x, y2);
      glEnd();
    }
    else
    {
      glBegin(GL_QUADS);
        glVertex2i(x, y1);
        glVertex2i(x + line_width, y1);
        glVertex2i(x + line_width, y2);
        glVertex2i(x, y2);
      glEnd();
    }
  }
}

/*
 * draw_horizontal_line
 *
 * Draws a horizontal line on the screen in the given color. It uses GL_LINES if
 * the line is to have a width of 1 otherwise it uses a filled in GL_QUAD.
 *
 * Parameters: screen - Provides us with information on the size of the screen.
 *             x1 - The starting point for the line.
 *             x2 - The ending point for the line.
 *             y - The y coordinate for the line.
 *             line_width - The width of the line.
 *             color - A pointer to an array containing the color values for
 *                     the line.
 */
void draw_horizontal_line(SCREEN *screen,
                          int x1,
                          int x2,
                          int y,
                          float line_width,
                          GLfloat *color)
{
  /*
   * Local Variables.
   */
  bool draw_as_line = false;

  /*
   * Draw the line as a GL_LINE if the line width is 1.
   */
  if (line_width == 1)
  {
    draw_as_line = true;
  }

  /*
   * We use a color rather than a texture for the lines at the moment.
   */
  glColor4fv(color);

  /*
   * Only draw the line if it will be in the viewing window.
   */
  if ((y >= 0) && (screen->height >= y))
  {
    if (draw_as_line)
    {
      glBegin(GL_LINES);
        glVertex2i(x1, y);
        glVertex2i(x2, y);
      glEnd();
    }
    else
    {
      glBegin(GL_QUADS);
        glVertex2i(x1, y);
        glVertex2i(x2, y);
        glVertex2i(x2, y + (int) line_width);
        glVertex2i(x1, y + (int) line_width);
      glEnd();
    }
  }
}

/*
 * draw_sprite
 *
 * Performs the equivalent of drawing a sprite on the screen. It does this by
 * creating a quad of the defined size (this need not have anything to do with
 * the size of the sprite) and then texturing it with the given texture.
 *
 * We can emulate other shapes by making parts of the texture transparent using
 * alpha testing. To do this the alpha value for the parts of the image should
 * be set to the minimum.
 *
 * Parameters: x1, y1 - The top left of where we want to draw the sprite.
 *             x2, y2 - The bottom right of where we want to draw the sprite.
 *             sprite - The texture reference for opengl.
 *             tx1, ty1 - The coordinates of the top left point in the texture
 *                        that will map to the top left of the sprite.
 *             tx2, ty2 - The coordinates of the bottom right point in the
 *                        texture that will map to the bottom right of the
 *                        sprite.
 */
void draw_sprite(float x1, float y1,
                 float x2, float y2,
                 GLuint sprite,
                 float tx1, float ty1,
                 float tx2, float ty2)
{
  glBindTexture(GL_TEXTURE_2D, sprite);

  /*
   * Draw the sprite as a quad with the the texture overlayed.
   */
  glBegin(GL_QUADS);
    glTexCoord2f(tx1, ty1); glVertex2f(x1, y1);
    glTexCoord2f(tx2, ty1); glVertex2f(x2, y1);
    glTexCoord2f(tx2, ty2); glVertex2f(x2, y2);
    glTexCoord2f(tx1, ty2); glVertex2f(x1, y2);
  glEnd();
}

/*
 * draw_animation_frame
 *
 * Takes an animation object and draws the current frame in the current
 * direction at the given coordinates.
 *
 * Parameters: x1, y1 - The top left coordinates which indicate where the frame
 *                      is drawn.
 *             x2, y2 - The bottom right coordinates which indicate where the
 *                      frame is drawn.
 *             animation - The animation object to be drawn.
 *             curr_frame - The current frame to be drawn.
 *             curr_direction - The direction of the animation at the moment.
 */
void draw_animation_frame(float x1, float y1,
                          float x2, float y2,
                          ANIMATION *animation,
                          int frame,
                          ANIMATION_DIRECTION direction)
{
  /*
   * Local Variables.
   */
  float tx1,ty1;
  float tx2,ty2;

  /*
   * If the frame number is not for a frame that exists we can recover by just
   * drawing the first frame in the sequence.
   */
  if (frame >= animation->num_frames_per_row)
  {
    DT_DEBUG_LOG("Attempting to draw animation frame that does not exist: \n" \
                 "direction = %i, frame = %i animation = \n",
                 direction, frame);
    frame = 0;
  }

  /*
   * Calculate the relative coordinates in the sprite sheet so that tx1,ty1
   * corresponds to the top left of the current frame and tx2,ty2 corresponds
   * to the bottom right.
   *
   * Note that we scale by the sheet width and height as the texture
   * coordinates are in the range [0.0f, 1.0f].
   */
  tx1 = ((float) (frame * animation->frame_width)) /
        ((float) animation->sprite_sheet->width);
  tx2 = ((float) ((frame + 1) * animation->frame_width)) /
        ((float) animation->sprite_sheet->width);
  ty1 = ((float) (direction * animation->frame_height)) /
        ((float) animation->sprite_sheet->height);
  ty2 = ((float) ((direction + 1) * animation->frame_height)) /
        ((float) animation->sprite_sheet->height);
  DT_ASSERT(tx1 <= 1.0f && tx2 <= 1.0f && ty1 <= 1.0f && ty2 <= 1.0f);

  /*
   * Draw the part of the sprite sheet calculated in the desired position.
   */
  draw_sprite(x1, y1, x2, y2,
              animation->sprite_sheet->sprite,
              tx1, ty1, tx2, ty2);
}

/*
 * adjust_diameter_for_height
 *
 * The aim of this function is to take a diameter of an object at a given height
 * above the floor and to calculate the diameter of its shadow on the floor.
 *
 * We use it to display the height of an object using its size.
 *
 * It is not possible to use a perspective projection instead as we want the
 * rest of the pitch to have an orthographic projection.
 *
 * Method: To achieve this we draw two right angle triangles. Both have a common
 * point at the eye position. Then the first has its points as the edges of the
 * object and the second has its points as the edges of the objects shadow.
 * By placing the eye directly above the edge of the disc one of these triangles
 * is contained in the other. Thus they have the same angle at the shared point.
 *
 * This angle is known as arctan(opposite/adjacent) =
 * arctan(disc edge to edge / object edge to camera)
 *
 * So tan (angle) = object edge to edge / object edge to camera
 *
 * But also tan (angle) = object edge to object shadow edge / floor to camera.
 *
 * So disc edge to object shadow edge = tan (angle) * floor to camera.
 *
 * Parameters: diameter - The diameter of the object.
 *             height - The height of the object.
 *             eye_height - The height above the floor that the camera is.
 *
 * Cost: One subtraction, one division, one multiplication.
 *
 * Returns : The diameter of the objects shadow.
 */
float adjust_diameter_for_height(float diameter, float height, float eye_height)
{
  /*
   * Local Variables.
   */
  float adj_diameter;
  float dist_eye_to_disc = eye_height - height;
  float tan_angle;

  /*
   * Calculate the tangent of the angle (see function header comment).
   */
  tan_angle = diameter / dist_eye_to_disc;

  /*
   * Use that to find the diameter of the shadow on the floor.
   */
  adj_diameter = tan_angle * eye_height;

  return(adj_diameter);
}

/*
 * draw_disc
 */
void draw_disc(DISC *disc, float camera_height)
{
  /*
   * Local Variables.
   */
  float adj_diameter;

  /*
   * To draw the disc on the screen we need to use a slightly ugly hack. This
   * function returns a new diameter for the disc based on how high it is. The
   * idea is that although the projection matrix is orthogonal the disc should
   * appear larger the higher it is. See the function comments for details.
   */
  adj_diameter = adjust_diameter_for_height(disc->diameter_m,
                                            disc->position.z,
                                            camera_height);

  draw_sprite(disc->position.x,
              disc->position.y,
              disc->position.x + adj_diameter,
              disc->position.y + adj_diameter,
              disc->graphic->sprite,
              0.0f, 0.0f, 1.0f, 1.0f);
}

/*
 * draw_player
 *
 * Draws a frame of whatever animation is currently running onto the screen
 * where the player is.
 *
 * Uses the animation handler to get a reference back to the animation object
 * that needs to be drawn.
 *
 * Parameters: player - Has the coordinates and the current frame/direction.
 *             animation_handler - Has links to all the animations that can be
 *                                 drawn.
 */
void draw_player(PLAYER *player, ANIMATION_HANDLER *animation_handler)
{
  /*
   * Local Variables.
   */
  ANIMATION *animation;

  /*
   * Use the animation handler to retrieve the animation that is to be drawn.
   */
  animation = handler_get_animation(animation_handler,
                                    player->animation_choice);

  /*
   * Draw the current frame onto the screen at the players location. Use the
   * direction of the player to determine which direction sprite to use.
   */
  draw_animation_frame(player->position.x, player->position.y,
                       player->position.x + player->width_x,
                       player->position.y + player->width_y,
                       animation,
                       player->curr_frame,
                       player->direction);
}

/*
 * draw_pitch_background
 *
 * Draws a tiled sprite onto the screen to simulate a grass effect on
 * the pitch.
 *
 * Parameters: screen - Contains information on what to draw into.
 *             pitch - The pitch dimensions.
 */
void draw_pitch_background(SCREEN *screen, PITCH *pitch)
{
  draw_sprite((float) (pitch->back_endzone_1),
              (float) (pitch->side_1),
              (float) (pitch->back_endzone_2),
              (float) (pitch->side_2),
              pitch->grass_tile->sprite,
              0.0f, 0.0f, 1.0f, 1.0f);
}

/*
 * draw_pitch_lines
 *
 * Draws the pitch onto a screen object. This is called before drawing any
 * players or the disc as otherwise they may be clipped on the lines.
 *
 * Parameters: screen - This gives us the height and width of the screen. Also
 *                      contains the point on the screen at which to start
 *                      drawing.
 *             pitch - Gives the pitch dimensions and the color of the line.
 */
void draw_pitch_lines(SCREEN *screen, PITCH *pitch)
{
  /*
   * Local Variables.
   */
  GLfloat *line_color = (GLfloat *) pitch->line_color;

  /*
   * The pitch is drawn using colors rather than textures so disable textures
   * for this function. Any function that draws just in colors must be wrapped
   * with a glDisable(GL_TEXTURE_2D) glEnable(GL_TEXTURE_2D) pair.
   */
  glDisable(GL_TEXTURE_2D);

  /*
   * Draw the top of the pitch.
   */
  draw_horizontal_line(screen,
                       pitch->back_endzone_1,
                       pitch->back_endzone_2,
                       pitch->side_1,
                       pitch->line_width_m,
                       line_color);
  /*
   * Draw the bottom of the pitch.
   */
  draw_horizontal_line(screen,
                       pitch->back_endzone_1,
                       pitch->back_endzone_2,
                       pitch->side_2,
                       pitch->line_width_m,
                       line_color);
  /*
   * Draw the back of the left hand side endzone.
   */
  draw_vertical_line(screen,
                     pitch->side_1,
                     pitch->side_2,
                     pitch->back_endzone_1,
                     (int) pitch->line_width_m,
                     line_color);
  /*
   * Draw the back of the right hand side endzone.
   */
  draw_vertical_line(screen,
                     pitch->side_1,
                     pitch->side_2,
                     pitch->back_endzone_2,
                     (int) pitch->line_width_m,
                     line_color);
  /*
   * Draw the front of the left hand side endzone.
   */
  draw_vertical_line(screen,
                     pitch->side_1,
                     pitch->side_2,
                     pitch->front_endzone_1,
                     (int) pitch->line_width_m,
                     line_color);
  /*
   * Draw the front of the right hand side endzone.
   */
  draw_vertical_line(screen,
                     pitch->side_1,
                     pitch->side_2,
                     pitch->front_endzone_2,
                     (int) pitch->line_width_m,
                     line_color);

  /*
   * Reneable texturing as we are done drawing in color.
   */
  glEnable(GL_TEXTURE_2D);
}
