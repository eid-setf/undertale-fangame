#include <raylib.h>

#include "globals.h"
#include "attacks.h"

gBlaster gaster_blaster = { 0 };

void gblaster_init()
{
    gaster_blaster.active = false;
    gaster_blaster.pos = (Vector2) { sans.pos.x + sans.pos.x / 2, sans.pos.y + sans.pos.y / 2 };
    gaster_blaster.cooldown = 0;

    gaster_blaster.frame_rec = (Rectangle) { 0, 0, gaster_blaster.texture.width / 6, gaster_blaster.texture.height };
    gaster_blaster.current_frame = 0;
    gaster_blaster.frame_speed = 4;
    gaster_blaster.frame_counter = 0;
    gaster_blaster.dir = LEFT;
    gaster_blaster.speed = (Vector2) {0, 0};

    gaster_blaster.beam.active = false;
    gaster_blaster.beam.width = game.background.width * SCALE;
    gaster_blaster.beam.height = SQUARE_SIZE * SCALE;
    gaster_blaster.beam.x = sans.pos.x;
    gaster_blaster.beam.y = sans.pos.y;
    gaster_blaster.beam.color = RAYWHITE;
}

void gblaster_spawn()
{
    if (gaster_blaster.cooldown > 0)
        return;

    gaster_blaster.active = true;
    gaster_blaster.pos = (Vector2) { sans.pos.x + sans.frame_rec.width * SCALE / 2, sans.pos.y + sans.frame_rec.height * SCALE / 2 };
    gaster_blaster.current_frame = 0;
    gaster_blaster.frame_rec.x = 0;
    gaster_blaster.dir = sans.dir;

    gaster_blaster.cooldown = GBLASTER_COOLDOWN;
}

void gblaster_move()
{
    if (!gaster_blaster.active)
        return;

    if (gaster_blaster.current_frame == 5) {
        beam_spawn();
    } else {
        gblaster_animate();
    }
}

/* TODO: fix animation */
void gblaster_animate()
{
    if (gaster_blaster.frame_counter >= (60 / gaster_blaster.frame_speed)) {
        gaster_blaster.frame_counter = 0;
        gaster_blaster.current_frame++;

        gaster_blaster.frame_rec.x = gaster_blaster.frame_rec.width * gaster_blaster.current_frame;
    }
}

int gblaster_rotate()
{
    int rot = 0;

    switch (gaster_blaster.dir) {
        case LEFT:
            rot = 90;
            break;
        case RIGHT:
            rot = -90;
            break;
        case UP:
            rot = 180;
            break;
        case DOWN:
            rot = 0;
    }

    return rot;
}

void beam_spawn()
{
    if (gaster_blaster.beam.color.a <= 20) {
        gaster_blaster.active = false;
        gaster_blaster.cooldown = 0;
        gaster_blaster.beam.width = game.background.width * SCALE;
        gaster_blaster.beam.height = SQUARE_SIZE * SCALE;
        gaster_blaster.beam.active = false;
        gaster_blaster.beam.color.a = 255;
        /* gblaster_init(); */
        return;
    }

    gaster_blaster.beam.active = true;
    gaster_blaster.beam.y = gaster_blaster.pos.y - gaster_blaster.beam.height / 2;

    if (gaster_blaster.dir == LEFT) {
        gaster_blaster.beam.x = gaster_blaster.pos.x - 15 * SCALE;
        int tmp = gaster_blaster.beam.x;
        gaster_blaster.beam.x -= gaster_blaster.beam.width;
        gaster_blaster.beam.width = tmp;
    } else if (gaster_blaster.dir == RIGHT ) {
        gaster_blaster.beam.x = gaster_blaster.pos.x + 15 * SCALE;
    } else if (gaster_blaster.dir == DOWN) {
        gaster_blaster.beam.x = gaster_blaster.pos.x - 11 * SCALE;
        gaster_blaster.beam.y = gaster_blaster.pos.y;
        gaster_blaster.beam.height = game.background.width * SCALE;
        gaster_blaster.beam.width = SQUARE_SIZE * SCALE;
    } else if (gaster_blaster.dir == UP) {
        gaster_blaster.beam.x = gaster_blaster.pos.x - 11 * SCALE;
        gaster_blaster.beam.y = gaster_blaster.pos.y - game.background.width * SCALE; /* TODO: Check why pos.y = pos.y + half(y)*/
        gaster_blaster.beam.height = game.background.width * SCALE;
        gaster_blaster.beam.width = SQUARE_SIZE * SCALE;
    }

    gaster_blaster.beam.color.a -= 5;
}

void bone_spawn()
{
    if (bone_cooldown != 0)
        return;

    sans.bone[bone_counter].active = true;
    sans.bone[bone_counter].pos = (Vector2){sans.pos.x + sans.frame_rec.width * 3 / 2 - sans.bone[0].texture.width / 2, sans.pos.y};
    sans.bone[bone_counter].speed = (Vector2){0, 0};

    switch (sans.dir)
    {
    case UP:
        sans.bone[bone_counter].speed.y = -10;
        break;
    case LEFT:
        sans.bone[bone_counter].speed.x = -10;
        break;
    case RIGHT:
        sans.bone[bone_counter].speed.x = 10;
        break;
    case DOWN:
    default:
        sans.bone[bone_counter].speed.y = 10;
        break;
    }

    bone_cooldown = BONE_COOLDOWN;
    bone_counter++;
}

void bone_move()
{
    for (size_t i = 0; i < MAX_BONES; i++) {
        if (sans.bone[i].active) {
                sans.bone[i].pos.x += sans.bone[i].speed.x;
                sans.bone[i].pos.y += sans.bone[i].speed.y;
        }

        if (sans.bone[i].pos.y < 0 || sans.bone[i].pos.y > game.background.height * SCALE ||
            sans.bone[i].pos.x < 0 || sans.bone[i].pos.x > game.background.width * SCALE) {
            sans.bone[i].active = false;
            sans.bone[i].pos = (Vector2){sans.pos.x + sans.frame_rec.width * 3 / 2 - sans.bone[i].texture.width / 2, sans.pos.y};
            sans.bone[i].speed = (Vector2){0, 0};
        }
    }
}
