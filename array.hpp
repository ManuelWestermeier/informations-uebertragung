#pragma once

#include <Arduino.h>

template <typename T = byte>
struct Array
{
    T *data = nullptr;
    int size = 0;

    // Default Constructor
    Array() = default;

    // In array.hpp
    Array(int _size, T *_data = nullptr) : size(_size)
    {
        if (_data)
        {
            data = new T[_size];
            memcpy(data, _data, _size * sizeof(T));
        }
        else
        {
            data = nullptr;
        }
    }

    // Destructor
    ~Array()
    {
        if (data)
        {
            delete[] data;
            data = nullptr;
        }
    }
};