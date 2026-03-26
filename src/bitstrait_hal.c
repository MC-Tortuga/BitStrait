#include "../include/bitstrait_hal.h"
#include <stdint.h>
#include <stdlib.h>

void HAL_RB_Init(RingBuffer_t *rb) {
  rb->head = 0;
  rb->tail = 0;
  rb->count = 0;
}

bool HAL_RB_Push(RingBuffer_t *rb, uint8_t byte) {
  if (rb->count >= RING_BUFFER_SIZE)
    return false;

  rb->buffer[rb->head] = byte;
  rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
  rb->count++;
  return true;
}

bool HAL_RB_Pop(RingBuffer_t *rb, uint8_t *byte) {
  if (rb == NULL || byte == NULL)
    return false;

  if (rb->count > RING_BUFFER_SIZE) {
    rb->count = 0;
    return false;
  }

  if (rb->count == 0)
    return false;

  if (rb->tail >= RING_BUFFER_SIZE) {
    rb->count = 0;
    rb->head = 0;
    rb->tail = 0;
    return false;
  }
  *byte = rb->buffer[rb->tail];
  rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
  rb->count--;
  return true;
}

void HAL_Get_Mock_Sensor_Data(uint8_t *battery, uint16_t *altitude) {
  static uint8_t b = 100;
  static uint16_t a = 200;

  if (b > 0)
    b--;
  a += (rand() % 3) - 1;
  *battery = b;
  *altitude = a;
}

void HAL_Inject_Interference(uint8_t *buffer, uint8_t length) {
  if (buffer == NULL || length <= 1)
    return;

  uint8_t target_range = length - 1;

  int byte_to_corrupt = rand() % target_range;

  int bit_to_flip = rand() % 8;

  buffer[byte_to_corrupt] ^= (1 << bit_to_flip);
}
