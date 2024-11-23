#pragma once

#include <Arduino.h>
#include "./buffer.hpp"

#define HASHSIZE 4
#define DEFAULT_WRITE_SPEED 20

// A simple hash function
void generateHash(const byte *data, int size, byte *hash)
{
    // Basic hash calculation (e.g., sum of bytes modulo 256 for simplicity)
    uint32_t sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += data[i];
    }
    hash[0] = (sum >> 24) & 0xFF; // MSB
    hash[1] = (sum >> 16) & 0xFF;
    hash[2] = (sum >> 8) & 0xFF;
    hash[3] = sum & 0xFF; // LSB
}

struct PocketMetadata
{
    int size = 0;
    int id = rand();
    int writeSpeed = DEFAULT_WRITE_SPEED;
};

struct Pocket
{
    Array<byte> data;
    Array<byte> signal;
    PocketMetadata pocketMetadata;

    // Callback to decide whether to continue reading
    void (*continueCallback)(const Pocket &p) = nullptr;

    Pocket(Array<byte> _data, Array<byte> _signal, PocketMetadata _pocketMetadata)
        : data(_data), signal(_signal), pocketMetadata(_pocketMetadata) {}

    bool read(byte pin)
    {
        byte receivedSignal[signal.size];
        int signalIndex = 0;

        while (true)
        {
            // Call the continue callback if set
            if (continueCallback)
            {
                continueCallback(*this);
            }

            // Read signal data
            byte value = digitalRead(pin);

            if (signalIndex < signal.size)
            {
                receivedSignal[signalIndex++] = value;
            }

            // If signal is complete, validate it
            if (signalIndex == signal.size)
            {
                bool isSignalMatch = true;
                for (int i = 0; i < signal.size; i++)
                {
                    if (receivedSignal[i] != signal.data[i])
                    {
                        isSignalMatch = false;
                        break;
                    }
                }

                if (isSignalMatch)
                {
                    // Signal matches; proceed to read metadata and data
                    byte metadataBuffer[sizeof(PocketMetadata)];
                    for (int j = 0; j < sizeof(PocketMetadata); j++)
                    {
                        if (continueCallback)
                        {
                            continueCallback(*this);
                        }
                        metadataBuffer[j] = digitalRead(pin);
                        delay(pocketMetadata.writeSpeed);
                    }

                    // Validate metadata hash
                    byte metadataHash[HASHSIZE];
                    for (int j = 0; j < HASHSIZE; j++)
                    {
                        if (continueCallback)
                        {
                            continueCallback(*this);
                        }
                        metadataHash[j] = digitalRead(pin);
                        delay(pocketMetadata.writeSpeed);
                    }

                    byte computedHash[HASHSIZE];
                    generateHash(metadataBuffer, sizeof(PocketMetadata), computedHash);
                    for (int j = 0; j < HASHSIZE; j++)
                    {
                        if (metadataHash[j] != computedHash[j])
                        {
                            return false; // Metadata hash mismatch
                        }
                    }

                    // Parse metadata
                    memcpy(&pocketMetadata, metadataBuffer, sizeof(PocketMetadata));

                    // Read data
                    byte *dataBuffer = new byte[pocketMetadata.size];
                    for (int j = 0; j < pocketMetadata.size; j++)
                    {
                        if (continueCallback)
                        {
                            continueCallback(*this);
                        }
                        dataBuffer[j] = digitalRead(pin);
                        delay(pocketMetadata.writeSpeed);
                    }

                    // Validate data hash
                    byte dataHash[HASHSIZE];
                    for (int j = 0; j < HASHSIZE; j++)
                    {
                        if (continueCallback)
                        {
                            continueCallback(*this);
                        }
                        dataHash[j] = digitalRead(pin);
                        delay(pocketMetadata.writeSpeed);
                    }

                    generateHash(dataBuffer, pocketMetadata.size, computedHash);
                    for (int j = 0; j < HASHSIZE; j++)
                    {
                        if (dataHash[j] != computedHash[j])
                        {
                            delete[] dataBuffer;
                            return false; // Data hash mismatch
                        }
                    }

                    // Assign data
                    data = Array<byte>(pocketMetadata.size, dataBuffer);

                    return true; // Successfully read and validated
                }
                else
                {
                    signalIndex = 0; // Reset signal index to try again
                }
            }

            delay(pocketMetadata.writeSpeed);
        }

        return false; // Should never reach this point
    }

    // In pocket.hpp
    void send(byte pin) const
    {
        // Send the signal first
        for (int i = 0; i < signal.size; i++)
        {
            digitalWrite(pin, signal.data[i]);
            delay(pocketMetadata.writeSpeed);
        }

        // Serialize metadata and send it
        byte metadataBuffer[sizeof(PocketMetadata)];
        memcpy(metadataBuffer, &pocketMetadata, sizeof(PocketMetadata));
        for (int i = 0; i < sizeof(PocketMetadata); i++)
        {
            digitalWrite(pin, metadataBuffer[i]);
            delay(pocketMetadata.writeSpeed);
        }

        // Send metadata hash
        byte metadataHash[HASHSIZE];
        generateHash(metadataBuffer, sizeof(PocketMetadata), metadataHash);
        for (int i = 0; i < HASHSIZE; i++)
        {
            digitalWrite(pin, metadataHash[i]);
            delay(pocketMetadata.writeSpeed);
        }

        // Send the data
        for (int i = 0; i < data.size; i++)
        {
            digitalWrite(pin, data.data[i]);
            delay(pocketMetadata.writeSpeed);
        }

        // Send data hash
        byte dataHash[HASHSIZE];
        generateHash(data.data, data.size, dataHash);
        for (int i = 0; i < HASHSIZE; i++)
        {
            digitalWrite(pin, dataHash[i]);
            delay(pocketMetadata.writeSpeed);
        }
    }
};
