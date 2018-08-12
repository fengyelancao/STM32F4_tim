#ifndef __EXIT_H
#define __EXIT_H
#include "advance_pwm.h"

#define PWM_cap_IO_0 PEin(0)
#define PWM_cap_IO_1 PEin(2)
void EXTIX_Init(void);

void end_exit_0(void);
void end_exit_2(void);


void star_exit_0(void);
void star_exit_2(void);

#endif

