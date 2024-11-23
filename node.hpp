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

    Node(NodeMetaData meta) : meta(meta)
    {
        pinMode(meta.pin, INPUT);
    }

    void update()
    {
    }
};
