#define PIRLED D4
#define PIR D1


void setup() {

  pinMode(PIR, INPUT);   // declare sensor as input
  pinMode(PIRLED, OUTPUT);  // declare LED as output
  digitalWrite(PIRLED, LOW);
  Serial.begin(9600);
}

void loop() {

  long state = digitalRead(PIR);
    if(state == HIGH) {
      digitalWrite (PIRLED, HIGH);
      Serial.println("Motion detected!");
      delay(1000);
    }
    else {
      digitalWrite (PIRLED, LOW);
      Serial.println("Motion absent!");
      delay(1000);
      }
}
