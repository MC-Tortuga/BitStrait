#ifndef BITSTRAIT_UI_H
#define BITSTRAIT_UI_H

#include "bitstrait_core.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void UI_Init(void);
void UI_Draw(const BitStrait_Telemetry_t *data, const uint8_t *legacy_raw,
             const uint8_t *cobs_encoded, size_t enc_length, bool valid,
             bool fault);
void UI_Cleanup(void);

#endif // !BITSTRAIT_UI_H
