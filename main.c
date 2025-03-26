#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>

#define LIGHTBLUE (Color){ 0, 121, 241, 155 }
int infoStartX = -500;
int taskStartX = 50;
int startY = 300;
int endX = 450;
int endY =250;

int width, height;

void setSize(int w, int h) {
    width = w*4;
    height = h*4;
}


void Quellen(){

}

void Info(){
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){33,100,119});
        DrawRectangle(0, 0, width, height, (Color){33,100,119,255});
        DrawText("Informationen", 100, 100, 40, WHITE);
        DrawRectangleRec((Rectangle){50, 500, 100, 40}, GRAY);
        EndDrawing();
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){{50, 500, 100, 40}}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            printf("Back!\n");
            return;
        }
    }
}

void Test(){
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){33,100,119});
        DrawRectangle(0, 0, width, height, (Color){33,100,119,255});
        DrawText("Aufgaben", 100, 100, 40, WHITE);
        DrawRectangleRec((Rectangle){50, 500, 100, 40}, GRAY);
        EndDrawing();
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){{50, 500, 100, 40}}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            return;
        }
    }
}

bool ButtonRect(Rectangle rec, Color col1,Color col2, const char* text, Font font) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), rec);
    DrawRectangleRec(rec, hover?col1:col2);
    DrawTextEx(font,text, (Vector2){rec.x+rec.width/2-MeasureText(text, rec.width/40)/2, rec.y+rec.height/2-20}, rec.width/40, 10, WHITE);
    return (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

int main(void) {
    InitWindow(width, height, "KonDiv");
    printf("Width: %d\nHeight: %d\n", width, height);
    SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_MAXIMIZED);
    SetTargetFPS(60);
    Font def = LoadFontEx("./resources/ViaodaLibre-Regular.ttf", 256, NULL, 255);
    GuiSetFont(def);
    if(!def.texture.id) {
        printf("Font not loaded!\n");
        return 1;
    }
    Vector2 size = MeasureTextEx(def, "Von Tintenfischen und Vögeln: Konvergenz und Divergenz in der Evolution", 40, 10);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        BeginDrawing();
        ClearBackground((Color){33,100,119});
        DrawRectangle(0, 0, width, height, (Color){33,100,119,255});
        DrawTextEx(def,"Von Tintenfischen und Vögeln:", (Vector2){400, 100}, 160, 40, WHITE);
        DrawTextEx(def,"Konvergenz und Divergenz in der Evolution", (Vector2){400, 250}, 160, 40, WHITE);
        /*if(ButtonRect((Rectangle){0, height/2, width/2, height/2}, BLUE, LIGHTBLUE,"Informationen", def)) {
            printf("Left!\n");
        }
        if(ButtonRect((Rectangle){width/2, height/2, width/2, height/2}, BLUE, LIGHTBLUE,"Aufgaben", def)) {
            printf("Right!\n");
        }*/
        if (CheckCollisionPointRec(mousePos, (Rectangle){width/2 + infoStartX, startY, endX, endY})&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Info();
        }
                // Aufgaben-Button Bereich (rot)
        else if (CheckCollisionPointRec(mousePos, (Rectangle){width/2 + taskStartX, startY, endX, endY})&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Test();
        }
        DrawRectangle(width/2 + infoStartX, startY, endX, endY, BLUE);
        DrawRectangle(width/2 + + taskStartX, startY, endX, endY, RED);
        
        DrawText("Informationen", width/2 + infoStartX, startY-20, 20, WHITE);
        DrawText("Aufgaben", width/2 + taskStartX, startY-20, 20, WHITE);
        //if(GuiButton((Rectangle){100, 500, 300, 100}, "Click me!")) printf("Clicked!\n");
        EndDrawing();
    }

    CloseWindow();
    return 0;
}