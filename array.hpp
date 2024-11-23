#pragma once

#include <Arduino.h>

template <typename T = byte>
struct Array
{
    T *data = nullptr;
    int size = 0;

    // Constructor to initialize array
    Array(int _size, T *_data) : size(_size), data(_data)
    {
    }

    // Destructor
    ~Array()
    {
        delete[] data;
    }
};