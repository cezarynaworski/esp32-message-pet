#ifndef PET_H
#define PET_H

#include "stdint.h"

#define HUNGER_DECAY_PER_MINUTE 0.05f
#define HAPPINESS_DECAY_PER_MINUTE 0.025f

typedef enum { PET_STATE_OK, PET_STATE_HUNGRY, PET_STATE_SAD } PetState;

typedef struct {
  float happiness;     // 0-2100 255=happy, 0=sad
  float hungry_status; // 0-100 255=full, 0 =hungry
} PetStatus;

void update_pet_status(PetStatus *pet_status, uint64_t sleep_duration_ms);

PetState calc_pet_state(PetStatus *pet_status);
#endif