#include <stdio.h>
#include <raylib.h>

#include "globals.h"
#include "title_screen.h"
#include "attacks.h"

int screen_width = game_width;
int screen_height = game_height;

int bone_counter = 0;
int bone_cooldown = BONE_COOLDOWN;

static bool game_over = false;
static bool pause = false;
static int frame_counter = 0;

enum game_screen current_screen = TITLE;
Game game = { 0 };
Player sans = { 0 };
Enemy frisk = { 0 };

/* Function declerations */
void init();
void game_init();
void player_init();
void enemy_init();

void update_game();

void player_move();
void player_face();
void player_animate();
void ent_wall_col(Entity *entity, Vector2 size);

void camera_move();

void enemy_collision();
void enemy_move();
void enemy_animate();

void game_draw();
void game_close();

int main()
{
    init();
    while (!WindowShouldClose()) {
        switch (current_screen) {
            case TITLE:
                if (IsKeyPressed(KEY_ENTER))
                    current_screen = GAME;
                break;
            case GAME:
                update_game();
                break;
            case ENDING:
                if (IsKeyPressed(KEY_ENTER)) {
                    current_screen = GAME;
                    game_init();
                    player_init();
                    gblaster_init();
                    enemy_init();
                }
                break;
        }

        game_draw();
    }

    game_close();
    return 0;
}

void init()
{
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(game_width, game_height, "Undertale Fangame");
    SetTargetFPS(60);
    SetExitKey(0);

    int monitor = GetCurrentMonitor();
    screen_width = GetMonitorWidth(monitor);
    screen_height = GetMonitorHeight(monitor);
    SetWindowSize(screen_width, screen_height);

    game.rtexture = LoadRenderTexture(game_width, game_height);
    SetTextureFilter(game.rtexture.texture, TEXTURE_FILTER_POINT);

    game.background = LoadTexture("resources/last_corridor.png");
    sans.texture = LoadTexture("resources/sans.png");
    sans.bone[0].texture = LoadTexture("resources/bone.png");
    frisk.texture = LoadTexture("resources/frisk.png");
    gaster_blaster.texture = LoadTexture("resources/gaster_blaster.png");

    game_init();
    player_init();
    gblaster_init();
    enemy_init();
}

void game_init()
{
    game_over = false;
    pause = false;

    game.camera = (Camera2D){0};
    game.camera.target = (Vector2){sans.pos.x + 8, sans.pos.y + 16};
    game.camera.offset = (Vector2){game_width / 2.0f, game_height / 2.0f};
    game.camera.rotation = 0.0f;
    game.camera.zoom = 1.0f;

    float game_ratio = (float) screen_height / game_height;
    Vector2 dv = { game_width * game_ratio, game_height * game_ratio };
    game.src = (Rectangle) { 0.0f, 0.0f, game.rtexture.texture.width, -game.rtexture.texture.height };
    game.dest = (Rectangle) { (screen_width - dv.x) / 2, 0, dv.x, dv.y };

    bone_counter = 0;
    bone_cooldown = 15;

    for (size_t i = 0; i < MAX_BONES; i++) {
        sans.bone[i].active = false;
        sans.bone[i].pos = (Vector2){sans.pos.x + sans.frame_rec.width * SCALE / 2 - sans.bone[i].texture.width / 2, sans.pos.y};
        sans.bone[i].speed = (Vector2){0, 0};
    }

    game.upper_col.height = SQUARE_SIZE;
    game.upper_col.width = SQUARE_SIZE * (50 * SCALE);
    game.upper_col.y = 120 * SCALE - SQUARE_SIZE - 20;
    game.upper_col.x = 0;

    for (size_t i = 0; i < MAX_TUBES; i++) {
        game.tubes_col[i].height = 20 * SCALE;
        game.tubes_col[i].width = 60 * SCALE;
        game.tubes_col[i].y = 140 * SCALE - game.tubes_col[i].height - 50;

        if (i == 0) {
            game.tubes_col[i].x = 140 * SCALE;
        } else {
            game.tubes_col[i].x = game.tubes_col[i - 1].x + game.tubes_col[i - 1].width * 2;
        }
    }
}

void player_init()
{
    sans.frame_rec = (Rectangle){0.0f, 0.0f, sans.texture.width / 4, sans.texture.height / 4};
    sans.current_frame = 0;
    sans.frame_speed = 4;
    sans.frame_counter = 0;
    sans.pos = (Vector2){game.background.width * SCALE / 2 + 25 * SCALE, game.background.height * SCALE / 2 + 50 * SCALE };
    sans.dir = LEFT;
    sans.speed = (Vector2){0.0f, 0.0f};
}

void enemy_init()
{
    frisk.pos = (Vector2) { game.background.width * SCALE / 2 - 60 * SCALE, game.background.height * SCALE / 2 + 50 * SCALE };
    frisk.speed = (Vector2) { 0, 0 };
    frisk.frame_rec = (Rectangle) { 0, 0, frisk.texture.width / 2, frisk.texture.height / 2 };
    frisk.current_frame = 0;
    frisk.frame_speed = 4;
    frisk.frame_counter = 0;
    frisk.dir = RIGHT;
    frisk.hp = 96;
    frisk.active = true;
}

void update_game()
{
    frame_counter++;
    sans.frame_counter++;
    frisk.frame_counter++;
    gaster_blaster.frame_counter++;

    if (bone_cooldown > 0)
        bone_cooldown--;

    player_move();
    ent_wall_col(&sans.entity,
                 (Vector2) { (sans.texture.width - 24) / 4 * SCALE,
                 sans.texture.height / 4 * SCALE });
    ent_wall_col(&frisk.entity,
                 (Vector2) { frisk.frame_rec.width * SCALE, frisk.frame_rec.height * SCALE });
    camera_move();

    if (IsKeyDown(KEY_Z))
        bone_spawn();

    if (bone_counter >= MAX_BONES)
        bone_counter = 0;

    if (IsKeyDown(KEY_X))
        gblaster_spawn();

    gblaster_move();

    bone_move();
    enemy_collision();
    enemy_move();
}

void player_move()
{
    sans.speed.x = 0;
    sans.speed.y = 0;
    sans.frame_rec.x = 0;

    if (IsKeyDown(KEY_LEFT)) {
        sans.speed.x = -PLAYER_SPEED;
    }

    if (IsKeyDown(KEY_RIGHT)) {
        sans.speed.x = PLAYER_SPEED;
    }

    if (IsKeyDown(KEY_UP)) {
        sans.speed.y = -PLAYER_SPEED;
    }

    if (IsKeyDown(KEY_DOWN)) {
        sans.speed.y = PLAYER_SPEED;
    }

    if (IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_RIGHT))
        sans.speed.x = 0;
    else if (IsKeyDown(KEY_UP) && IsKeyDown(KEY_DOWN))
        sans.speed.y = 0;

    if (sans.speed.x > 0) {
        sans.dir = RIGHT;
    } else if (sans.speed.x < 0) {
        sans.dir = LEFT;
    } else {
        if (sans.speed.y < 0) {
            sans.dir = UP;
        } else if (sans.speed.y > 0) {
            sans.dir = DOWN;
        }
    }

    player_face(sans);

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
        player_animate(sans);
    } else {
        sans.frame_rec.x = 0;
        sans.current_frame = 0;
    }

    sans.pos.x += sans.speed.x;
    sans.pos.y += sans.speed.y;
}

void player_face()
{
    sans.frame_rec.y = sans.texture.height / 4 * sans.dir;

    if (sans.dir == UP || sans.dir == DOWN) {
        sans.frame_rec.x = sans.texture.width / 4 * sans.current_frame;
        sans.frame_rec.width = sans.texture.width / 4;
    }
    else if (sans.dir == LEFT || sans.dir == RIGHT)
    {
        sans.frame_rec.x = (sans.texture.width - 24) / 4 * sans.current_frame;
        sans.frame_rec.width = (sans.texture.width - 24) / 4;
    }
}

void player_animate()
{
    if (sans.frame_counter >= (60 / sans.frame_speed)) {
        if (sans.speed.x == 0 && sans.speed.y == 0) {
            sans.current_frame = 0;
            sans.frame_rec.x = 0;
            return;
        }

        sans.frame_counter = 0;
        sans.current_frame++;

        if (sans.current_frame > 3)
            sans.current_frame = 0;

        player_face(sans);
    }
}

void ent_wall_col(Entity *entity, Vector2 size)
{
    if (entity->pos.y < 0)
        entity->pos.y = 0;
    if (entity->pos.y + entity->frame_rec.height * SCALE > game.background.height * SCALE)
        entity->pos.y = game.background.height * SCALE - entity->frame_rec.height * SCALE;

    if (entity->pos.x < 0)
        entity->pos.x = 0;
    if (entity->pos.x + entity->frame_rec.width * SCALE > game.background.width * SCALE)
        entity->pos.x = game.background.width * SCALE - entity->frame_rec.width * SCALE;

    /* Upper Collision */
    if (entity->pos.y < game.upper_col.y) {
        entity->pos.y = game.upper_col.y;
    }

    /* Tube collision */
    for (size_t i = 0; i < MAX_TUBES; i++) {
        if (CheckCollisionRecs((Rectangle){ entity->pos.x, entity->pos.y,
                size.x, size.y }, game.tubes_col[i]))
        {
            int tmpx = entity->pos.x;
            int tmpy = entity->pos.y;

            Rectangle over = GetCollisionRec((Rectangle){entity->pos.x, entity->pos.y,
                                                         size.x, size.y },
                                             game.tubes_col[i]);

            if (entity->speed.x > 0 && entity->pos.x < over.x && over.width < over.height) {
                entity->pos.x = game.tubes_col[i].x - entity->frame_rec.width * SCALE;
                entity->pos.y = tmpy;
            }

            if (entity->speed.x < 0 && entity->pos.x == over.x
                && over.width < over.height) {
                entity->pos.x = game.tubes_col[i].x + game.tubes_col[i].width;
                entity->pos.y = tmpy;
            }

            if (entity->speed.y < 0 && entity->pos.y == over.y && over.width > over.height) {
                entity->pos.y = game.tubes_col[i].y + game.tubes_col[i].height;
                entity->pos.x = tmpx;
            }
        }
    }
}

void camera_move()
{
    game.camera.target = sans.pos;
    game.camera.offset = (Vector2){game_width / 2.0f, game_height / 2.0f};

    Vector2 max = GetWorldToScreen2D((Vector2){game.background.width * SCALE, game.background.height * SCALE}, game.camera);
    Vector2 min = GetWorldToScreen2D((Vector2){0, 0}, game.camera);

    if (max.x < game_width)
        game.camera.offset.x = game_width / 2 + (game_width - max.x);
    if (max.y < game_height)
        game.camera.offset.y = game_height / 2 + (game_height - max.y);
    if (min.x > 0)
        game.camera.offset.x = game_width / 2 - min.x;
    if (min.y > 0)
        game.camera.offset.y = game_height / 2 - min.y;
}

void enemy_collision()
{
    if (frisk.hp <= 0) {
        enemy_init(frisk);
    }

    if (frame_counter % 2)
        return;

    /* Bone collision */
    for (size_t i = 0; i < MAX_BONES; i++) {
        if (sans.bone[i].active) {
            if (CheckCollisionRecs((Rectangle) { frisk.pos.x, frisk.pos.y, frisk.frame_rec.width * SCALE, frisk.frame_rec.height * SCALE},
                                   (Rectangle) { sans.bone[i].pos.x, sans.bone[i].pos.y, sans.bone[0].texture.width, sans.bone[0].texture.height})) {
                frisk.hp--;
                frisk.pos.x += -frisk.speed.x * 1.6;
            }
        }
    }

    /* Gaster blaster collision */
    if (gaster_blaster.beam.active) {
        if (CheckCollisionRecs((Rectangle) { frisk.pos.x, frisk.pos.y, frisk.frame_rec.width * SCALE, frisk.frame_rec.height * SCALE},
                               (Rectangle) gaster_blaster.beam.rec)) {
            frisk.hp--;
            frisk.pos.x += -frisk.speed.x * 1.8;
            frisk.pos.y += -frisk.speed.y * 1.8;
        }
    }

    /* Kill player */
    if (CheckCollisionRecs((Rectangle) { frisk.pos.x, frisk.pos.y, frisk.frame_rec.width * SCALE, frisk.frame_rec.height * SCALE},
                           (Rectangle) { sans.pos.x, sans.pos.y, sans.frame_rec.width * SCALE, sans.frame_rec.height * SCALE})) {
        current_screen = ENDING;
    }
}

void enemy_move()
{
    if (sans.pos.x > frisk.pos.x) {
        frisk.speed.x = FRISK_SPEEDX;
        frisk.dir = RIGHT;
    } else if (sans.pos.x < frisk.pos.x) {
        frisk.speed.x = -FRISK_SPEEDX;
        frisk.dir = LEFT;
    }

    if (sans.pos.y < frisk.pos.y) {
        frisk.speed.y = -FRISK_SPEEDY;
    } else if (sans.pos.y > frisk.pos.y) {
        frisk.speed.y = FRISK_SPEEDY;
    } else {
        frisk.speed.y = 0;
    }

    frisk.pos.x += frisk.speed.x;
    frisk.pos.y += frisk.speed.y;

    enemy_animate(frisk);
}

void enemy_animate()
{
    if (frisk.frame_counter >= (60 / frisk.frame_speed)) {
        if (frisk.speed.x == 0 && frisk.speed.y == 0) {
            frisk.current_frame = 0;
            frisk.frame_rec.x = 0;
            return;
        }

        frisk.frame_counter = 0;
        frisk.current_frame++;

        if (frisk.current_frame > 1)
            frisk.current_frame = 0;

        frisk.frame_rec.x = frisk.frame_rec.width * frisk.current_frame;
        frisk.frame_rec.y = frisk.frame_rec.height * !(frisk.dir % 2);
    }
}

void game_draw()
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginTextureMode(game.rtexture);
    ClearBackground(RAYWHITE);

    switch (current_screen) {
        case TITLE:
            draw_title();
            break;
        case GAME:
            BeginMode2D(game.camera);

            DrawTextureEx(game.background, (Vector2){0, 0}, 0, SCALE, RAYWHITE);
            DrawTexturePro(sans.texture, sans.frame_rec,
                           (Rectangle){sans.pos.x, sans.pos.y,
                           sans.frame_rec.width * SCALE,
                           sans.frame_rec.height * SCALE},
                           (Vector2){0, 0}, 0, WHITE);

            for (size_t i = 0; i < MAX_BONES; i++) {
                if (sans.bone[i].active)
                    DrawTextureV(sans.bone[0].texture, sans.bone[i].pos, WHITE);
            }

            if (frisk.active) {
                DrawTexturePro(frisk.texture, frisk.frame_rec,
                               (Rectangle){frisk.pos.x, frisk.pos.y,
                               frisk.frame_rec.width * SCALE,
                               frisk.frame_rec.height * SCALE},
                               (Vector2){0, 0}, 0, WHITE);
            }

            if (gaster_blaster.beam.active) {
                DrawRectangleRec(gaster_blaster.beam.rec, gaster_blaster.beam.color);
            }

            /* TODO: make gaster_blaster ACTUALLY rotate not just the picture */
            if (gaster_blaster.active) {
                int rot = gblaster_rotate();
                DrawTexturePro(gaster_blaster.texture, gaster_blaster.frame_rec,
                               (Rectangle) { gaster_blaster.pos.x, gaster_blaster.pos.y,
                               gaster_blaster.frame_rec.width * SCALE,
                               gaster_blaster.frame_rec.height * SCALE},
                               (Vector2) { gaster_blaster.frame_rec.width * SCALE / 2,
                               gaster_blaster.frame_rec.height * SCALE / 2},
                               rot, WHITE);
            }

            EndMode2D();

            DrawText(TextFormat("Frisk's HP: %d", frisk.hp), 10, 10, 30, SKYBLUE);
            DrawText(TextFormat("Bones: %d", MAX_BONES - bone_counter),
                     screen_width - 10 - MeasureText(TextFormat("Bones: %d",
                                                                MAX_BONES - bone_counter), 30),
                     10, 30, SKYBLUE);
            break;
        case ENDING:
            DrawText("Game Over!", 10, 10, 20, GREEN);
            break;
    }

    EndTextureMode();

    DrawTexturePro(game.rtexture.texture, game.src, game.dest, game.origin,
                   0, WHITE);

    EndDrawing();
}

void game_close()
{
    UnloadTexture(sans.texture);
    UnloadTexture(sans.bone[0].texture);
    UnloadTexture(game.background);
    CloseWindow();
}
