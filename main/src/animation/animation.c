#include "animation/animation.h"
#include <stdint.h>

const AnimationFrame *animation_get_current_frame(const Animation *anim) {
  return &anim->frames[anim->current_frame];
}

const AnimationFrame *animation_get_previous_frame(const Animation *anim) {

  uint8_t current_frame = anim->current_frame;
  if (current_frame == 0)
    return &anim->frames[(anim->frame_count) - 1];
  else
    return &anim->frames[anim->current_frame-1];
}


void animation_update(Animation *anim, uint32_t delta_ms) {
  anim->elapsed_ms += delta_ms;

  if (anim->elapsed_ms >= anim->frames[anim->current_frame].duration_ms) {
    anim->elapsed_ms = 0;
    anim->frame_flushed = false;

    anim->current_frame++;

    if (anim->current_frame >= anim->frame_count) {
      if (anim->loop) {
        anim->current_frame = 0;
      } else {
        anim->current_frame = anim->frame_count - 1;
        anim->finished=true;
      }
    }
  }
}
void animation_reset(Animation *anim) {
  anim->current_frame = 0;
  anim->elapsed_ms = 0;
  anim->frame_flushed = false;
  anim->finished=false;
}
