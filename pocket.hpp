#pragma once

#include <Arduino.h>
#include "./buffer.hpp"
#include "./hash.hpp"

#define HASHSIZE 4
#define DEFAULT_WRITE_SPEED 20

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

    bool (*continueCallback)() = nullptr;

    Pocket(Array<byte> _data, Array<byte> _signal, PocketMetadata _pocketMetadata)
        : data(_data), signal(_signal), pocketMetadata(_pocketMetadata) {}

    bool read(byte pin)
    {
        byte receivedSignal[signal.size];
        int signalIndex = 0;

        while (continueCallback && continueCallback())
        {
            // Read signal data at default write speed
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
                        metadataBuffer[j] = digitalRead(pin);
                        delay(DEFAULT_WRITE_SPEED);
                    }

                    // Validate metadata hash at default write speed
                    byte metadataHash[HASHSIZE];
                    for (int j = 0; j < HASHSIZE; j++)
                    {
                        metadataHash[j] = digitalRead(pin);
                        delay(DEFAULT_WRITE_SPEED);
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

                    // Read data at pocket's write speed
                    byte *dataBuffer = new byte[pocketMetadata.size];
                    for (int j = 0; j < pocketMetadata.size; j++)
                    {
                        dataBuffer[j] = digitalRead(pin);
                        delay(pocketMetadata.writeSpeed);
                    }

                    // Validate data hash at default write speed
                    byte dataHash[HASHSIZE];
                    for (int j = 0; j < HASHSIZE; j++)
                    {
                        dataHash[j] = digitalRead(pin);
                        delay(DEFAULT_WRITE_SPEED);
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

            delay(DEFAULT_WRITE_SPEED);
        }

        return false; // Should never reach this point
    }

    void send(byte pin) const
    {
        // Send the signal at default write speed
        for (int i = 0; i < signal.size; i++)
        {
            digitalWrite(pin, signal.data[i]);
            delay(DEFAULT_WRITE_SPEED);
        }

        // Serialize metadata and send it at default write speed
        byte metadataBuffer[sizeof(PocketMetadata)];
        memcpy(metadataBuffer, &pocketMetadata, sizeof(PocketMetadata));
        for (int i = 0; i < sizeof(PocketMetadata); i++)
        {
            digitalWrite(pin, metadataBuffer[i]);
            delay(DEFAULT_WRITE_SPEED);
        }

        // Send metadata hash at default write speed
        byte metadataHash[HASHSIZE];
        generateHash(metadataBuffer, sizeof(PocketMetadata), metadataHash);
        for (int i = 0; i < HASHSIZE; i++)
        {
            digitalWrite(pin, metadataHash[i]);
            delay(DEFAULT_WRITE_SPEED);
        }

        // Send the data at pocket's write speed
        for (int i = 0; i < data.size; i++)
        {
            digitalWrite(pin, data.data[i]);
            delay(pocketMetadata.writeSpeed);
        }

        // Send data hash at default write speed
        byte dataHash[HASHSIZE];
        generateHash(data.data, data.size, dataHash);
        for (int i = 0; i < HASHSIZE; i++)
        {
            digitalWrite(pin, dataHash[i]);
            delay(DEFAULT_WRITE_SPEED);
        }
    }
};
