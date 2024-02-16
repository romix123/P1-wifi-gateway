#pragma once

#include <stdint.h>
#include <stdio.h>

// the CRC routine
uint32_t calculateCRC32(const uint8_t *data, size_t length);