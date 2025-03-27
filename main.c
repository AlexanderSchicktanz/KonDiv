#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>

#define LIGHTBLUE (Color){ 0, 121, 241, 155 }

int infoStartX = -500;
int taskStartX = 50;
int quellenStartX = 600;
int startY = 300;
int endX = 450;
int endY = 250;
int width, height;

typedef enum {
    MENU,
    INFO,
    TEST,
    QUELLEN
} Screen;

void setSize(int w, int h) {
    width = w * 4;
    height = h * 4;
}

int main(void) {
    InitWindow(width, height, "KonDiv");
    SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    SetTargetFPS(60);

    Font def = LoadFontEx("./resources/ViaodaLibre-Regular.ttf", 256, NULL, 255);
    GuiSetFont(def);
    if (!def.texture.id) {
        printf("Font not loaded!\n");
        return 1;
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
    Screen currentScreen = MENU;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        BeginDrawing();
        ClearBackground((Color){33, 100, 119});
        DrawRectangle(0, 0, width, height, (Color){33, 100, 119, 255});

        switch (currentScreen) {
            case MENU:
                DrawTextEx(def, "Von Tintenfischen und Vögeln:", (Vector2){400, 100}, 160, 40, WHITE);
                DrawTextEx(def, "Konvergenz und Divergenz in der Evolution", (Vector2){400, 250}, 160, 40, WHITE);
                
                DrawRectangle(width / 2 + infoStartX, startY, endX, endY, BLUE);
                DrawRectangle(width / 2 + taskStartX, startY, endX, endY, RED);
                DrawRectangle(width / 2 + quellenStartX, startY, endX, endY, LIGHTBLUE);
                DrawText("Informationen", width / 2 + infoStartX, startY - 20, 20, WHITE);
                DrawText("Aufgaben", width / 2 + taskStartX, startY - 20, 20, WHITE);
                DrawText("Quellen", width / 2 + quellenStartX, startY - 20, 20, WHITE);
                
                if (CheckCollisionPointRec(mousePos, (Rectangle){width / 2 + infoStartX, startY, endX, endY}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = INFO;
                }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){width / 2 + taskStartX, startY, endX, endY}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = TEST;
                }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){width / 2 + quellenStartX, startY, endX, endY}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = QUELLEN;
                }
                break;

            case INFO:
                DrawText("Informationen", 100, 100, 40, WHITE);
                DrawRectangleRec((Rectangle){50, 500, 100, 40}, GRAY);
                if (CheckCollisionPointRec(mousePos, (Rectangle){50, 500, 100, 40}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                DrawTextEx(def, "Konvergente Evolution (Analogie)\n"
"Definition: das Entstehen ähnlicher Merkmale bei nicht miteinander verwandten Arten, Entwicklung von analogen Merkmalen bei nicht näher verwandten Arten\n"
"Ursache: im Laufe der Evolution durch Anpassung an eine ähnliche funktionale Anforderung und ähnliche Umweltbedingungen (ähnliche ökologische Nischen) ausgebildet, Wechselspiel von ähnlichem Selektionsdruck und Entwicklungskorridoren*1, gleiche Selektionsfaktoren, die zu vergleichbaren Anpassungen geführt haben\n"
"Folge:\n"
"Beispiele: Fische und Wale (Flossen), Vögel und Insekten (Flügel), Dornen und Stacheln*2, Tintenfische und Vögel (Schnabel)\n"
"ähnliche Merkmale lassen sich direkt auf ihre Funktion zurückführen und liefern nicht unbedingt einen Rückschluss auf nahe Verwandtschaft zwischen zwei Arten\n"
"Analoge Organe ähneln sich nicht nur in der Funktion, sondern teilweise auch äußerlich, teilweise sogar (oberflächlich) anatomisch, ABER: unabhängig voneinander entstanden\n"
"Analogie ist das Ergebnis von Konvergenz\n"
"*1 Bandbreite an möglichen Phänotypen, die ein bestimmter Genotyp unter verschiedenen Umweltbedingungen annehmen kann.\n"
"*2 In der Botanik gibt es ebenfalls analoge Bildungen der Pflanzen. So werden von Laien Stacheln oft mit Dornen verwechselt. Im Gegensatz zu den Dornen werden Stacheln aber nur aus den oberen Zellschichten (Epidermis, Rindengewebe) gebildet. Sie sind also nur Oberflächenstrukturen (Emergenzen). Dornen sind hingegen Umbildungen der Blätter oder der Sprossachse.\n", (Vector2){100, 400}, 40, 10, WHITE);
            DrawTextEx(def, "Divergente Evolution\n"
"Definition: das Entstehen unterschiedlicher Merkmale bei eng verwandten Arten, Form der Artbildung\n"
"Ursache: Starke inner- und außerartliche Konkurrenz, verschiedene Evolutionsfaktoren\n"
"Folge: Divergenz führt zur Besetzung und Bildung neuer ökologischer Nischen und damit zur adaptiven Radiation. Im Endeffekt entstehen so neue Arten (Speziation), Individuen werden an ihre neuen, sich voneinander unterscheidenden Lebensbedingungen individuell angepasst\n"
"Beispiele: Darwinfinken, Knochenbau bei Säugetieren\n"
"Trotz der divergenten Entwicklungen bleiben Merkmale aufgrund der gemeinsamen Abstammung vergleichbar. Diese grundsätzliche Übereinstimmung wird als Homologie bezeichnet\n", (Vector2){100, 900}, 40, 10, WHITE);
            break;
            
            case TEST:
                DrawText("Aufgaben", 100, 100, 40, WHITE);
                DrawRectangleRec((Rectangle){50, 500, 100, 40}, GRAY);
                if (CheckCollisionPointRec(mousePos, (Rectangle){50, 500, 100, 40}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                break;
            
            case QUELLEN:
                DrawText("Quellen", 100, 100, 40, WHITE);
                DrawRectangleRec((Rectangle){50, 500, 100, 40}, GRAY);
                if (CheckCollisionPointRec(mousePos, (Rectangle){50, 500, 100, 40}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                DrawTextEx(def, "https://studyflix.de/biologie/konvergenz-6333\n"
"https://simpleclub.com/lessons/biologie-divergenz\n"
"https://de.wikipedia.org/wiki/Divergenz_(Biologie)\n"
"https://www.biologie-seite.de/Biologie/Konvergenz_(Biologie)\n", (Vector2){100, 400}, 40, 10, WHITE);
                break;
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
