#include <Wire.h>

#include "MainThread.h"

MainThread *mainThread;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClockStretchLimit(2500);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  mainThread = new MainThread();
}

void loop() { mainThread->loop(); }
