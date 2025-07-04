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
} AsteroidSize;  // spaces in array?

// asteroid
typedef struct {
    Vector2 pos;
    Vector2 vel;
    bool active;
    AsteroidSize asteroidSize;
    int size;
    int sides;  // for poly shape
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
        DrawPolyLines(asteroid->pos, asteroid->sides, asteroid->size, asteroid->angle, RAYWHITE);
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
    int edge = rand() % 4;  // 0=top, 1=right, 2=bottom, 3=left

    switch (edge) {
        case 0:
            initpos.x = rand() % SCREEN_WIDTH;
            initpos.y = -50 - (rand() % 100);
            break;
        case 1:
            initpos.x = SCREEN_WIDTH + 50 + (rand() % 100);
            initpos.y = rand() % SCREEN_HEIGHT;
            break;
        case 2:
            initpos.x = rand() % SCREEN_WIDTH;
            initpos.y = SCREEN_HEIGHT + 50 + (rand() % 100);
            break;
        case 3:
            initpos.x = -50 - (rand() % 100);
            initpos.y = rand() % SCREEN_HEIGHT;
            break;
    }

    float centerradius = 80.0f;
    Vector2 centerpos = {(float)SCREEN_WIDTH / 2,
                         (float)SCREEN_HEIGHT /
                             2};  // should be random from area near center of screen

    float offset_angle = ((float)rand() / RAND_MAX) * 2.0f * PI;
    float offset_distance = ((float)rand() / RAND_MAX) * centerradius;
    Vector2 offset = {cos(offset_angle) * offset_distance, sin(offset_angle) * offset_distance};
    // final position of the random point about center
    Vector2 target = Vector2Add(centerpos, offset);

    Vector2 initvel = Vector2Subtract(target, initpos);  // good enough for now
    initvel = Vector2Normalize(initvel);
    float speed = (float)30 + (rand() % (60 - 30 + 1));

    initvel = Vector2Scale(initvel, speed);
    float rotvel = rand() % 360;
    int sides = 5 + (rand() % (12 - 5 + 1));

    AsteroidSize asteroidSize;
    int prob = rand() % 100;
    if (prob < 20) {
        asteroidSize = ASTEROID_SMALL;
    } else if (prob < 60) {
        asteroidSize = ASTEROID_MEDIUM;
    } else {
        asteroidSize = ASTEROID_LARGE;
    }

    int size;
    switch (asteroidSize) {
        case ASTEROID_SMALL:
            size = 20 + (rand() % 20);  // 20-40
            break;
        case ASTEROID_MEDIUM:
            size = 40 + (rand() % 30);  // 40-70
            break;
        case ASTEROID_LARGE:
            size = 70 + (rand() % 40);  // 70-110
            break;
    }

    Asteroid asteroid = {
        initpos,
        initvel,
        false,
        asteroidSize,
        size,
        sides,
        0.0f,
        0.0f,
        rotvel,
        0.0f,
        0.0f,
        32.0f,
    };
    return asteroid;
}

void UpdateAsteroid(Asteroid *asteroid, float dt) {
    if (!asteroid->active) return;

    // update position
    asteroid->pos.x += asteroid->vel.x * dt;
    asteroid->pos.y += asteroid->vel.y * dt;

    // update rotation
    asteroid->angle += asteroid->rspeed * dt;

    // update lifetime
    asteroid->lifetime += dt;
    if (asteroid->lifetime > asteroid->maxlife) {
        // make sure it's off-screen
        if (asteroid->pos.x < -20 || asteroid->pos.x > SCREEN_WIDTH + 20 || asteroid->pos.y < -20 ||
            asteroid->pos.y > SCREEN_HEIGHT + 20) {
            asteroid->active = false;
            printf("asteroid is now inactive");
        }
    }
}

// apply a force to repulse asteroids away from each other
void SeparateAsteroids(Asteroid *asteroids, int count, float dt) {
    float force = 150.0f;

    for (int i = 0; i < count; i++) {
        if (!asteroids[i].active) continue;

        Vector2 repulsion = Vector2Zero();
        int near = 0;

        for (int j = 0; j < count; j++) {
            if (i == j || !asteroids[j].active) continue;

            Vector2 diff = Vector2Subtract(asteroids[i].pos, asteroids[j].pos);
            float distance = Vector2Length(diff);

            float radii = asteroids[i].size + asteroids[j].size;

            if (distance < radii && distance > 0) {
                Vector2 push_direction = Vector2Normalize(diff);
                float push_strength = (radii - distance) / radii;
                Vector2 push = Vector2Scale(push_direction, push_strength * force);
                repulsion = Vector2Add(repulsion, push);
                near++;
            }
        }

        if (near > 0) {
            repulsion = Vector2Scale(repulsion, 1.0f / near);
            Vector2 repulsevel = Vector2Scale(repulsion, dt);
            asteroids[i].vel = Vector2Add(asteroids[i].vel, repulsevel);
        }
    }
}

Asteroid CreateAsteroidAtPos(Vector2 pos, AsteroidSize asteroidSize) {
    float angle = ((float)rand() / RAND_MAX) * 2.0f * PI;  // random angle post split
    float speed = 30 + (rand() % 31);
    Vector2 vel = {cos(angle) * speed, sin(angle) * speed};

    float rotvel = rand() % 360;
    int sides = 5 + (rand() % (12 - 5 + 1));

    int size;
    switch (asteroidSize) {
        case ASTEROID_SMALL:
            size = 20 + (rand() % 20);
            break;
        case ASTEROID_MEDIUM:
            size = 40 + (rand() % 30);
            break;
        case ASTEROID_LARGE:
            size = 70 + (rand() % 40);
            break;
    }

    Asteroid asteroid = {
        pos,
        vel,
        false,
        asteroidSize,
        size,
        sides,
        0.0f,
        0.0f,
        rotvel,
        0.0f,
        0.0f,
        32.0f,
    };
    return asteroid;
}
void SplitAsteroid(Asteroid *asteroid, Asteroid *asteroids) {
    Vector2 parentPos = asteroid->pos;
    AsteroidSize parentSize = asteroid->asteroidSize;

    // deactivate parent
    asteroid->active = false;

    AsteroidSize size;
    int nbSpawn;

    switch (parentSize) {
        case ASTEROID_LARGE:
            size = ASTEROID_MEDIUM;
            nbSpawn = 2;
            break;
        case ASTEROID_MEDIUM:
            size = ASTEROID_SMALL;
            nbSpawn = 2;
            break;
        case ASTEROID_SMALL:
            return;
    }

    for (int spawn = 0; spawn < nbSpawn; spawn++) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (!asteroids[i].active) {
                asteroids[i] = CreateAsteroidAtPos(parentPos, size);
                asteroids[i].active = true;
                asteroids[i].lifetime = 0.0f;
                break;
            }
        }
    }
}

void CheckBulletAsteroidHit(Player *player, Asteroid *asteroids) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!player->bullets[i].active) continue;

        for (int j = 0; j < MAX_ASTEROIDS; j++) {
            if (!asteroids[j].active) continue;

            Vector2 diff = Vector2Subtract(player->bullets[i].pos, asteroids[j].pos);
            float distance = Vector2Length(diff);

            if (distance < asteroids[j].size) {
                // bullet on hit
                player->bullets[i].active = false;

                // split!
                SplitAsteroid(&asteroids[j], asteroids);
                break;
            }
        }
    }
}

void CheckPlayerAsteroidHit(Player *player, Asteroid *asteroids, float dt) {
    float player_radius = 20.0f;

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!asteroids[i].active) continue;

        Vector2 diff = Vector2Subtract(player->pos, asteroids[i].pos);
        float distance = Vector2Length(diff);

        if (distance < (player_radius + asteroids[i].size) && distance > 0) {
            Vector2 collision_direction = Vector2Normalize(diff);

            float player_bump_force = 300.0f;
            Vector2 player_bump = Vector2Scale(collision_direction, player_bump_force);
            player->vel = Vector2Add(player->vel, Vector2Scale(player_bump, dt));

            float asteroid_bump_force = 100.0f;
            Vector2 asteroid_bump = Vector2Scale(collision_direction, -asteroid_bump_force);
            asteroids[i].vel = Vector2Add(asteroids[i].vel, Vector2Scale(asteroid_bump, dt));
        }
    }
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raysteroids - asteroiods in C");
    SetTargetFPS(TARGET_FRAMERATE);

    Asteroid asteroids[MAX_ASTEROIDS];
    // initialize asteroids
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i] = CreateAsteroid();
    }

    Player player = {(Vector2){400, 300}, (Vector2){0, 0}, 10.0f, 0.2f, 0.0f, 200.0f};

    // initialize bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        player.bullets[i] =
            (Bullet){(Vector2){0, 0}, (Vector2){0, 0}, false, 0.0f, 0.0f, 0.0f, 0.0f};
    }

    float dt = 0.0f;
    float asteroidTimer = 0.0f;
    float spawn = 2.0f;

    while (!WindowShouldClose()) {
        dt = GetFrameTime();

        asteroidTimer += dt;
        if (asteroidTimer >= spawn) {
            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                if (!asteroids[i].active) {
                    asteroids[i] = CreateAsteroid();
                    asteroids[i].active = true;
                    asteroids[i].lifetime = 0.0f;
                    break;
                }
            }
            asteroidTimer = 0.0f;
        }

        // update
        UpdatePlayer(&player, dt);

        for (int i = 0; i < MAX_BULLETS; i++) {
            UpdateBullet(&player.bullets[i], dt);
        }

        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            UpdateAsteroid(&asteroids[i], dt);
        }
        SeparateAsteroids(asteroids, MAX_ASTEROIDS, dt);

        CheckBulletAsteroidHit(&player, asteroids);
        CheckPlayerAsteroidHit(&player, asteroids, dt);

        BeginDrawing();
        ClearBackground(NEARBLACK);
        // DrawText("asteroids", GetScreenWidth()/2, GetScreenHeight()/2, 20, RAYWHITE);
        DrawPlayer(&player);

        for (int i = 0; i < MAX_BULLETS; i++) {
            DrawBullet(&player.bullets[i]);
        }

        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            DrawAsteroid(&asteroids[i]);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
