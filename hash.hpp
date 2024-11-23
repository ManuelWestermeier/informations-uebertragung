#pragma once

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