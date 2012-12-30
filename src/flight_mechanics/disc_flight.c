/*
 * disc_flight.c
 *
 * This file contains the functions which can be called to update the disc
 * positions based on a discrete time step.
 *
 *  Created on: 6 Feb 2010
 *      Author: David
 */

#include "../dt_logger.h"

#include <math.h>
#include "../disc.h"
#include "disc_forces.h"
#include "../physics.h"
#include "disc_flight_constants.h"
#include "disc_dimensions.h"
#include "../data_structures/vector.h"

/*
 * update_disc_up_axis
 *
 * After calculating the new velocities of the disc call this function to update
 * the direction that the up axis points. This provides us the ability to
 * stabilise flight that starts out as IO and other complex flight mechanics.
 *
 * The method is to rotate the up vector around each of the other primary axis
 * using the roll and pitch velocity to measure the number of radians to rotate
 * by.
 *
 * Parameters: old_d3 - The current up vector.
 *             d1 - The axis on the disc plane parallel to the velocity.
 *             roll_vel - The roll velocity in radians/s
 *             pitch_vel - The pitch velocity in radians/s
 *             time_step_s - The number of seconds elapsed since last update.
 *
 * Returns: A new up vector as updated using the equations detailed inline.
 */
VECTOR3 update_disc_up_axis(VECTOR3 *old_d3,
                           VECTOR3 *d1,
                           float roll_vel,
                           float pitch_vel,
                           float time_step_s)
{
  /*
   * Local Variables.
   */
  float roll_angle = roll_vel * time_step_s;
  float pitch_angle = pitch_vel * time_step_s;
  VECTOR3 d3;
  VECTOR3 d2 = vector_cross_product(old_d3, d1);

  /*
   * The disc pitches around the d2 axis and rolls around the d1 axis so since
   * these are orthogonal axis the order of rotation is irrelevant.
   */
  d3 = rotate_vector_around_vector(old_d3, roll_angle, d1);
  d3 = rotate_vector_around_vector(&d3, pitch_angle, &d2);

  DT_DEBUG_LOG("Update up vector on disc: \n\told vector (%f, %f, %f), " \
               "d1 = (%f, %f, %f), d2 = (%f, %f, %f), " \
               "\n\troll_vel = %f, pitch_vel = %f, dt = %f" \
               "\n\tResult: (%f, %f, %f)\n",
               old_d3->x, old_d3->y, old_d3->z, d1->x, d1->y, d1->z,
               d2.x, d2.y, d2.z, roll_vel, pitch_vel, time_step_s,
               d3.x, d3.y, d3.z);

  return(d3);
}

/*
 * find_disc_plane_axis
 *
 * Given the velocity vector and the current up vector on the disc use this
 * function to find the axis on the disc plane along the direction of travel.
 *
 * Parameters: velocity - The velocity vector of the disc.
 *             d3 - The up vector on the disc. This should be normalised.
 */
VECTOR3 find_disc_plane_axis(VECTOR3 *velocity,
                             VECTOR3 *d3)
{
  /*
   * Local Variables.
   */
  float component_of_vel_on_d3;
  VECTOR3 vel_on_d3;
  VECTOR3 d1;

  /*
   * To calculate the vector d1 we use the equation:
   * d1 = v - (v.d3 * d3) normalised
   *
   * So removing the component of v which is along the d3 axis. Since v is
   * orthogonal to d2 by choice of d1,d2,d3 this means that we don't need to
   * remove the component of d1 along d2 (it is zero).
   */
  component_of_vel_on_d3 = vector_dot_product(velocity, d3);
  vel_on_d3 = mult_vector_by_scalar(d3, component_of_vel_on_d3);
  d1 = vector_minus(velocity, &vel_on_d3);

  /*
   * As we are using d1 as an axis we make it of unit length in its current
   * direction.
   */
  d1 = unit_vector(&d1);

  DT_DEBUG_LOG("Finding d1: \n\tvelocity = (%f,%f,%f), \n\td3 = (%f,%f,%f) =>"\
               " \n\td1 = (%f,%f,%f)\n",
               velocity->x, velocity->y, velocity->z,
               d3->x, d3->y, d3->z, d1.x, d1.y, d1.z);

  return(d1);
}

/*
 * get_disc_acc_from_forces
 *
 * There are only 3 forces on which the acceleration of the disc directly
 * depends. The DRAG, LIFT and force due to gravity.
 *
 * Parameters: disc_forces - The forces acting on the disc.
 *             velocity - The previous velocity vector.
 *             acceleration - The acceleration of the disc.
 *             d1 - The axis of the disc along the disc plane.
 *             d3 - The axis of the disc pointing upwards from the disc plane.
 */
void get_disc_acc_from_forces(DISC_FORCES *disc_forces,
                              VECTOR3 *velocity,
                              VECTOR3 *acceleration,
                              VECTOR3 *d1,
                              VECTOR3 *d3)
{
  /*
   * Local Variables.
   */
  VECTOR3 acc_due_to_gravity;
  float alpha = smallest_angle_between_vectors(velocity, d1);
  float sin_alpha = sinf(alpha);
  float cos_alpha = cosf(alpha);
  VECTOR3 projection_to_d1;
  VECTOR3 projection_to_d3;
  float component_of_lift_on_d1 = disc_forces->lift_force * sin_alpha /
                                  DISC_MASS_KG;
  float component_of_lift_on_d3 = disc_forces->lift_force * cos_alpha /
                                  DISC_MASS_KG;
  float component_of_drag_on_d1 = disc_forces->drag_force * cos_alpha /
                                  DISC_MASS_KG;
  float component_of_drag_on_d3 = disc_forces->drag_force * sin_alpha /
                                  DISC_MASS_KG;

  /*
   * Calculate the projection of the lift and drag on d1 (the axis of the disc
   * plane along which the disc is pointing).
   *
   * Note that drag acts negatively along this axis.
   */
  projection_to_d1 = mult_vector_by_scalar(d1,
                                           component_of_lift_on_d1 -
                                           component_of_drag_on_d1);

  /*
   * Calculate the projection of the lift and the drag on d3 (the axis of the
   * disc perpendicular to d1 and pointing up from the centre of the disc).
   *
   * Note that both drag and lift acts positively on this axis.
   */
  projection_to_d3 = mult_vector_by_scalar(d3,
                                           component_of_drag_on_d3 +
                                           component_of_lift_on_d3);

  /*
   * Set up the gravity acceleration vector.
   */
  acc_due_to_gravity.x = 0.0f;
  acc_due_to_gravity.y = 0.0f;
  acc_due_to_gravity.z = GRAVITY_ACC_M_PER_S2;

  /*
   * To get the acceleration vector add the two components of the acceleration
   * along the two primary disc axis.
   *
   * There are no disc forces acting along the axis d2 which is why it does not
   * appear in any of these calculations.
   */
  *acceleration = vector_add(&projection_to_d1, &projection_to_d3);
  *acceleration = vector_add(acceleration, &acc_due_to_gravity);
}

/*
 * update_extra_vels_from_disc_forces
 *
 * This function adjust the spin, roll and pitch velocities associated with the
 * disc object based on the forces acting on the disc.
 *
 * Call this function after calling calc_disc_forces but before updating d3.
 *
 * Parameters: disc - The disc object whose spin, roll, pitch will be updated.
 *             disc_forces - The forces acting on the disc.
 *             time_step_s - The number of s since the last update.
 */
void update_extra_vels_from_disc_forces(DISC *disc,
                                        DISC_FORCES *disc_forces,
                                        float time_step_s)
{
  /*
   * Update the angular velocities. Note that at present we have assumed that
   * the first derivative of spin velocity w.r.t time is 0.
   */
  disc->angular_velocity.x = (2.0f * disc_forces->pitch_moment) /
                              (disc->angular_velocity.z * DISC_AXIAL_INERTIA);
  disc->angular_velocity.y = (2.0f * disc_forces->roll_moment) /
                              (disc->angular_velocity.z * DISC_AXIAL_INERTIA);

  DT_DEBUG_LOG("Computed new angular velocities:" \
               "\n\tpitch_vel = %f, roll velocity = %f\n",
               disc->angular_velocity.x, disc->angular_velocity.y);
}

/*
 * calc_disc_forces
 *
 * Utility function for the frisbee flight mechanics model. Takes the dependent
 * variables and fills in a set of the forces that are acting on the disc.
 *
 * The disc forces object must be created before calling to this function.
 *
 * Parameters: disc_velocity - The velocity vector of the disc.
 *             angular_velocity - The last angular velocity calculated.
 *             alpha - The angle between the velocity vector and the disc plane.
 *
 * Returns: A disc forces structure containing the new values.
 */
DISC_FORCES calc_disc_forces(VECTOR3 *disc_velocity,
                             VECTOR3 *angular_velocity,
                             float alpha)
{
  /*
   * Local Variables.
   */
  float pitch_vel = angular_velocity->x;
  float roll_vel = angular_velocity->y;
  float spin_vel = angular_velocity->z;
  DISC_FORCES output_forces;
  float spd_sqrd = vector_dot_product(disc_velocity, disc_velocity);
  float coefficient = (ATMOSPHERIC_DENSITY_KG_PER_M3 * DISC_AREA_M2 * spd_sqrd)
                      / 2;
  float diam_coeff = coefficient * DISC_DIAMETER_M;
  float alpha2 = alpha + CONST_L_ZERO / CONST_L_ALPHA;

  /*
   * These calculations are taken from the flight mechanics model given in
   * Hummell, Hubbard 2002 (Identification of frisbee aerodynamic coefficients
   * using flight data).
   */
  output_forces.lift_force = (CONST_L_ZERO + CONST_L_ALPHA * alpha) *
                             coefficient;
  output_forces.drag_force = (CONST_D_ZERO + CONST_D_ALPHA * alpha2 * alpha2) *
                             coefficient;
  output_forces.roll_moment = (CONST_R_R * spin_vel + CONST_R_P * roll_vel) *
                              diam_coeff;
  output_forces.pitch_moment = (CONST_M_ZERO + CONST_M_ALPHA * alpha +
                                CONST_M_Q * pitch_vel) * diam_coeff;
  output_forces.spin_down_moment = CONST_N_R * spin_vel * diam_coeff;

  DT_DEBUG_LOG("Calculated forces on disc from: \n\tvel = (%f, %f, %f)," \
                "\n\troll = %f, pitch = %f, spin = %f and alpha = %f.\n" \
                "Returned values are: \n\tlift = %f, drag = %f, pitch = %f," \
                "roll = %f, spin = %f\n",
                disc_velocity->x, disc_velocity->y, disc_velocity->z,
                roll_vel, pitch_vel, spin_vel, alpha,
                output_forces.lift_force, output_forces.drag_force,
                output_forces.pitch_moment, output_forces.roll_moment,
                output_forces.spin_down_moment);

  return(output_forces);
}
