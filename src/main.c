#define _DEFAULT_SOURCE
#include "../include/bitstrait_core.h"
#include "../include/bitstrait_hal.h"
#include "../include/bitstrait_ui.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main() {
  srand(time(NULL));
  UI_Init();
  RingBuffer_t rx_ring;
  HAL_RB_Init(&rx_ring);

  BitStrait_Telemetry_t tx = {100, 200, 0x0F}; // Mock Sensor Source
  BitStrait_Telemetry_t rx = {0};              // Decoded Destination

  uint8_t legacy_buffer[FRAME_SIZE];   // V1 Raw Frame
  uint8_t cobs_wire[FRAME_SIZE + 2];   // V2 Consistent Overhead Byte Stuffing
  uint8_t parser_node[FRAME_SIZE + 2]; // Accumulator for Ring Buffer
  uint8_t scratch_pad[FRAME_SIZE + 2]; // Decoder Workspace

  size_t parser_idx = 0;
  size_t enc_length = 0;
  bool is_valid = true;
  bool fault_mode = false;

  while (1) {
    int ch = getch();
    if (ch == 'q')
      break;
    if (ch == 'f')
      fault_mode = !fault_mode;

    HAL_Get_Mock_Sensor_Data(&tx.battery, &tx.altitude);

    BitStrait_Pack(&tx, legacy_buffer);

    enc_length = BitStrait_COBS_Encode(legacy_buffer, FRAME_SIZE, cobs_wire);

    if (fault_mode && enc_length > 1)
      HAL_Inject_Interference(cobs_wire, enc_length);

    for (size_t i = 0; i < enc_length; i++) {
      HAL_RB_Push(&rx_ring, cobs_wire[i]);
    }

    uint8_t incoming_byte;
    while (HAL_RB_Pop(&rx_ring, &incoming_byte)) {
      if (parser_idx < sizeof(parser_node)) {
        parser_node[parser_idx++] = incoming_byte;
      } else {
        parser_idx = 0;
        continue;
      }

      if (incoming_byte == 0x00) {
        if (parser_idx > 1 && parser_idx <= (FRAME_SIZE + 2)) {
          BitStrait_COBS_Decode(parser_node, parser_idx - 1, scratch_pad);
          is_valid = BitStrait_Unpack(scratch_pad, &rx);
        } else {
          is_valid = false;
        }

        parser_idx = 0;
      }
    }

    UI_Draw(&rx, legacy_buffer, cobs_wire, enc_length, is_valid, fault_mode);
    usleep(100000);
  }
  UI_Cleanup();
  return 0;
}
