#include <Arduino.h>
#include "./node.hpp"

#define IS_SENDING true

void handlePocket(const Pocket &p) {
  Serial.println("Pocket received!");
  Serial.print("Data: ");
  for (int i = 0; i < p.data.size; i++) {
    Serial.print(p.data.data[i]);
  }
  Serial.println();
}

bool continueReading() {
  return true;
}

void setup() {
  Serial.begin(9600);

  Node node({
    .pin = 13,
    .signal = Array<byte>(4, new byte[4]{ 255, 0, 255, 123 }),
  });

  node.onPocket = handlePocket;
  node.continueCallback = continueReading;

  // data to send
  Buffer buffer;
  buffer.size = 3;
  buffer.data = "HEY";

#if IS_SENDING
  delay(500);
  node.send(buffer);
#else
  node.update();
#endif
}

void loop() {}