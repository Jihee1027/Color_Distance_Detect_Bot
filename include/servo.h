#ifndef SERVO_H
#define SERVO_H

void servo_init(void);
void servo_set_angle(float angle);
void servo_move_by(float delta_degrees);
float servo_get_current_angle(void);

#endif