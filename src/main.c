#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/raylib.h"
#include "../include/raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TARGET_FRAMERATE 60
#define NEARBLACK CLITERAL(Color){5, 5, 5, 255}
#define MAX_BULLETS 420
#define MAX_ASTEROIDS 100

typedef struct {
    Vector2 pos;
    Vector2 vel;
    bool active;  // if bullet should be updated
    float speed;
    float rot;       // rotation so it's aligned with where we shoot
    float lifetime;  // how long the bullet is in game
    float maxlife;   // when it should be inactive
} Bullet;

// player movement
typedef struct {
    Vector2 pos;
    Vector2 vel;
    float speed;
    float friction;
    float rot;
    float rspeed;
    Bullet bullets[MAX_BULLETS];
} Player;

typedef enum {
    ASTEROID_SMALL = 1,
    ASTEROID_MEDIUM = 2,
    ASTEROID_LARGE = 4,
} AsteroidSize;

// asteroid
typedef struct {
    Vector2 pos;
    Vector2 vel;
    bool active;
    AsteroidSize size;
    float accel;
    float rot;
    float rspeed;
    float angle;  // angle from center of screen?
    float lifetime;
    float maxlife;
} Asteroid;

void DrawPlayer(Player *player) {
    DrawPoly(player->pos, 3, 20, player->rot, RAYWHITE);
}

void UpdatePlayer(Player *player, float dt) {
    float rad = player->rot * DEG2RAD;
    Vector2 forwards = {cos(rad), sin(rad)};
    if (IsKeyDown(KEY_W)) {  // up
        // forwards velocity taking into account rotation
        player->vel.x += forwards.x * player->speed * dt;
        player->vel.y += forwards.y * player->speed * dt;
    }
    if (IsKeyDown(KEY_S)) {  // down
        player->vel.x -= forwards.x * player->speed * 0.6 * dt;
        player->vel.y -= forwards.y * player->speed * 0.6 * dt;
    }
    if (IsKeyDown(KEY_A)) {  // left
        player->rot -= player->rspeed * dt;
    }
    if (IsKeyDown(KEY_D)) {  // right
        player->rot += player->rspeed * dt;
    }
    if (IsKeyPressed(KEY_SPACE)) {  // shoot
        // create new bullet
        Bullet bullet = {player->pos,
                         (Vector2){cos(rad) * 10.0f, sin(rad) * 10.0f},
                         true,
                         10.0f,
                         player->rot,
                         0.0f,
                         15.0f};
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!player->bullets[i].active) {
                player->bullets[i] = bullet;
                printf("bullet %d is active", i);
                break;
            }
        }
    }

    // update pos
    player->pos.x += player->vel.x;
    player->pos.y += player->vel.y;

    // apply friction
    player->vel.x *= powf(player->friction, dt);
    player->vel.y *= powf(player->friction, dt);

    if (player->pos.x < 0.0f) {
        player->pos.x = SCREEN_WIDTH;
    }
    if (player->pos.x > SCREEN_WIDTH) {
        player->pos.x = 0.0f;
    }
    if (player->pos.y < 0.0f) {
        player->pos.y = SCREEN_HEIGHT;
    }
    if (player->pos.y > SCREEN_HEIGHT) {
        player->pos.y = 0.0f;
    }

    float speed = sqrt(player->vel.x * player->vel.x + player->vel.y * player->vel.y);
    if (speed > 200.0f) {
        player->vel.x = (player->vel.x / speed) * 100.0f;
        player->vel.y = (player->vel.y / speed) * 100.0f;
    }
}

void DrawBullet(Bullet *bullet) {
    if (bullet->active) DrawRectangle(bullet->pos.x, bullet->pos.y, 8, 8, RED);
}

void UpdateBullet(Bullet *bullet, float dt) {
    if (!bullet->active) return;
    // update position
    bullet->pos.x += bullet->vel.x;
    bullet->pos.y += bullet->vel.y;

    bullet->lifetime += dt;

    if (bullet->lifetime > bullet->maxlife) {
        bullet->active = false;
        printf("bullet is now inactive");
    }
}

void DrawAsteroid(Asteroid *asteroid) {
    if (asteroid->active)
        DrawPolyLines(asteroid->pos, 8, asteroid->size, asteroid->angle, RAYWHITE);
}

Asteroid CreateAsteroid() {
    // position should be off screen
    // velocity is a vector2 from pos towards center of screen
    // active false, inactive if lifetime > maxlife, or is hit, or not activated
    // accel ? should this be friction??
    // rot spinning asteroid in space
    // rspeed rotational speed (probs rvel is better)
    // angle maybe unneeded since we're adding two vectors
    // lifetime nb of seconds asteroid is alive
    // maxlife: nb of seconds until asteroid is destroyed and set inactive

    // empty init
    Vector2 initpos = {0, 0};
    initpos.x= rand() % SCREEN_WIDTH;
    initpos.y= rand() % SCREEN_HEIGHT;
    // center of the screen
    Vector2 centerpos = {(float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2}; // should be random from area near center of screen
    Vector2 initvel = Vector2Subtract(initpos, centerpos); // good enough for now
    float rotvel = rand() % 360;

    Asteroid asteroid = { 
        initpos,
        initvel,
        false,
        ASTEROID_MEDIUM,
        0.0f,
        0.0f,
        rotvel,
        0.0f,
        0.0f,
        32.0f,
    };
    return asteroid;
}

void UpdateAsteroid(Asteroid *asteroid){


}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib starter");
    SetTargetFPS(TARGET_FRAMERATE);

    Asteroid asteroids[MAX_ASTEROIDS];
    // initialize asteroids
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids
    }

    Player player = {(Vector2){400, 300}, (Vector2){0, 0}, 10.0f, 0.2f, 0.0f, 200.0f};

    // initialize bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        player.bullets[i] =
            (Bullet){(Vector2){0, 0}, (Vector2){0, 0}, false, 0.0f, 0.0f, 0.0f, 0.0f};
    }

    float dt = 0.0f;

    while (!WindowShouldClose()) {
        dt = GetFrameTime();

        // update
        UpdatePlayer(&player, dt);
        for (int i = 0; i < MAX_BULLETS; i++) {
            UpdateBullet(&player.bullets[i], dt);
        }

        BeginDrawing();
        ClearBackground(NEARBLACK);
        // DrawText("asteroids", GetScreenWidth()/2, GetScreenHeight()/2, 20, RAYWHITE);
        DrawPlayer(&player);

        for (int i = 0; i < MAX_BULLETS; i++) {
            DrawBullet(&player.bullets[i]);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
