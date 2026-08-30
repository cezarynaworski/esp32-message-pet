#ifndef CAT_ANIMATION_H
#define CAT_ANIMATION_H

//#define CAT_LAYER_COUNT 8

#include "animation/animation.h"
typedef enum {
  CAT_LAYER_BUBBLE,
  CAT_LAYER_LEFT_EYE,
  CAT_LAYER_RIGHT_EYE,
  CAT_LAYER_LEFT_WHISKER,
  CAT_LAYER_RIGHT_WHISKER,
  CAT_LAYER_LEFT_FLUSH,
  CAT_LAYER_RIGHT_FLUSH,
  CAT_LAYER_MOUTH,
  CAT_LAYER_COUNT
} CatLayer;

extern Animation cat_blinking_animation;
extern Animation cat_look_around_animation;
extern Animation cat_look_left_right_animation;
extern Animation cat_new_message_animation;
extern Animation cat_eating_animation;
extern Animation cat_petting_animation;

#endif
