#include <Arduino.h>
#include "./node.hpp"

// Callback to handle received pockets
void handlePocket(const Pocket &p)
{
    Serial.println("Pocket received!");
}

// Callback for custom behavior during the reading process
void continueReading(const Pocket &p)
{
    Serial.println("Reading in progress...");
    delay(10); // Simulated work
}

void setup()
{
    Serial.begin(9600);

    // Define the signal array
    byte signalData[] = {255, 0, 255, 123};
    Array<byte> signal(sizeof(signalData), signalData);

    // Define data to send
    byte dataToSend[] = {42, 43, 44};
    Array<byte> data(sizeof(dataToSend), dataToSend);

    // Initialize Node metadata
    NodeMetaData nodeMeta = {
        .pin = 13,
        .signal = signal};

    // Create a Node instance
    Node node(nodeMeta);

    // Set callbacks
    node.onPocket = handlePocket;
    node.continueCallback = continueReading;

    // Simulate sending a pocket
    Pocket pocketToSend(data, signal, PocketMetadata());
    node.send(pocketToSend);
}

void loop()
{
    // Simulate reading updates in the loop
    // Node node.update() should be called to check for incoming pockets
}