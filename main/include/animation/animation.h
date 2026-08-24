#ifndef ANIMATION_H
#define ANIMATION_H
#include "sprites/sprite.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  bool mirror;
} LayerInfo;

typedef struct {
  int16_t x;
  int16_t y;
} LayerOffset;

typedef struct {
  const Sprite *const *layers;
  uint32_t duration_ms;
  const LayerOffset* layer_offset;
  const char* text; //optional text
  const LayerOffset text_offset;
} AnimationFrame;

typedef struct {
  const AnimationFrame *frames;
  const LayerInfo *layer_info;
  uint8_t layer_count;
  uint8_t frame_count;
  uint8_t current_frame;
  uint32_t elapsed_ms;
  bool frame_flushed;
  bool loop;
  bool finished;
} Animation;

const AnimationFrame *animation_get_current_frame(const Animation *anim);
const AnimationFrame *animation_get_previous_frame(const Animation *anim);
void animation_update(Animation *anim, uint32_t delta_ms);
void animation_reset(Animation *anim);

#endif