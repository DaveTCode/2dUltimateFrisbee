/*
 * main.c
 *
 *  Created on: 17 Oct 2009
 *      Author: David
 */

#include "dt_logger.h"


#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_opengl.h"
#include "strmap/strmap.h"

#include "dt_logger.h"

#include "ai_general/ai_event_handler.h"
#include "ai_general/player_ai.h"
#include "animation/animation_handler.h"
#include "audio/audio_general.h"
#include "automaton/data_structures/automaton_timed_event_queue.h"
#include "camera_handler.h"
#include "collision_handler.h"
#include "config_file/config_map.h"
#include "config_file/config_loader.h"
#include "conversion_constants.h"
#include "gl_window_handler.h"
#include "impl_automatons/generic_o_d_files/event_names.h"
#include "impl_automatons/generic_o_d_files/init_automaton_events.h"
#include "input_handler.h"
#include "match_state.h"
#include "match_creation.h"
#include "pitch.h"
#include "physics.h"
#include "rendering/font_loader.h"
#include "rendering/font_structures.h"
#include "screen.h"
#include "window_handler.h"

/*
 * Should be set in the config file once it exists. Forces the FPS to be this
 * or lower.
 */
#define DEFAULT_MAX_FPS 30

/**
 * main
 *
 * TODO: Write function block once this is cleaned up.
 */
int main(int argc, char** argv)
{
  /*
   * Local Variables.
   */
  SDL_Event transient_event;
  SCREEN *screen;
  MATCH_STATE *match_state;
  Uint32 frame_start_time;
  Uint32 frame_time_taken_ms;
  Uint32 physics_time_delta;
  Uint32 last_physics_update;
  Uint32 ai_time_delta;
  Uint32 last_ai_update;
  Uint32 ms_per_frame;
  int rc;
  StrMap *config_table;
  Uint32 last_animation_update = 0;
  Uint32 animation_ms_per_frame;
  int max_fps;
  FONT *font;
  char disc_graphic_file[MAX_CONFIG_VALUE_LEN + 1];
  char grass_tile_file[MAX_CONFIG_VALUE_LEN + 1];
  char o_xml_file[MAX_CONFIG_VALUE_LEN + 1];
  char d_xml_file[MAX_CONFIG_VALUE_LEN + 1];
  int ii;
  SDL_Color white = {0xFF, 0xFF, 0xFF, 0x00};

  /*
   * Begin logging.
   */
  DT_INIT_LOG;

  /*
   * TODO: Move loading and retrieving constants using defaults from the config
   * file to a different folder, code file. Simple api.
   */
  load_config_to_str_map("config.txt", &config_table);
  if (!get_config_value_int(config_table,
                            cv_animation_ms_per_frame,
                            (int *)&animation_ms_per_frame))
  {
    game_exit("Programmer error: animation ms per frame not handled in cfg.");
  }
  if (!get_config_value_int(config_table, cv_max_fps, &max_fps))
  {
    game_exit("Programmer error: max fps not handled in cfg.");
  }
  ms_per_frame = (int) MILLISECONDS_PER_SECOND / max_fps;
  if (!get_config_value_str(config_table, cv_disc_graphic, (char *)disc_graphic_file))
  {
    game_exit("Programmer error: disc graphic not handled in cfg.");
  }
  if (!get_config_value_str(config_table, cv_grass_tile_filename, (char *)grass_tile_file))
  {
    game_exit("Programmer error: grass tile graphic not handled in cfg.");
  }
  if (!get_config_value_str(config_table, cv_o_xml_file, (char *)o_xml_file))
  {
    game_exit("Programmer error: O XML file not handled in cfg.");
  }
  if (!get_config_value_str(config_table, cv_d_xml_file, (char *)d_xml_file))
  {
    game_exit("Programmer error: D XML file not handled in cfg.");
  }

  /*
   * Create the screen object and use it to initialise the window system.
   */
  screen = create_screen();
  rc = init_window_system(screen, SCREEN_WIDTH, SCREEN_HEIGHT, BPP);
  if (INIT_OK != rc)
  {
    game_exit("Window system could not be initialised.");
  }

  /*
   * Initialise Audio subsystem
   */
  rc = init_audio_subsystem(config_table);
  if (INIT_AUDIO_SYSTEM_OK != rc)
  {
    game_exit("Audio subsystem could not be initialised.");
  }

  /*
   * Initialise the opengl components of the screen.
   */
  rc = init_gl(screen);
  if (GL_INIT_OK != rc)
  {
    game_exit("Could not do opengl initialisation.");
  }

  /*
   * TODO: Proper font management system where more than one font can be loaded
   *
   * Create the font object that is going to be used to write text throughout
   * the game.
   */
  rc = create_font("resources\\Fonts\\LucidaSansRegular.ttf",
                   &font,
                   30,
                   TTF_STYLE_NORMAL,
                   white);
  if (BUILD_FONT_OK != rc)
  {
    game_exit("Could not load up LucidaSansRegular font.");
  }

  /*
   * TODO: Constants to move from here.
   *
   * Create a new match state object before starting the game loop.
   */
  match_state = create_match_state(15,
                                   60 * 60,
                                   disc_graphic_file,
                                   grass_tile_file,
                                   o_xml_file,
                                   d_xml_file,
                                   create_o_automaton_states,
                                   create_automaton_events);
  if (NULL == match_state)
  {
    game_exit("Failed to create match state.");
  }
  init_pitch(match_state->pitch);

  /*
   * Scale the pitch by an arbitrary amount to account for the otherwise small
   * scaling.
   */
  scale_camera(match_state->camera_handler, 10.0f);

  /*
   * Load the animation data.
   */
  rc = load_animation_data(match_state->animation_handler);
  if (LOAD_ANIMATION_DATA_OK != rc)
  {
    game_exit("Failed to load animation data.");
  }

  /*
   * Initialise the timings
   */
  last_physics_update = SDL_GetTicks();
  last_ai_update = SDL_GetTicks();

  /*
   * Initialise and start a new match.
   */
  start_match(match_state);

  // @@@DAT testing
  throw_multi_player_ai_event_by_name(match_state->teams,
                                      match_state->players_per_team,
                                      match_state->teams[0]->players[0]->automaton,
                                      AUTOMATON_EVENT_PULL_THROWN);

  /*
   * Main game loop: Listen for events from the user and act on them.
   */
  while(true)
  {
    /*
     * Reset the timer.
     */
    frame_start_time = SDL_GetTicks();

    /*
     * Look at disc
     */
    look_at_location(match_state->camera_handler,
                     match_state->disc->position.x,
                     match_state->disc->position.y,
                     screen);

    /*
     * While there are events to process, do so.
     */
    while(SDL_PollEvent(&transient_event))
    {
      /*
       * First check whether it is an SDL_QUIT event. If so then exit with
       * message.
       */
      if (transient_event.type == SDL_QUIT)
      {
        destroy_match_state(match_state);
        game_exit("User requested exit via SDL_QUIT event.");
      }
      else if ((transient_event.type == SDL_KEYDOWN) ||
               (transient_event.type == SDL_KEYUP))
      {
        /*
         * The event was the user either depressing or releasing a key so call
         * to the keyboard event handler to deal with the event.
         */
        handle_keyboard_event(&(transient_event.key),
                              transient_event.type,
                              match_state,
                              screen);
      }
      else if ((transient_event.type == SDL_MOUSEBUTTONDOWN) ||
               (transient_event.type == SDL_MOUSEBUTTONUP))
      {
        /*
         * The event was the user either depressing or releasing a mouse button
         * so pass to the mouse button event handler to deal with it. This
         * includes the mouse wheel moving.
         */
        handle_mousebutton_event(&(transient_event.button), match_state);
      }
    }

    /*
     * Perform an update on all the ai objects.
     */
    ai_time_delta = SDL_GetTicks() - last_ai_update;
    process_all_player_ai(match_state->teams,
                          match_state->players_per_team,
                          ai_time_delta);
    last_ai_update = SDL_GetTicks();

    /*
     * Process the automaton timed event queue to see if any events need to be
     * popped.
     */
    pop_all_timed_events(match_state->automaton_handler->timed_event_queue,
                         SDL_GetTicks(),
                         match_state);

    /*
     * Do physics processing. This updates the positions of all moving entities
     * in the game.
     *
     * WARNING - If this takes longer than the amount of time allocated per
     * frame then there might be interesting problems in the frame refresh.
     */
    physics_time_delta = SDL_GetTicks() - last_physics_update;
    calculate_positions(match_state,
                        physics_time_delta);
    last_physics_update = SDL_GetTicks();

    /*
     * Having moved all of the objects to their new positions we need to
     * detect collisions and verify the new locations.
     *
     * At the end of this function the positions of all objects will have been
     * updated.
     */
    detect_and_handle_collisions(match_state->teams,
                                 match_state->players_per_team,
                                 match_state->disc);

    /*
     * Update the camera object.
     */
    update_camera_position(match_state);

    /*
     * TODO: Is this management of animations sufficient?
     *
     * Update the animation frame counters.
     */
    if (SDL_GetTicks() - last_animation_update >= animation_ms_per_frame)
    {
      last_animation_update = SDL_GetTicks();
      for (ii = 0; ii < match_state->players_per_team; ii++)
      {
        increment_animation_frame_counter(match_state->teams[0]->players[ii],
                                          match_state->animation_handler);
        increment_animation_frame_counter(match_state->teams[1]->players[ii],
                                          match_state->animation_handler);
      }
    }

    /*
     * If the frame has taken less than the maximum allowed amount of time to
     * render then delay the screen update.
     */
    frame_time_taken_ms = SDL_GetTicks() - frame_start_time;
    if (frame_time_taken_ms < ms_per_frame)
    {
      SDL_Delay(ms_per_frame - frame_time_taken_ms);
    }

    /*
     * Redraw the screen.
     */
    redraw_screen(screen, match_state, font);
  }

  return(0);
}

/*
 * redraw_screen
 *
 * This function is called once per time around the main loop. It redraws the
 * screen with all players in the correct positions.
 *
 * Parameters: screen - The screen object on which we are drawing.
 *             match_state - The unique objects in a match.
 */
void redraw_screen(SCREEN *screen,
                   MATCH_STATE *match_state,
                   FONT *font)
{
  /*
   * Local Variables.
   */
  char score_string[10];
  char game_time_string[10];

  /*
   * Clear the screen before doing any drawing.
   */
  glClear(GL_COLOR_BUFFER_BIT);

  draw_pitch_background(screen, match_state->pitch);

  /*
   * Draw the pitch outline. This is the lines and the 'permanent' markings.
   */
  draw_pitch_lines(screen,
                   match_state->pitch);

  /*
   * After drawing the screen iterate over the list of players in both teams
   * and if they have new positions then update them.
   */
  draw_players(match_state->teams,
               match_state->players_per_team,
               match_state->animation_handler);

  /*
   * Draw the disc on top of the players.
   */
  draw_disc(match_state->disc, 50.0f);

  /*
   * Put the score on the screen as text. We make this independent of scaling
   * and screen movement so that the text is on a fixed position on the screen
   * regardless of the orientation etc.
   */
  glPushMatrix();
  glLoadIdentity();
  convert_score_to_text(match_state->match_stats->team_a_score,
                        match_state->match_stats->team_b_score,
                        score_string);
  draw_text(score_string, 250, 10, font);
  glPopMatrix();

  /*
   * Put the current game time in the top left of the screen. This counts up.
   */
  glPushMatrix();
  glLoadIdentity();
  convert_timer_to_text(match_state->match_stats->game_timer, game_time_string);
  draw_text(game_time_string, 10, 10, font);
  glPopMatrix();

  /*
   * This performs the actual update of the screen so until this line nothing
   * we have done will be rendered each time around the render loop.
   */
  SDL_GL_SwapBuffers();
}

/*
 * detect_and_handle_collisions
 *
 * Polls all of the players to determine whether they have arrived at their
 * location yet. If they have then it throws an event to the player so that the
 * AI can handle.
 *
 * Parameters: automaton - Required to have access to the events.
 *             teams - All the players.
 *             players_per_team - Numbers of players in each team.
 *             ms_per_frame - Used to find how far each player will travel in
 *                            the next frame.
 */
void detect_players_arrived_at_location(AUTOMATON *automaton,
                                        TEAM **teams,
                                        int players_per_team,
                                        Uint32 ms_per_frame)
{
  /*
   * Local Variables.
   */
  PLAYER *player;
  float dist_per_frame;
  float s_per_frame = ((float) ms_per_frame) / MILLISECONDS_PER_SECOND;
  int ii;
  int jj;

  for (ii = 0; ii < players_per_team; ii++)
  {
    for (jj = 0; jj < 2; jj++)
    {
      player = teams[jj]->players[ii];

      if (vectors_equal(&(player->position), &(player->desired_position)))
      {
        /*
         * Calculate the distance that the player travels per update based on
         * their current speed.
         */
        dist_per_frame = s_per_frame *
            (player->current_speed_percent / 100.0f) * player->max_speed;

        /*
         * If the player is within one frame of reaching the desired position
         * then throw a new event to inform the ai and let it decide what to do
         * next.
         *
         * TODO: If the AI doesn't handle an arrived at desired position event
         * then it could get thrown multiple times. How should this be handled?
         */
        if (dist_between_vectors_2d(&(player->desired_position),
                                    &(player->position)) < dist_per_frame)
        {
          throw_single_player_ai_event_by_name(player,
                                       automaton,
                                       AUTOMATON_EVENT_ARRIVED_AT_LOCATION);
        }
      }
    }
  }
}

/*
 * convert_timer_to_text
 */
void convert_timer_to_text(TIMER *timer, char *text)
{
  /*
   * Local Variables.
   */
  Uint32 num_ms_passed;
  Uint32 num_s_passed;
  Uint32 num_m_passed;
  Uint32 num_h_passed;

  /*
   * Use constants to find the number of seconds, minutes and hours since the
   * timer was initialised.
   */
  num_ms_passed = SDL_GetTicks() - timer->start_time_ms;
  num_s_passed = (Uint32) (num_ms_passed / MILLISECONDS_PER_SECOND);
  num_m_passed = (Uint32) (num_s_passed / SECONDS_PER_MINUTE);
  num_h_passed = (Uint32) (num_m_passed / MINUTES_PER_HOUR);

  /*
   * Convert those into the format hh:mm:ss in text.
   */
  sprintf(text, "%i:%i:%i", num_h_passed, num_m_passed, num_s_passed);
}

/*
 * convert_score_to_text
 *
 * Utility function used to convert from two scores as integers into a text
 * representation to put on the screen.
 *
 * Parameters: score_a - The score that will appear on the left of the pair.
 *             score_b - The score that will appear on the right.
 *             score - The text representation of the score. If either score is
 *                     over 99 then this will be an empty string.
 */
void convert_score_to_text(unsigned int score_a,
                           unsigned int score_b,
                           char *score)
{
  /*
   * Prevent overflows by reporting an invalid score if the score is more than
   * 2 digits.
   */
  if (score_a >= 100 || score_b >= 100)
  {
    score[0] = '\0';
  }
  else
  {
    sprintf(score, "%i - %i", score_a, score_b);
  }
}

/*
 * draw_players
 *
 * Draw the players on the screen.
 *
 * Parameters: teams - Contains a link to all the players on the pitch.
 *             players_per_team - Used to iterate over the player array.
 *             animation_handler - Contains information on the animations used
 *                                 for the players.
 */
void draw_players(TEAM *teams[],
                  int players_per_team,
                  ANIMATION_HANDLER *animation_handler)
{
  /*
   * Local Variables.
   */
  int ii;

  for (ii = 0; ii < players_per_team; ii++)
  {
    draw_player(teams[0]->players[ii], animation_handler);
    draw_player(teams[1]->players[ii], animation_handler);
  }
}

/*
 * game_exit
 *
 * Parameters: message - The log message to output on exit.
 */
void game_exit(char *message)
{
  /*
   * Exit SDL and the TTF subsystem freeing the screen buffers etc.
   */
  TTF_Quit();
  SDL_Quit();

  /*
   * If there was a message passed into this function the print it out to
   * stdout.
   */
  if (NULL != message)
  {
    printf("%s\n", message);
  }

  DT_KILL_LOG;

  exit(0);
}
