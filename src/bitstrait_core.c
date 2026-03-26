#include "../include/bitstrait_core.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

uint8_t BitStrait_ComputeCRC(const uint8_t *data, uint8_t length) {
  assert(data);

  uint8_t crc = 0x00;

  for (uint8_t i = 0; i < length; i++) {
    crc ^= data[i];

    for (uint8_t j = 0; j < 8; j++) {
      crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ CRC8_POLY)
                         : (uint8_t)(crc << 1);
    }
  }
  return crc;
}

void BitStrait_Pack(const BitStrait_Telemetry_t *in, uint8_t *out) {
  out[0] = in->battery;
  out[1] = (uint8_t)(in->altitude & 0xFF);
  out[2] = (uint8_t)((in->altitude >> 8) & 0x03);
  out[3] = in->status & 0x0F;
  out[4] = BitStrait_ComputeCRC(out, PAYLOAD_SIZE);
}

bool BitStrait_Unpack(const uint8_t *in, BitStrait_Telemetry_t *out) {
  if (BitStrait_ComputeCRC(in, PAYLOAD_SIZE) != in[4])
    return false;

  out->battery = in[0];
  out->altitude = (uint16_t)in[1] | ((uint16_t)(in[2] & 0x03) << 8);
  out->status = in[3] & 0x0F;
  return true;
}

size_t BitStrait_COBS_Encode(const uint8_t *in, size_t in_length,
                             uint8_t *out) {
  size_t write_index = 1;
  size_t code_index = 0;
  uint8_t code = 1;

  for (size_t i = 0; i < in_length; i++) {
    if (in[i] == 0) {
      out[code_index] = code;
      code_index = write_index++;
      code = 1;
    } else {
      out[write_index++] = in[i];
      code++;
      if (code == 0xFF) {
        code_index = write_index++;
        code = 1;
      }
    }
  }
  out[code_index] = code;
  out[write_index] = 0x00; // Delimiter
  return write_index + 1;
}

size_t BitStrait_COBS_Decode(const uint8_t *in, size_t in_length,
                             uint8_t *out) {
  size_t read_index = 0;
  size_t write_index = 0;
  uint8_t code;

  while (read_index < in_length) {
    code = in[read_index++];
    for (uint8_t i = 1; i < code; i++) {
      out[write_index++] = in[read_index++];
    }
    if (code < 0xFF && read_index < in_length) {
      out[write_index++] = 0;
    };
  }
  return write_index;
}
