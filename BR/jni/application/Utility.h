#ifndef UTIL_H
#define UTIL_H

#include <zenilib.h>

const float down_c = Zeni::Global::pi / 2;
const float left_c = Zeni::Global::pi;
const float right_c = 0;
const float up_c = Zeni::Global::pi * 3 / 2;

const float sidebar_c = 300;
extern float screen_x_c;
const float screen_y_c = 900;
const float frame_rate_c = 120;

const float player_size_c = 64+16;

const float start_push_c = 15.0f;
const float air_res_c = .01f;
const float gravity_c = .05f;


#endif
