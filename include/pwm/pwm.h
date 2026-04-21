#ifndef PWM_H
#define PWM_H

void init_pwm(void);
void set_right_motor_speed(float speed);
void set_left_motor_speed(float speed);
int areMotorsOn(void);

#endif
