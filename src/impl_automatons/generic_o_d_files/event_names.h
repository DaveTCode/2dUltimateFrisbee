/*
 * event_names.h
 *
 *  Created on: 26 May 2010
 *      Author: David
 */

#ifndef EVENT_NAMES_H_
#define EVENT_NAMES_H_

/*
 * The total number of events as named below.
 */
#define NUM_AUTOMATON_EVENTS 8

/*
 * Strings used to name the events in the automatons.
 */
#define AUTOMATON_EVENT_DISC_CAUGHT "event_disc_caught"
#define AUTOMATON_EVENT_DISC_RELEASED "event_disc_released"
#define AUTOMATON_EVENT_PULL_THROWN "event_pull_thrown"
#define AUTOMATON_EVENT_PULL_DROPPED "event_pull_dropped"
#define AUTOMATON_EVENT_PULL_CAUGHT "event_pull_caught"
#define AUTOMATON_EVENT_DISC_PICKED_UP "event_disc_picked_up"
#define AUTOMATON_EVENT_ARRIVED_AT_LOCATION "event_arrived_at_location"
#define AUTOMATON_EVENT_DISC_HIT_FLOOR "event_disc_hit_floor"

#endif /* EVENT_NAMES_H_ */
