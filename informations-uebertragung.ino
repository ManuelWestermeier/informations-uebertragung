#include <Arduino.h>
#include "./node.hpp"

#define IS_SENDING true

void handlePocket(const Pocket &p) { Serial.println("Pocket received!"); }
bool continueReading() { return true; }

void setup()
{
    Serial.begin(9600);

    Node node({
        .pin = 13,
        .signal = Array<byte>(4, new byte[4]{255, 0, 255, 123}),
    });

    node.onPocket = handlePocket;
    node.continueCallback = continueReading;

#if IS_SENDING
    node.send(Pocket(
        Array<byte>(3, new byte[3]{42, 43, 44}),
        node.meta.signal,
        PocketMetadata()));
#else
    node.update();
#endif
}

void loop() {}