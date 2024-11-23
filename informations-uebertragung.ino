#include <Arduino.h>
#include "./array.hpp"
#include "./pocket.hpp"
#include "./node.hpp"

// Corrected signal data initialization
byte signaldata[] = {255, 0, 255, 123};

// Updated Array initialization
Array<byte> signal(sizeof(signaldata), signaldata);

NodeMetaData nmp = {
    13, // pin
    signal,
};

Node node(nmp);

void OnPocket(const Pocket &p)
{
  Serial.println("Pocket Received");
}

void setup()
{
  Serial.begin(9600); // Initialize Serial communication
  node.onPocket = OnPocket;
}

void loop()
{
  node.update();
}