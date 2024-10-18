#define APWM_PIN 53
#define AIN1_PIN 51 
#define AIN2_PIN 49 
#define STBY_PIN 47
#define BPWM_PIN 45
#define BIN1_PIN 43 
#define BIN2_PIN 41

void init_pit(){
  pinMode(APWM_PIN, OUTPUT);
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);

  pinMode(STBY_PIN, OUTPUT);

  pinMode(BPWM_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);

  digitalWrite(APWM_PIN, LOW);
  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(AIN2_PIN, LOW);

  digitalWrite(STBY_PIN, HIGH);

  digitalWrite(BPWM_PIN, LOW);
  digitalWrite(BIN1_PIN, LOW);
  digitalWrite(BIN2_PIN, LOW);
}


void pit_up(){
  digitalWrite(APWM_PIN, HIGH);
  digitalWrite(AIN1_PIN, HIGH);
  digitalWrite(AIN2_PIN, LOW);
}

void pit_down(){
  digitalWrite(APWM_PIN, HIGH);
  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(AIN2_PIN, HIGH);
}

void pit_stop(){
  digitalWrite(APWM_PIN, LOW);
  digitalWrite(AIN1_PIN, HIGH);
  digitalWrite(AIN2_PIN, HIGH);
}