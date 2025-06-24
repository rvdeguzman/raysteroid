#include "../include/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TARGET_FRAMERATE 60

int main(void) {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib starter");
    SetTargetFPS(TARGET_FRAMERATE);

    bool showMessageBox = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        DrawText("welcome to raylib", 200, 190, 20, LIGHTGRAY);

        if (GuiButton((Rectangle){24, 24, 120, 30}, "#191#show msg")) showMessageBox = true;

        if (showMessageBox) {
            int res = GuiMessageBox((Rectangle){85, 70, 250, 100},
                                       "#191#msg box",
                                       "let's learn raylib!!",
                                       "yuh;nuh");

            if (res >= 0) showMessageBox = false;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
