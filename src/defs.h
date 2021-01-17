#ifndef DEFS_H_
#define DEFS_H_
#include <stdint.h>

static const uint32_t GB_TARGET_FPS = 60;
static const uint32_t GB_TARGET_FRAMETIME_MS = 1000/GB_TARGET_FPS;
#define GB_SCREEN_WIDTH 640
#define GB_SCREEN_HEIGHT 480

#define GB_ADDRESS_SPACE_SIZE 65536
static const uint32_t GB_CYCLES_PER_SEC = 4194304;
static const uint32_t GB_SIM_TARGET_FPS = 60;
static const uint32_t GB_CYCLES_PER_FRAME = GB_CYCLES_PER_SEC/GB_SIM_TARGET_FPS;

#endif
