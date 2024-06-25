#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <raylib.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define SQUARE_SIZE 24
#define SCALE 3
#define PLAYER_SPEED ((float)SQUARE_SIZE / 8)

#define MAX_TUBES 8
#define MAX_BONES 15
#define BONE_COOLDOWN 8

#define FRISK_SPEEDX (PLAYER_SPEED * 1.2)
#define FRISK_SPEEDY (PLAYER_SPEED / 2)

enum {
    DOWN,
    UP,
    LEFT,
    RIGHT
};

enum game_screen { TITLE, GAME, ENDING };

static const int game_width = 800;
static const int game_height = 600;
extern int screen_width;
extern int screen_height;

typedef struct Game {
    Rectangle upper_col;
    Rectangle tubes_col[MAX_TUBES];
    Texture2D background;
    RenderTexture2D rtexture;
    Rectangle src;
    Rectangle dest;
    Vector2 origin;
    Camera2D camera;
} Game;

typedef struct Entity {
    Texture2D texture;
    Rectangle frame_rec;
    int current_frame;
    int frame_speed;
    int frame_counter;
    int dir;
    Vector2 pos;
    Vector2 speed;
} Entity;

typedef struct Bone {
    Texture2D texture;
    Vector2 pos;
    Vector2 speed;
    bool active;
} Bone;

typedef struct Player {
    union {
        struct Entity;
        Entity entity;
    };
    Bone bone[MAX_BONES];
} Player;

typedef struct Enemy {
    union {
        struct Entity;
        Entity entity;
    };
    int hp;
    bool active;
} Enemy;

extern enum game_screen current_screen;
extern Game game;
extern Player sans;
extern Enemy frisk;

#endif // GLOBALS_H_
