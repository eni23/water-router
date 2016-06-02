boolean led_blink_state = true;
boolean led_blink_enabled = false;
RgbColor led_blink_color = RgbColor(0, 0, 0);
int led_blink_timer;

void led_init(){
  led.Begin();
  led.Show();
}

void led_set( RgbColor color ){
  led.SetPixelColor(0, color);
  led.Show();
}

void led_off(){
  led.SetPixelColor(0, RgbColor(0,0,0));
  led.Show();
}


void led_blink( RgbColor color, int cycle_time ){
  led_set(color);
  led_blink_color = color;
  led_blink_timer = timer.setInterval(cycle_time, led_blink_cycle);
  led_blink_enabled = true;
}

void led_blink_times( RgbColor color, int cycle_time, int times ){
  led_blink_color = color;
  led_blink_timer = timer.setTimer(cycle_time, led_blink_cycle, times*2);
}

void led_blink_stop(){
  timer.deleteTimer(led_blink_timer);
  led_off();
  led_blink_enabled = false;
}

void led_blink_cycle( ){
  if (led_blink_state){
    led_set(led_blink_color);
    led_blink_state = false;
  }
  else {
    led_off();
    led_blink_state=true;
  }
}


void led_timeout( RgbColor color, int time ){
  led_set(color);
  timer.setTimeout(time, led_off);
}

void led_timeout_ui( RgbColor color, int time){
  if (!led_blink_enabled){
    led_timeout(color, time);
  }
}
