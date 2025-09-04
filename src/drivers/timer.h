#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <system.h>

#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_1 0x41
#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND   0x43

extern volatile uint32_t timer_ticks;

void init_timer(uint32_t frequency);
uint32_t get_timer_ticks(void);
void sleep(uint32_t milliseconds);

#endif
