#include "timer.h"

volatile uint32_t timer_ticks = 0;

void init_timer(uint32_t frequency) {
    // find the frequency
    uint32_t divisor = 1193180 / frequency;
    
    // send byte
    outb(PIT_COMMAND, 0x36);
    
    // send frequency
    outb(PIT_CHANNEL_0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL_0, (uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t get_timer_ticks(void) {
    return timer_ticks;
}

void sleep(uint32_t milliseconds) {
    uint32_t target = timer_ticks + milliseconds;
    while (timer_ticks < target) {
        asm("hlt"); // wait until next interrupt
    }
}

void timer_handler(void) {
    timer_ticks++;
}
