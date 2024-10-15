#define SCL_Pin 21  //Set clock pin to A5
#define SDA_Pin 20  //Set data pin to A4
char digid_starts[] = { 0, 7, 12 };
//unsigned char waiting[] = { 0x1C, 0x20, 0x10, 0x20, 0x1C, 0x00, 0x3C, 0x14, 0x3C, 0x00, 0x3C, 0x00, 0x04, 0x3C, 0x04, 0x00 };
unsigned char time_mx[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t digits[10][4] = {
  // 0
  { 0x7E, 0x81, 0x81, 0x7E },
  // 1
  { 0x00, 0x82, 0xFF, 0x80 },
  // 2
  { 0xC2, 0xA1, 0x91, 0x8E },
  // 3
  { 0x42, 0x89, 0x89, 0x76 },
  // 4
  { 0x0F, 0x10, 0x10, 0xFF },
  // 5
  { 0x4F, 0x89, 0x89, 0x71 },
  // 6
  { 0x7E, 0x89, 0x89, 0x72 },
  // 7
  { 0x01, 0x01, 0xF1, 0x0E },
  // 8
  { 0x76, 0x89, 0x89, 0x76 },
  // 9
  { 0x4E, 0x91, 0x91, 0x7E }
};




void init_led_display() {
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);
  digitalWrite(SDA_Pin, HIGH);
  digitalWrite(SCL_Pin, HIGH);
  //clear the display
  //matrix_display(clear);
}

void IIC_start() {
  digitalWrite(SCL_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, LOW);
  delayMicroseconds(3);
}
void IIC_send(unsigned char send_data) {
  for (char i = 0; i < 8; i++)  //Each byte has 8 bits 8bit for every character
  {
    digitalWrite(SCL_Pin, LOW);  // pull down clock pin SCL_Pin to change the signal of SDA
    delayMicroseconds(3);
    if (send_data & 0x01)  //set high and low level of SDA_Pin according to 1 or 0 of every bit
    {
      digitalWrite(SDA_Pin, HIGH);
    } else {
      digitalWrite(SDA_Pin, LOW);
    }
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, HIGH);  //pull up the clock pin SCL_Pin to stop transmission
    delayMicroseconds(3);
    send_data = send_data >> 1;  // detect bit by bit, shift the data to the right by one
  }
}
void IIC_end() {
  digitalWrite(SCL_Pin, LOW);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, LOW);
  delayMicroseconds(3);
  digitalWrite(SCL_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, HIGH);
  delayMicroseconds(3);
}
// the function for dot matrix display
void matrix_display(unsigned char matrix_value[]) {
  IIC_start();                  // use the function of the data transmission start condition
  IIC_send(0xc0);               //select address
  for (int i = 0; i < 16; i++)  //pattern data has 16 bits
  {
    IIC_send(matrix_value[i]);  //convey the pattern data
  }
  IIC_end();  //end the transmission of pattern data
  IIC_start();
  IIC_send(0x8A);  //display control, set pulse width to 4/16 s
  IIC_end();
}

void display_matrix_scroll(unsigned char matrix_value[],int offset,int size) {
  IIC_start();                  // use the function of the data transmission start condition
  IIC_send(0xc0);               //select address
  for (int i = offset; i < 16+offset; i++)  //pattern data has 16 bits
  {
    IIC_send(matrix_value[i%size]);  //convey the pattern data
  }
  IIC_end();  //end the transmission of pattern data
  IIC_start();
  IIC_send(0x8A);  //display control, set pulse width to 4/16 s
  IIC_end();
}
void insert_digit(char digit, char pos) {
  unsigned char col = 0x00;
  char index = digid_starts[pos];
  for (int i = 0; i < 4; i++) {
    time_mx[i + index] = digits[digit][i];
  }
}
void update_time_matrix(int secs) {
  if (secs >= 180) {
    insert_digit(3, 0);
    insert_digit(0, 1);
    insert_digit(0, 2);
    return;
  } else if (secs >= 120) {
    secs = secs - 120;
    insert_digit(2, 0);
  } else if (secs >= 60) {
    secs = secs - 60;
    insert_digit(1, 0);
  } else {
    insert_digit(0, 0);
  }
  if (secs < 10) {
    insert_digit(0, 1);
    insert_digit(secs, 2);
    return;
  }
  int decs = secs / 10;
  insert_digit(decs, 1);
  insert_digit(secs - decs * 10, 2);
}
void display_seconds(int secs) {
  update_time_matrix(secs);
  matrix_display(time_mx);
}
