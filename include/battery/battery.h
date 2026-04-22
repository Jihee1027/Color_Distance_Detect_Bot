#ifndef BATTERY_H
#define BATTERY_H

void battery_init(void);
float battery_read_voltage(void);
float battery_get_voltage(void);
void battery_print_status(void);
void battery_monitor_task(void);

#endif
