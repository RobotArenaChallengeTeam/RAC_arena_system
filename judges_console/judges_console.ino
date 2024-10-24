#include <FastLED.h>
#include "led_display.h"
#include "writings.h"
#include "pit_motor.h"

#define LEDRGB_PIN 5
#define BTN_LED 3
#define BTN_PIN 2
#define OVERRIDE_PIN A11

#define LED_INCR 20
#define NUM_LEDS 200  // Numero de leds
#define PIT_MOVE_TIME 5000
CRGB leds[NUM_LEDS];

enum State { OFF,
             STANDBY,
             PLAYER_READY,
             COUNTDOWN,
             FIGHT,
             PAUSE,
             END
};

enum PitCommand { PIT_UP,
                  PIT_DOWN,
                  PIT_STOP
};

enum Transition { RESET,
                  SET,
                  PLAYER_B_R,
                  PLAYER_B_T,
                  PLAYER_Y_R,
                  PLAYER_Y_T,
                  START,
                  TIMEOUT
};

State current_state = OFF;
int match_length = 180;
int rac_length = 90;
int fight_seconds = match_length;
int countdown_i = 0;
bool player_b_ready = false;
bool player_y_ready = false;
bool player_b_tap = false;
bool player_y_tap = false;
bool override_mode = false;
PitCommand pit_cmd = PIT_STOP;
PitCommand pit_state = PIT_STOP;
unsigned long pit_cmd_ms = 0;
unsigned long current_time = 0;
unsigned long countdown_time = 0;
unsigned long match_time = 0;

void setup() {
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial.begin(9600);
  pinMode(BTN_LED, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(OVERRIDE_PIN, INPUT_PULLUP);
  init_pit();
  if (!digitalRead(BTN_PIN)) {
    match_length = rac_length;
    fight_seconds = rac_length;
  }
  if (!digitalRead(OVERRIDE_PIN)) {
    override_mode = true;
  }
  init_led_display();
  FastLED.addLeds<WS2812, LEDRGB_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();

  sendCmd(200);
}

void loop() {
  handle_pit();
  current_time = millis();
  handle_state();
  handle_consoles();
  delay(20);
}

void handle_pit() {
  if (override_mode) {
    if (!digitalRead(OVERRIDE_PIN)) {
      pit_cmd = PIT_UP;
    } else {
      pit_cmd = PIT_DOWN;
    }
  }

  if (pit_state != pit_cmd) {
    if (pit_cmd == PIT_DOWN && digitalRead(OVERRIDE_PIN)) {
      pit_state = PIT_DOWN;
      pit_cmd_ms = current_time;
    }
    if (pit_cmd == PIT_UP && !digitalRead(OVERRIDE_PIN)) {
      pit_state = PIT_UP;
      pit_cmd_ms = current_time;
    }
  }

  if (current_time - pit_cmd_ms < PIT_MOVE_TIME) {
    if (pit_cmd == PIT_DOWN) {
      pit_down();
    }
    if (pit_cmd == PIT_UP) {
      pit_up();
    }
  } else {
    pit_stop();
  }
}
bool btn_hold = false;
unsigned long btn_press_time = 0;
bool longpress = false;

void handle_consoles() {
  bool btn_press = !digitalRead(BTN_PIN);
  if (btn_press && !btn_hold) {
    btn_hold = true;
    btn_press_time = current_time;
  } else if (!btn_press) {
    if (btn_hold == true && current_time - btn_press_time < 1000 && current_time - btn_press_time > 60) {
      transition(SET);
    }
    longpress = false;
    btn_hold = false;
  }
  if (btn_hold) {
    if (!longpress && current_time - btn_press_time > 3000) {
      longpress = true;
      transition(RESET);
    }
  }
  if (Serial2.available()) {
    char cmd_b = Serial2.read();
    if (cmd_b == 'r') {
      transition(PLAYER_B_R);
    }
    if (cmd_b == 't') {
      transition(PLAYER_B_T);
    }
  }
  if (Serial1.available()) {
    char cmd_y = Serial1.read();
    if (cmd_y == 'r') {
      transition(PLAYER_Y_R);
    }
    if (cmd_y == 't') {
      transition(PLAYER_Y_T);
    }
  }
}
int scrol_off = 0;
unsigned long scroll_time = 0;
void scroll_writing(unsigned char writing[], int size, int scroll_speed = 150) {
  display_matrix_scroll(writing, scrol_off, size);
  int ms_delta = (scroll_speed > 0) ? scroll_speed : -scroll_speed;
  if (current_time - scroll_time > ms_delta) {
    scroll_time = current_time;
    if (scroll_speed > 0) {
      scrol_off = (scrol_off + 1) % size;
    } else {
      scrol_off = (scrol_off - 1 + size) % size;
    }
  }
}
void handle_state() {
  switch (current_state) {
    case OFF:
      led_fade_step();
      scroll_writing(waiting, sizeof(waiting));
      roundround_strip(96);
      FastLED.show();
      break;
    case STANDBY:
      pit_cmd = PIT_UP;
      digitalWrite(BTN_LED, LOW);
      scroll_writing(waiting, sizeof(waiting));
      player_ready_led();
      FastLED.show();
      break;
    case PLAYER_READY:
      led_fade_step();
      scroll_writing(ready, sizeof(ready));
      player_ready_led();
      FastLED.show();
      break;
    case COUNTDOWN:
      digitalWrite(BTN_LED, LOW);
      scroll_writing(arrows, sizeof(arrows), 50);
      if (current_time - countdown_time < 500) {
        set_leds(CRGB(0, 255, 0));
        FastLED.show();
      } else {
        FastLED.clear();
        FastLED.show();
      }
      if (current_time - countdown_time > 1000) {
        Serial.println(300 + countdown_i);
        countdown_i--;
        countdown_time = current_time;
      }
      if (countdown_i < 0) {
        transition(START);
      }
      break;
    case FIGHT:
      digitalWrite(BTN_LED, HIGH);
      if (current_time - match_time >= 1000) {
        match_time = current_time;
        sendCmd(fight_seconds);
        display_seconds(fight_seconds);
        fight_seconds--;
        if (fight_seconds < 0) {
          transition(TIMEOUT);
        }
      }
      if (fight_seconds < 90) {
        pit_cmd = PIT_DOWN;
      }
      set_leds(CRGB(255, 255, 255));
      FastLED.show();
      break;
    case PAUSE:
      led_fade_step();
      scroll_writing(pause, sizeof(pause));
      set_leds(CRGB(255, 0, 0));
      FastLED.show();
      break;
    case END:
      digitalWrite(BTN_LED, LOW);
      if (player_b_tap) {
        scroll_writing(blue_tap, sizeof(blue_tap));
      } else if (player_y_tap) {
        scroll_writing(yellow_tap, sizeof(yellow_tap));
      } else {
        matrix_display(end);
      }
      set_leds(CRGB(255, 0, 0));
      FastLED.show();
      break;
  }
}

void transition(int transition) {
  State prev_state = current_state;
  switch (current_state) {
    case OFF:
      if (transition == SET){
        current_state = STANDBY;
      }
      break;
    case STANDBY:
      if (transition == PLAYER_B_R) {
        player_b_ready = true;
        sendCmd(201);
      }
      if (transition == PLAYER_Y_R) {
        player_y_ready = true;
        sendCmd(201);
      }
      if (player_y_ready && player_b_ready) {
        current_state = PLAYER_READY;
      }
      break;
    case PLAYER_READY:
      if (transition == SET) {
        current_state = COUNTDOWN;
      }
      break;
    case COUNTDOWN:
      if (transition == START) {
        current_state = FIGHT;
      }
      break;
    case FIGHT:
      if (transition == PLAYER_B_T) {
        player_b_tap = true;
        current_state = END;
      }
      if (transition == PLAYER_Y_T) {
        player_y_tap = true;
        current_state = END;
      }
      if (transition == SET) {
        current_state = PAUSE;
      }
      if (transition == TIMEOUT) {
        current_state = END;
      }
      break;
    case PAUSE:
      if (transition == SET) {
        current_state = COUNTDOWN;
      }
      break;
    case END:
      break;
  }
  if (transition == RESET) {
    fight_seconds = match_length;
    current_state = OFF;
    prev_state = END;
    match_time = 0;
    player_b_ready = false;
    player_y_ready = false;
    player_b_tap = false;
    player_y_tap = false;
  }
  if (current_state != prev_state) {
    scrol_off = 0;
    scroll_time = 0;
    switch (current_state) {
      case OFF:
        sendCmd(200);
      case STANDBY:
        sendCmd(200);
        break;
      case PLAYER_READY:
        break;
      case COUNTDOWN:
        sendCmd(201);
        countdown_time = 0;
        countdown_i = 3;
        break;
      case FIGHT:
        match_time = 0;
        break;
      case PAUSE:
        fight_seconds++;
        sendCmd(202);
        break;
      case END:
        sendCmd(203);
        break;
    }
  }
}

void sendCmd(int cmd) {
  Serial.println(cmd);
  if (cmd == 201) {
    if (player_b_ready) {
      Serial2.println(201);
    }
    if (player_y_ready) {
      Serial1.println(201);
    }
    return;
  }
  Serial1.println(cmd);
  Serial2.println(cmd);
}

int led_brightness = 0;
int brightness_dir = 1;
void led_fade_step() {
  analogWrite(BTN_LED, led_brightness);
  led_brightness = led_brightness + LED_INCR * brightness_dir;
  led_brightness = constrain(led_brightness, 0, 255);
  if (led_brightness == 0 || led_brightness == 255) {
    brightness_dir *= -1;
  }
}

void set_leds(CRGB color) {
  for (int l = 0; l < NUM_LEDS; l++) {
    leds[l] = color;
  }
}
int fade_y = 0;
int fade_b = 100;
void roundround_strip(int len) {
  for (int i = 0; i < NUM_LEDS; i++) {  //9948
    leds[i] = CRGB(255, 255, 255);
    //y
    if (i == fade_y) {
      leds[i] = CRGB(255, 150, 0);
    }
    if (i < fade_y + len && i > fade_y) {
      leds[i] = CRGB(255, 150, 0);
    }
    if (i < ((fade_y + len) % NUM_LEDS) && fade_y > NUM_LEDS - len) {
      leds[i] = CRGB(255, 150, 0);
    }
    //y
    if (i == fade_b) {
      leds[i] = CRGB(0, 0, 255);
    }
    if (i < fade_b + len && i > fade_b) {
      leds[i] = CRGB(0, 0, 255);
    }
    if (i < ((fade_b + len) % NUM_LEDS) && fade_b > NUM_LEDS - len) {
      leds[i] = CRGB(0, 0, 255);
    }

  }
  
    fade_y = (fade_y + 1 < NUM_LEDS) ? fade_y + 1 : 0;
    fade_b = (fade_b + 1 < NUM_LEDS) ? fade_b + 1 : 0;
}

void player_ready_led() {
  FastLED.clear();
  if (player_y_ready) {
    for (int l = 150; l < 250; l++) {
      leds[l % NUM_LEDS] = CRGB(255, 150, 0);
    }
  }
  if (player_b_ready) {
    for (int l = 50; l < 150; l++) {
      leds[l] = CRGB(0, 0, 255);
    }
  }
}