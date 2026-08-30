#include "animation/cat_animation.h"
#include "animation/animation.h"
#include "sprites/eye.h"
#include "sprites/flush.h"
#include "sprites/mouth.h"
#include "sprites/sprite.h"
#include "sprites/whiskers.h"

// Default / blinking
static const Sprite *cat_default_layers[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye,
    [CAT_LAYER_RIGHT_EYE] = &eye,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_blinking_layers[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_blinking,
    [CAT_LAYER_RIGHT_EYE] = &eye_blinking,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eyes_closed_layers[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

// Look around
static const Sprite *cat_look_around_layer_1[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_upper,
    [CAT_LAYER_RIGHT_EYE] = &eye_upper,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_look_around_layer_2[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_upper_right,
    [CAT_LAYER_RIGHT_EYE] = &eye_upper_right,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_look_around_layer_3[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_lower_right,
    [CAT_LAYER_RIGHT_EYE] = &eye_lower_right,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_look_around_layer_4[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_lower,
    [CAT_LAYER_RIGHT_EYE] = &eye_lower,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_look_around_layer_5[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_lower_left,
    [CAT_LAYER_RIGHT_EYE] = &eye_lower_left,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_look_around_layer_6[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_upper_left,
    [CAT_LAYER_RIGHT_EYE] = &eye_upper_left,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_look_around_layer_7[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_squint_right,
    [CAT_LAYER_RIGHT_EYE] = &eye_squint_left,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_look_around_layer_8[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_yawn_left,
    [CAT_LAYER_RIGHT_EYE] = &eye_yawn_right,
    [CAT_LAYER_MOUTH] = &mouth_tongue_basic,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const LayerOffset cat_look_around_offset_1[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.y = -3},
    [CAT_LAYER_RIGHT_EYE] = {.y = -3},
};
static const LayerOffset cat_look_around_offset_2[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.x = 3, .y = -3},
    [CAT_LAYER_RIGHT_EYE] = {.x = 3, .y = -3},
};
static const LayerOffset cat_look_around_offset_3[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.x = 3, .y = 3},
    [CAT_LAYER_RIGHT_EYE] = {.x = 3, .y = 3},
};
static const LayerOffset cat_look_around_offset_4[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.y = 3},
    [CAT_LAYER_RIGHT_EYE] = {.y = 3},
};
static const LayerOffset cat_look_around_offset_5[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.x = -3, .y = 3},
    [CAT_LAYER_RIGHT_EYE] = {.x = -3, .y = 3},
};
static const LayerOffset cat_look_around_offset_6[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.x = -3, .y = -3},
    [CAT_LAYER_RIGHT_EYE] = {.x = -3, .y = -3},
};
static const LayerOffset cat_look_around_offset_7[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.x = 3},
    [CAT_LAYER_RIGHT_EYE] = {.x = -3},
};

// Eyes Left-Right
static const Sprite *cat_eyes_left_layers[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_left,
    [CAT_LAYER_RIGHT_EYE] = &eye_left,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eyes_right_layers[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_right,
    [CAT_LAYER_RIGHT_EYE] = &eye_right,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const LayerOffset cat_eyes_left_offset[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.x = -3},
    [CAT_LAYER_RIGHT_EYE] = {.x = -3},
};
static const LayerOffset cat_eyes_right_offset[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.x = 3},
    [CAT_LAYER_RIGHT_EYE] = {.x = 3},
};

// New Message

static const Sprite *cat_new_message_layers_1[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye,
    [CAT_LAYER_RIGHT_EYE] = &eye,
    [CAT_LAYER_MOUTH] = &mouth_notice,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_new_message_layers_2[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye,
    [CAT_LAYER_RIGHT_EYE] = &eye,
    [CAT_LAYER_MOUTH] = &mouth_surprised,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_new_message_layers_3[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye,
    [CAT_LAYER_RIGHT_EYE] = &eye,
    [CAT_LAYER_MOUTH] = &mouth_talking,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_new_message_layers_4[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_star,
    [CAT_LAYER_RIGHT_EYE] = &eye_star,
    [CAT_LAYER_MOUTH] = &mouth_excited_1,
    [CAT_LAYER_LEFT_WHISKER] = &whisker_shake,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake};
static const Sprite *cat_new_message_layers_5[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_star,
    [CAT_LAYER_RIGHT_EYE] = &eye_star,
    [CAT_LAYER_MOUTH] = &mouth_excited_2,
    [CAT_LAYER_LEFT_WHISKER] = &whisker_shake_2,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake_2};
static const Sprite *cat_new_message_layers_6[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_blinking,
    [CAT_LAYER_RIGHT_EYE] = &eye_blinking,
    [CAT_LAYER_MOUTH] = &mouth_excited_2,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_new_message_layers_7[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed_happy,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed_happy,
    [CAT_LAYER_MOUTH] = &mouth,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker,
    [CAT_LAYER_LEFT_FLUSH] = &flush,
    [CAT_LAYER_RIGHT_FLUSH] = &flush};

static const LayerOffset new_message_text_offset = {.x = 3, .y = 20};

// Cat Eating

static const Sprite *cat_eating_layers_1[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye,
    [CAT_LAYER_RIGHT_EYE] = &eye,
    [CAT_LAYER_MOUTH] = &mouth_tongue_2,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eating_layers_2[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye,
    [CAT_LAYER_RIGHT_EYE] = &eye,
    [CAT_LAYER_MOUTH] = &mouth_notice,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eating_layers_3[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_star,
    [CAT_LAYER_RIGHT_EYE] = &eye_star,
    [CAT_LAYER_MOUTH] = &mouth_excited_1,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eating_layers_4[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_star,
    [CAT_LAYER_RIGHT_EYE] = &eye_star,
    [CAT_LAYER_MOUTH] = &mouth_excited_2,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eating_layers_5[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_eating_1,
    [CAT_LAYER_RIGHT_EYE] = &eye_eating_1,
    [CAT_LAYER_MOUTH] = &mouth_eating_1,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eating_layers_6[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_eating_2,
    [CAT_LAYER_RIGHT_EYE] = &eye_eating_2,
    [CAT_LAYER_MOUTH] = &mouth_eating_2,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eating_layers_7[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed_happy,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed_happy,
    [CAT_LAYER_MOUTH] = &mouth_eating_3,
    [CAT_LAYER_LEFT_WHISKER] = &whisker_shake_2,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake_2};

static const Sprite *cat_eating_layers_8[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed_happy,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed_happy,
    [CAT_LAYER_MOUTH] = &mouth_eating_4,
    [CAT_LAYER_LEFT_WHISKER] = &whisker_shake,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake};

static const Sprite *cat_eating_layers_9[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed_happy,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed_happy,
    [CAT_LAYER_MOUTH] = &mouth_eating_5,
    [CAT_LAYER_LEFT_WHISKER] = &whisker_shake,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake};

static const Sprite *cat_eating_layers_10[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed_happy,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed_happy,
    [CAT_LAYER_MOUTH] = &mouth_tongue_1,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_eating_layers_11[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed_happy,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed_happy,
    [CAT_LAYER_MOUTH] = &mouth_tongue_basic,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

// cat petting
static const Sprite *cat_petting_layers_1[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed,
    [CAT_LAYER_MOUTH] = &mouth_eating_5,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_petting_layers_2[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed,
    [CAT_LAYER_MOUTH] = &mouth_talking,
    [CAT_LAYER_LEFT_WHISKER] = &whisker,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker};

static const Sprite *cat_petting_layers_3[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed,   [CAT_LAYER_RIGHT_EYE] = &eye_closed,
    [CAT_LAYER_MOUTH] = &mouth_excited_1, [CAT_LAYER_LEFT_WHISKER] = &whisker_shake_2,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake_2, [CAT_LAYER_LEFT_FLUSH] = &flush,
    [CAT_LAYER_RIGHT_FLUSH] = &flush};

static const Sprite *cat_petting_layers_4[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed,   [CAT_LAYER_RIGHT_EYE] = &eye_closed,
    [CAT_LAYER_MOUTH] = &mouth_excited_2,           [CAT_LAYER_LEFT_WHISKER] = &whisker_shake,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake, [CAT_LAYER_LEFT_FLUSH] = &flush,
    [CAT_LAYER_RIGHT_FLUSH] = &flush};

static const Sprite *cat_petting_layers_5[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_heart,   [CAT_LAYER_RIGHT_EYE] = &eye_heart,
    [CAT_LAYER_MOUTH] = &mouth_excited_2,    [CAT_LAYER_LEFT_WHISKER] = &whisker_shake,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake, [CAT_LAYER_LEFT_FLUSH] = &flush,
    [CAT_LAYER_RIGHT_FLUSH] = &flush};
static const Sprite *cat_petting_layers_6[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_heart,
    [CAT_LAYER_RIGHT_EYE] = &eye_heart,
    [CAT_LAYER_MOUTH] = &mouth_excited_2,
    [CAT_LAYER_LEFT_WHISKER] = &whisker_shake,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake,
    [CAT_LAYER_LEFT_FLUSH] = &flush,
    [CAT_LAYER_RIGHT_FLUSH] = &flush};

static const Sprite *cat_petting_layers_7[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = &eye_closed,
    [CAT_LAYER_RIGHT_EYE] = &eye_closed,
    [CAT_LAYER_MOUTH] = &mouth_kiss,
    [CAT_LAYER_LEFT_WHISKER] = &whisker_shake,
    [CAT_LAYER_RIGHT_WHISKER] = &whisker_shake,
    [CAT_LAYER_LEFT_FLUSH] = &flush,
    [CAT_LAYER_RIGHT_FLUSH] = &flush};

// Default layers positions
static const LayerInfo cat_default_layer_info[CAT_LAYER_COUNT] = {
    [CAT_LAYER_LEFT_EYE] = {.x = 20, .y = 25, .width = 40, .height = 45},

    [CAT_LAYER_RIGHT_EYE] = {.x = 100, .y = 25, .width = 40, .height = 45},

    [CAT_LAYER_MOUTH] = {.x = 45, .y = 55, .width = 70, .height = 60},
    [CAT_LAYER_LEFT_WHISKER] = {.x = 3, .y = 70, .width = 40, .height = 40},
    [CAT_LAYER_RIGHT_WHISKER] =
        {.x = 117, .y = 70, .width = 40, .height = 40, .mirror = true},
    [CAT_LAYER_LEFT_FLUSH] = {.x = 32, .y = 80, .height = 25, .width = 15},
    [CAT_LAYER_RIGHT_FLUSH] = {.x = 100, .y = 80, .height = 25, .width = 15},
    [CAT_LAYER_BUBBLE] = {.x = 0, .y = 0, .height = 0, .width = 0}};

Animation cat_blinking_animation = {
    .frames =
        (const AnimationFrame[]){
            {.layers = cat_default_layers, .duration_ms = 4000},

            {.layers = cat_blinking_layers, .duration_ms = 100},

            {.layers = cat_eyes_closed_layers, .duration_ms = 100},

            {.layers = cat_blinking_layers, .duration_ms = 100},

            {.layers = cat_default_layers, .duration_ms = 500},

            {.layers = cat_blinking_layers, .duration_ms = 100},

            {.layers = cat_eyes_closed_layers, .duration_ms = 100},

            {.layers = cat_blinking_layers, .duration_ms = 100},
        },
    .layer_info = cat_default_layer_info,

    .layer_count = CAT_LAYER_COUNT,
    .frame_count = 8,
    .loop = true,
};

Animation cat_look_around_animation = {
    .frames =
        (const AnimationFrame[]){

            {.layers = cat_look_around_layer_1,
             .duration_ms = 100,
             .layer_offset = cat_look_around_offset_1},

            {.layers = cat_look_around_layer_2,
             .duration_ms = 100,
             .layer_offset = cat_look_around_offset_2},

            {.layers = cat_look_around_layer_3,
             .duration_ms = 100,
             .layer_offset = cat_look_around_offset_3},

            {.layers = cat_look_around_layer_4,
             .duration_ms = 100,
             .layer_offset = cat_look_around_offset_4},

            {.layers = cat_look_around_layer_5,
             .duration_ms = 100,
             .layer_offset = cat_look_around_offset_5},

            {.layers = cat_look_around_layer_6,
             .duration_ms = 100,
             .layer_offset = cat_look_around_offset_6},

            {.layers = cat_look_around_layer_7,
             .duration_ms = 1000,
             .layer_offset = cat_look_around_offset_7},

            {.layers = cat_look_around_layer_8, .duration_ms = 1000},

        },
    .layer_info = cat_default_layer_info,

    .layer_count = CAT_LAYER_COUNT,
    .frame_count = 8,
    .loop = false,
};

Animation cat_look_left_right_animation = {
    .frames =
        (const AnimationFrame[]){
            {.layers = cat_default_layers, .duration_ms = 1000},

            {.layers = cat_eyes_left_layers,
             .duration_ms = 1000,
             .layer_offset = cat_eyes_left_offset},

            {.layers = cat_default_layers, .duration_ms = 100},

            {.layers = cat_eyes_right_layers,
             .duration_ms = 1000,
             .layer_offset = cat_eyes_right_offset},

            {.layers = cat_default_layers, .duration_ms = 500},

        },
    .layer_info = cat_default_layer_info,

    .layer_count = CAT_LAYER_COUNT,
    .frame_count = 5,
    .loop = true,
};

Animation cat_eating_animation = {
    .frames =
        (const AnimationFrame[]){

            {.layers = cat_eating_layers_1, .duration_ms = 400},

            {.layers = cat_eating_layers_2, .duration_ms = 500},

            {.layers = cat_eating_layers_3, .duration_ms = 150},
            {.layers = cat_eating_layers_4, .duration_ms = 700},
            {.layers = cat_eating_layers_5, .duration_ms = 300},
            {.layers = cat_eating_layers_6, .duration_ms = 300},
            {.layers = cat_eating_layers_7, .duration_ms = 300},
            {.layers = cat_eating_layers_8, .duration_ms = 400},

            {.layers = cat_eating_layers_9, .duration_ms = 400},

            {.layers = cat_eating_layers_10, .duration_ms = 400},
            {.layers = cat_eating_layers_11, .duration_ms = 1000},

        },
    .layer_info = cat_default_layer_info,

    .layer_count = CAT_LAYER_COUNT,
    .frame_count = 11,
    .loop = false,
};

// cat sleeping animation

Animation cat_new_message_animation = {
    .frames =
        (const AnimationFrame[]){

            {.layers = cat_new_message_layers_1, .duration_ms = 150},

            {.layers = cat_new_message_layers_2, .duration_ms = 150},

            {.layers = cat_new_message_layers_3, .duration_ms = 150},
            {.layers = cat_new_message_layers_4, .duration_ms = 200},
            {.layers = cat_new_message_layers_5, .duration_ms = 500},
            {.layers = cat_new_message_layers_6, .duration_ms = 100},
            {.layers = cat_new_message_layers_7,
             .duration_ms = 2000,
             .text = "NEW MESSAGE!",
             .text_offset = new_message_text_offset},

        },
    .layer_info = cat_default_layer_info,

    .layer_count = CAT_LAYER_COUNT,
    .frame_count = 7,
    .loop = false,

};

Animation cat_petting_animation = {
    .frames =
        (const AnimationFrame[]){

            {.layers = cat_petting_layers_1, .duration_ms = 200},

            {.layers = cat_petting_layers_2, .duration_ms = 200},

            {.layers = cat_petting_layers_3, .duration_ms = 200},
            {.layers = cat_petting_layers_4, .duration_ms = 600},
            {.layers = cat_petting_layers_5, .duration_ms = 400},
            {.layers = cat_petting_layers_6, .duration_ms = 400},
            {.layers = cat_petting_layers_7, .duration_ms = 1000},

        },
    .layer_info = cat_default_layer_info,

    .layer_count = CAT_LAYER_COUNT,
    .frame_count = 7,
    .loop = false,

};

// cat sad animation (hungry)

// cat yawning animation
