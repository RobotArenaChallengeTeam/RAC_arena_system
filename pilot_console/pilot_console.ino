#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define BTN_LED 10
#define BTN_PIN 16
#define LED_INCR 30

#define i2c_Address 0x3c  //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     //   QT-PY / XIAO

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum States {
  READY,
  SET,
  FIGHT,
  PAUSE,
  END
};

int secs = 180;
int cmd = 200;
int led_brightness = 0;
int brightness_dir = 1;
bool btn_hold = false;
String inString = "";
unsigned long current_time = 0;
States current_state = READY;
void setup() {
  pinMode(BTN_LED, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  delay(250);                        // wait for the OLED to power up
  display.begin(i2c_Address, true);  // Address 0x3C default
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  //display.setContrast (1); // dim display
  // invert the display
  //display.invertDisplay(true);
  //display.invertDisplay(false);
  for (int w_i = 0; w_i < 10; w_i++) {
    digitalWrite(10, LOW);
    display.clearDisplay();
    display.setCursor(0, 0);
    for (int d_i = 0; d_i < 10-w_i; d_i++) {
      	display.print(".");
    }
    display.display();
    delay(1000);
  }
  display.setTextSize(4);
  Serial1.begin(9600);
}



void loop() {
  bool btn_press = !digitalRead(BTN_PIN);
  bool btn_just_pressed = false;
  if (btn_press && !btn_hold) {
    btn_hold = true;
    btn_just_pressed = true;
  } else if (!btn_press) {
    btn_hold = false;
  }
  while (Serial1.available() > 0) {
    int inChar = Serial1.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      int inval = inString.toInt();
      if (inval > 180) {
        cmd = inval;
      }
      if (inval <= 180) {
        cmd = inval;
        secs = inval;
      }
      inString = "";
    }
  }
  if (cmd <= 180) {
    current_state = FIGHT;
  } else if (cmd == 200) {
    current_state = READY;
  } else if (cmd == 201) {
    current_state = SET;
  } else if (cmd == 202) {
    current_state = PAUSE;
  } else if (cmd == 203) {
    current_state = END;
  }
  switch (current_state) {
    case READY:
      if (btn_just_pressed) {
        Serial1.println("r");
      }
      led_fade_step();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(" GET");
      display.println("Ready");
      display.display();
      break;
    case SET:
      digitalWrite(10, LOW);
      display.clearDisplay();
      display.setCursor(20, 20);
      display.println("SET");
      display.display();
      break;
    case FIGHT:
      if (btn_just_pressed) {
        Serial1.println("t");
      }
      digitalWrite(10, HIGH);
      led_brightness = 255;
      display.clearDisplay();
      display.setCursor(20, 20);
      display.println(time_from_int(secs));
      display.display();
      break;
    case PAUSE:
      digitalWrite(10, LOW);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("PAUSE");
      display.println(time_from_int(secs));
      display.display();
      break;
    case END:
      digitalWrite(10, LOW);
      led_brightness = 0;
      display.clearDisplay();
      display.setCursor(20, 20);
      display.println("END");
      display.display();
      break;
  }
}

void led_fade_step() {
  analogWrite(BTN_LED, led_brightness);
  led_brightness = led_brightness + LED_INCR * brightness_dir;
  led_brightness = constrain(led_brightness, 0, 255);
  if (led_brightness == 0 || led_brightness == 255) {
    brightness_dir *= -1;
  }
}

String time_from_int(int secs) {
  String time_str = "0:";
  if (secs >= 60) {
    secs = secs - 60;
    time_str = "1:";
  }
  if (secs >= 60) {
    secs = secs - 60;
    time_str = "2:";
  }
  if (secs >= 60) {
    secs = secs - 60;
    time_str = "3:";
  }
  if (secs < 10) {
    time_str += "0";
  }
  time_str += String(secs);
  return time_str;
}