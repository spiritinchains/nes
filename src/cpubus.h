
#ifndef CPUBUS_H
#define CPUBUS_H

#include <inttypes.h>

void bus_init();

uint8_t read8(uint16_t);
uint16_t read16(uint16_t);

void write8(uint16_t, uint8_t);

#endif
