#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>

int width, height;

void setSize(int w, int h) {
    width = w*4;
    height = h*4;
}

typedef struct cool{
    char r;
    char g;
    char b;
} Colour;

int main(void) {
    InitWindow(width, height, "KonDiv");
    printf("Width: %d\nHeight: %d\n", width, height);
    SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_MAXIMIZED);
    SetTargetFPS(60);
    Font def = LoadFontEx("./resources/ViaodaLibre-Regular.ttf", 256, NULL, 255);
    if(!def.texture.id) {
        printf("Font not loaded!\n");
        return 1;
    }
    Vector2 size = MeasureTextEx(def, "Von Tintenfischen und Vögeln: Konvergenz und Divergenz in der Evolution", 40, 10);
    while (!WindowShouldClose()) {
        
        BeginDrawing();
        ClearBackground((Color){33,100,119});
        DrawTextEx(def,"Von Tintenfischen und Vögeln:", (Vector2){400, (height-size.y)/2}, 160, 40, WHITE);
        DrawTextEx(def,"Konvergenz und Divergenz in der Evolution", (Vector2){400, (height-size.y)/2+size.y*4+40}, 160, 40, WHITE);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
        if(GuiButton((Rectangle){100, 500, 300, 100}, "Click me!")) printf("Clicked!\n");
        EndDrawing();
    }

    CloseWindow();
    return 0;
}