#pragma once

#include <Arduino.h>

struct Buffer
{
    int size = 0;
    byte *data = nullptr; // Ensure data is initialized to nullptr
};