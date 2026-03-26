#ifndef BITSTRAIT_HAL_H
#define BITSTRAIT_HAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RING_BUFFER_SIZE 32

typedef struct {
  uint8_t buffer[RING_BUFFER_SIZE];
  size_t head;
  size_t tail;
  size_t count;
} RingBuffer_t;

void HAL_RB_Init(RingBuffer_t *rb);
bool HAL_RB_Push(RingBuffer_t *rb, uint8_t byte);
bool HAL_RB_Pop(RingBuffer_t *rb, uint8_t *byte);

void HAL_Get_Mock_Sensor_Data(uint8_t *battery, uint16_t *altitude);
void HAL_Inject_Interference(uint8_t *buffer, uint8_t length);

#endif // !BITSTRAIT_HAL_H
