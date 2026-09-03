#include "pet.h"
#include "macro.h"

void update_pet_status(PetStatus *pet_state, uint64_t sleep_duration_ms) {
  float minutes = sleep_duration_ms / 60000.0f;
  pet_state->hungry_status =
      MAX(0.0f, pet_state->hungry_status - minutes * HUNGER_DECAY_PER_MINUTE);
  pet_state->happiness =
      MAX(0.0f, pet_state->happiness - minutes * HAPPINESS_DECAY_PER_MINUTE);
}

PetState calc_pet_state(PetStatus *pet_status) {
  if (pet_status->hungry_status <= 10) {
    return PET_STATE_HUNGRY;
  }

  if (pet_status->happiness <= 10) {
    return PET_STATE_SAD;
  }

  return PET_STATE_OK;
}