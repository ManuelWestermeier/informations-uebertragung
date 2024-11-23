#pragma once

#include <Arduino.h>

#include "./buffer.hpp"

#define HASHSIZE 4

struct PocketMetadata
{
    int size = 0;
    int id = rand();
};

struct Pocket
{
    Array<byte> data;
    Array<byte> signal;
    PocketMetadata pocketMetadata;

    Pocket(Array<byte> _data, Array<byte> _signal, PocketMetadata _pocketMetadata) : data(_data),
                                                                                     signal(_signal),
                                                                                     pocketMetadata(_pocketMetadata)
    {
    }

    Buffer getPocketBuffer()
    {
        int packageSize = signal.size + sizeof(PocketMetadata) + HASHSIZE + data.size + HASHSIZE;
        byte *buff = new byte[packageSize];

        int i = 0;
        for (; i < signal.size; i++)
        {
            buff[i] = signal.data[i];
        }
        for (; i < signal.size + sizeof(PocketMetadata); i++)
        {
        }

        Buffer buffer;
        buffer.size = packageSize;
        buffer.data = buff;

        return buffer;
    }

    byte send(byte pin, int waitTime)
    {
        Buffer buffer = getPocketBuffer();
        for (int i = 0; i < buffer.size; i++)
        {
            digitalWrite(pin, buffer.data[i]);
            delay(waitTime);
        }
        delete[] buffer.data;
    }
};