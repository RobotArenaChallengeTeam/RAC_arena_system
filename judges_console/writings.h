unsigned char waiting[] = {
  0x00,
  0x00,
  0x1E,
  0x20,
  0x10,
  0x20,
  0x1E,  //w
  0x00,
  0x00,
  0x3E,
  0x0A,
  0x3E,  //a
  0x00,
  0x00,
  0x3E,  //i
  0x00,
  0x00,
  0x02,
  0x3E,
  0x02,  //t
  0x00,
  0x00,
  0x3E,  //i
  0x00,
  0x00,
  0x3E,
  0x08,
  0x10,
  0x3E,  //n
  0x00,
  0x00,
  0x3E,
  0x22,
  0x32,  //g
  0x00,
  0x00,
};

unsigned char ready[] = {
  0x00,
  0x00,
  0x3E,
  0x12,
  0x2E,  //r
  0x00,
  0x00,
  0x3E,
  0x2A,
  0x22,  //E
  0x00,
  0x00,
  0x3E,
  0x0A,
  0x3E,  //a
  0x00,
  0x00,
  0x3E,
  0x22,
  0x1C,  //D
  0x00,
  0x00,
  0x06,
  0x38,
  0x06,  //Y
  0x00,
  0x00,
};

unsigned char pause[] = {
  0x00,
  0x00,
  0x3E,
  0x0A,
  0x0E,  //p
  0x00,
  0x00,
  0x3E,
  0x0A,
  0x3E,  //a
  0x00,
  0x00,
  0x3E,
  0x20,
  0x3E,  //U
  0x00,
  0x00,
  0x2E,
  0x2A,
  0x3A,  //S
  0x00,
  0x00,
  0x3E,
  0x2A,
  0x22,  //E
  0x00,
  0x00,
};

unsigned char end[] = {
  0x00,
  0x3E,
  0x2A,
  0x22,  //E
  0x00,
  0x00,
  0x3E,
  0x08,
  0x10,
  0x3E,  //n
  0x00,
  0x00,
  0x3E,
  0x22,
  0x1C,  //D
  0x00,
};

unsigned char blue_tap[] = {
  0x00,
  0x00,
  0x3E,
  0x2A,
  0x14,  //B
  0x00,
  0x00,
  0x3E,
  0x20,
  0x20,  //L
  0x00,
  0x00,
  0x3E,
  0x20,
  0x3E,  //U
  0x00,
  0x00,
  0x3E,
  0x2A,
  0x22,  //E
  0x00,
  0x00,
  0x00,
  0x02,
  0x3E,
  0x02,  //t
  0x00,
  0x00,
  0x3E,
  0x0A,
  0x3E,  //a
  0x00,
  0x00,
  0x3E,
  0x0A,
  0x0E,  //p
  0x00,
  0x00,
};

unsigned char yellow_tap[] = {
  0x00,
  0x00,
  0x06,
  0x38,
  0x06,  //Y
  0x00,
  0x00,
  0x3E,
  0x2A,
  0x22,  //E
  0x00,
  0x00,
  0x3E,
  0x20,
  0x20,  //L
  0x00,
  0x00,
  0x3E,
  0x20,
  0x20,  //L
  0x00,
  0x00,
  0x3E,
  0x22,
  0x3E,  //o
  0x00,
  0x00,
  0x1E,
  0x20,
  0x10,
  0x20,
  0x1E,  //w
  0x00,
  0x00,
  0x00,
  0x02,
  0x3E,
  0x02,  //t
  0x00,
  0x00,
  0x3E,
  0x0A,
  0x3E,  //a
  0x00,
  0x00,
  0x3E,
  0x0A,
  0x0E,  //p
  0x00,
  0x00,
};