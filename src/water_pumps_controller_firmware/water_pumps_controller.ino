#include <TinyWireS.h>

#define I2C_SLAVE_ADDRESS 0x08 // Address of the slave
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

#define PUMP_1_PIN 1
#define PUMP_2_PIN A3
#define PUMP_3_PIN 4

int inline get_pin(byte data) {

  switch (data) {
    case 0x01:
    case 0xFF:
      return PUMP_1_PIN;
    case 0x02:
      return PUMP_2_PIN;
    case 0x03:
      return PUMP_3_PIN;
    default:
      return -1;
  }
}

void receiveEvent(uint8_t num_bytes) {
  if (num_bytes < 2 || num_bytes > TWI_RX_BUFFER_SIZE) return;

  int pin; bool state = false; bool clr = false;

  if (num_bytes % 2 == 1) {
    num_bytes--;
    clr = true;
  }

  for (int i = 0; i < num_bytes; i++) {
    if (i % 2 == 0) {
      pin = get_pin(TinyWireS.receive());

    } else {
      if (pin < 0) {
        continue;
      }
      byte swt = TinyWireS.receive();
      state = swt == 0x01 || swt == 0xFF;
      digitalWrite(pin, state ? HIGH : LOW);
    }
  }
  if (clr) {
    TinyWireS.receive();
  }
}

void setup() {
  TinyWireS.begin(I2C_SLAVE_ADDRESS); // join i2c network
  TinyWireS.onReceive(receiveEvent);

  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);
  pinMode(PUMP_3_PIN, OUTPUT);

}

void loop() {

  TinyWireS_stop_check();
}
