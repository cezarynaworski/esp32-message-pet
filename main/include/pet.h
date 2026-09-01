#ifndef PET_H
#define PET_H

#include "stdint.h"

typedef struct {
  uint8_t happiness; //0-255  255=happy, 0=sad
  uint8_t hungry_status; //0-256  255=full, 0 =hungry
} CatState;

#endif