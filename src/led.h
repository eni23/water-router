#include <Arduino.h>
#include <SimpleTimer.h>
#include <NeoPixelBus.h>
#include "config.h"


// defines
#ifndef __LED_CPP
#define __LED_CPP


// timer stuff
extern SimpleTimer timer;

// rgb led stuff
extern NeoPixelBus led;
extern RgbColor led_color_blue;
extern RgbColor led_color_red;
extern RgbColor led_color_green;


extern boolean led_blink_state;
extern boolean led_blink_enabled;
extern RgbColor led_blink_color;
extern int led_blink_timer;

extern void led_init();

extern void led_set( RgbColor color );

extern void led_off();

extern void led_blink_cycle( );

extern void led_blink( RgbColor color, int cycle_time );

extern void led_blink_times( RgbColor color, int cycle_time, int times );

extern void led_blink_stop();

extern void led_timeout( RgbColor color, int time );

extern void led_timeout_ui( RgbColor color, int time);


#endif
