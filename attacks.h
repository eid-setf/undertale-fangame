#ifndef ATTACKS_H_
#define ATTACKS_H_

#include <stddef.h>
#include <raylib.h>
#include "globals.h"

#define GBLASTER_COOLDOWN 30

typedef struct gBlaster_beam {
    union {
        Rectangle;
        Rectangle rec;
    };
    bool active;
    Color color;
} gBlaster_beam;

typedef struct gBlaster {
    union {
        Entity;
        Entity entity;
    };
    bool active;
    int cooldown;
    gBlaster_beam beam;
} gBlaster;

extern gBlaster gaster_blaster;
extern int bone_counter;
extern int bone_cooldown;

void bone_spawn();
void bone_move();

void gblaster_init();
void gblaster_spawn();
void gblaster_move();
void gblaster_animate();
int gblaster_rotate();
void beam_spawn();

#endif // ATTACKS_H_
