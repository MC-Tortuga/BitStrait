#ifndef BITSTRAIT_CORE_H
#define BITSTRAIT_CORE_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define PAYLOAD_SIZE 4
#define FRAME_SIZE 5
#define CRC8_POLY 0x07

typedef struct {
  uint8_t battery;
  uint16_t altitude;
  uint8_t status;
} BitStrait_Telemetry_t;

uint8_t BitStrait_ComputeCRC(const uint8_t *data, uint8_t length);
void BitStrait_Pack(const BitStrait_Telemetry_t *in, uint8_t *out);
bool BitStrait_Unpack(const uint8_t *in, BitStrait_Telemetry_t *out);

// Consistent Overhead Byte Stuffing
size_t BitStrait_COBS_Encode(const uint8_t *in, size_t in_length, uint8_t *out);
size_t BitStrait_COBS_Decode(const uint8_t *in, size_t in_length, uint8_t *out);

#endif // !BITSTRAIT_CORE_H
