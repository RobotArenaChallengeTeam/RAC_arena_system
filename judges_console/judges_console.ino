#include <FastLED.h>
#include "led_display.h"
#include "writings.h"
#define NUM_LEDS 1     // Numero de leds
#define LEDRGB_PIN 10  // LEDRGB_PIN ARDUINO
#define BTN_LED 6
#define BTN_PIN 4
#define LED_INCR 30

CRGB leds[NUM_LEDS];

enum State { STANDBY,
             PLAYER_READY,
             COUNTDOWN,
             FIGHT,
             PAUSE,
             END
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

State current_state = STANDBY;
int match_length = 180;
int fight_seconds = match_length;
int countdown_i = 0;
bool player_b_ready = false;
bool player_y_ready = true;
bool player_b_tap = false;
bool player_y_tap = false;
unsigned long current_time = 0;
unsigned long countdown_time = 0;
unsigned long match_time = 0;

void setup() {
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial.begin(9600);
  pinMode(BTN_LED, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  init_led_display();
  FastLED.addLeds<WS2812, LEDRGB_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();
  sendCmd(200);
}

void loop() {
  current_time = millis();
  handle_state();
  handle_consoles();
  delay(50);
}
bool btn_hold = false;
unsigned long btn_press_time = 0;

void handle_consoles() {
  bool btn_press = !digitalRead(BTN_PIN);
  if (btn_press && !btn_hold) {
    btn_hold = true;
    btn_press_time = current_time;
  } else if (!btn_press) {
    if (btn_hold == true && current_time - btn_press_time < 1000 && current_time - btn_press_time > 150) {
      transition(SET);
    }
    btn_hold = false;
  }
  if (btn_hold) {
    if (current_time - btn_press_time > 3000) {
      transition(RESET);
    }
  }
  if (Serial1.available()) {
    char cmd_b = Serial1.read();
    if (cmd_b == 'r') {
      transition(PLAYER_B_R);
    }
    if (cmd_b == 't') {
      transition(PLAYER_B_T);
    }
  }
  if (Serial2.available()) {
    char cmd_y = Serial2.read();
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
void scroll_writing(unsigned char writing[], int size) {
  display_matrix_scroll(writing, scrol_off, size);
  if (current_time - scroll_time > 150) {
    scroll_time = current_time;
    scrol_off = (scrol_off + 1) % size;
  }
}
void handle_state() {
  switch (current_state) {
    case STANDBY:
      scroll_writing(waiting, sizeof(waiting));
      leds[0] = CRGB(255, 255, 0);
      FastLED.show();
      break;
    case PLAYER_READY:
      scroll_writing(ready, sizeof(ready));
      leds[0] = CRGB(255, 255, 0);
      FastLED.show();
      break;
    case COUNTDOWN:
      display_seconds(fight_seconds);
      if (current_time - countdown_time < 500) {
        leds[0] = CRGB(0, 255, 0);
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
      if (current_time - match_time >= 1000) {
        match_time = current_time;
        sendCmd(fight_seconds);
        display_seconds(fight_seconds);
        fight_seconds--;
        if (fight_seconds < 0) {
          transition(TIMEOUT);
        }
      }
      leds[0] = CRGB(255, 255, 255);
      FastLED.show();
      break;
    case PAUSE:
      scroll_writing(pause, sizeof(pause));
      leds[0] = CRGB(255, 0, 0);
      FastLED.show();
      break;
    case END:
      if(player_b_tap){
        scroll_writing(blue_tap, sizeof(blue_tap));
      }else if(player_y_tap){
        scroll_writing(yellow_tap, sizeof(yellow_tap));
      }else{
        matrix_display(end);
      }
      
      leds[0] = CRGB(255, 0, 0);
      FastLED.show();
      break;
  }
}

void transition(int transition) {
  State prev_state = current_state;
  switch (current_state) {
    case STANDBY:
      if (transition == PLAYER_B_R) {
        player_b_ready = true;
        sendCmd(201);
      }
      if (transition == PLAYER_Y_R) {
        player_b_ready = true;
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
        player_y_tap = true;
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
    current_state = STANDBY;
    match_time = 0;
    player_b_ready = false;
    player_y_ready = true;
    player_b_tap = false;
    player_y_tap = false;
  }
  if (current_state != prev_state) {
    scrol_off = 0;
    scroll_time = 0;
    switch (current_state) {
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
      Serial1.println(201);
    }
    if (player_y_ready) {
      Serial2.println(201);
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