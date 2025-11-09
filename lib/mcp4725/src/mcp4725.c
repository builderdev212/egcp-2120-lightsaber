#include "mcp4725.h"

void write_mcp4725(uint16_t data) {
    write_i2c(MCP4725_ADDR, (uint8_t[]){0x00 | ((data >> 8) & 0x0F), data & 0xFF}, 2);
}
