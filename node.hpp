#pragma once

#include <Arduino.h>
#include "./array.hpp"
#include "./pocket.hpp"

struct NodeMetaData
{
    uint8_t pin;
    Array<byte> signal;
};

struct Node
{
    NodeMetaData meta;

    void (*onPocket)(const Pocket &p) = nullptr;
    bool (*continueCallback)() = nullptr;

    Node(NodeMetaData meta) : meta(meta)
    {
        pinMode(meta.pin, INPUT);
    }

    void update()
    {
        byte placeholderData[1] = {0}; // Create a placeholder byte array
        
        Pocket pocket{
            Array<byte>(1, placeholderData), // Initialize data array
            meta.signal,
            PocketMetadata(),
        };

        // Assign the continue callback
        pocket.continueCallback = continueCallback;

        // Attempt to read a Pocket from the input pin
        if (pocket.read(meta.pin))
        {
            // If successfully read, trigger the onPocket handler if set
            if (onPocket)
            {
                onPocket(pocket);
            }
        }
    }

    bool send(const Pocket &pocket)
    {
        // Pin must be set to OUTPUT for sending
        pinMode(meta.pin, OUTPUT);

        // Use the Pocket's send method
        pocket.send(meta.pin);

        // Restore pin to INPUT mode after sending
        pinMode(meta.pin, INPUT);

        return true; // Indicate successful send
    }
};