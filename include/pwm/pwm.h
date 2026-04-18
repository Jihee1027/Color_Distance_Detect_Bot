#ifndef PWM_H
#define PWM_H

void init_pwm(void);
void set_right_motor_speed(double speed);
void set_left_motor_speed(double speed);
int areMotorsOn(void);

#endif