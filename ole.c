#include <raylib.h>
#include "raygui.h"
#include <stdio.h>
#include "raylib.h"


    
    typedef enum {
        SCREEN_MAIN = 0,
        SCREEN_INFO,
        SCREEN_TASKS
    } ScreenType;
    
    int main(void) 
    {
        // Fenstergröße festlegen (kann auch mit GetMonitorWidth(0) für Vollbild angepasst werden)
        const int screenWidth = 1920;
        const int screenHeight = 1080;
        
        InitWindow(screenWidth, screenHeight, "Biologie: Konvergenz & Divergenz");
        SetTargetFPS(60);
        
        Color backgroundColor = (Color){33, 100, 119, 255};
        ScreenType currentScreen = SCREEN_MAIN;
    
        // Werte für größe Bilder Info und tasks
        int infoStartX = -500;
        int taskStartX = 50;
        int startY = 300;
        int endX = 450;
        int endY =250;

        


        while (!WindowShouldClose()) 
        {
            // Mouse erkennung 
            if (currentScreen == SCREEN_MAIN && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Vektor speichert x und y 
                Vector2 mousePos = GetMousePosition();
                
                // Info-Button Bereich (blau)
                // Schaut ob geklickt in Bereich wenn ja ändert Screen
                // Werte immer Gleich mit größe Bildern

                // Bereich als Rectangle 
                if (CheckCollisionPointRec(mousePos, (Rectangle){screenWidth/2 + infoStartX, startY, endX, endY})) 
                {
                    //Wechselt screen
                    currentScreen = SCREEN_INFO;
                }
                // Aufgaben-Button Bereich (rot)
                else if (CheckCollisionPointRec(mousePos, (Rectangle){screenWidth/2 + taskStartX, startY, endX, endY})) 
                {
                    currentScreen = SCREEN_TASKS;
                }
            }
    
          
            BeginDrawing();
                ClearBackground(backgroundColor);
                
                // Weist nicht ob Switch die Idee ist aber erstmal am besten glaube ich da ich nicht weis wie viele screens wir noch brauchen 
                switch (currentScreen) 
                {
                    case SCREEN_MAIN:
                    {
                        // von ChatGPT um Text zu centrieren da es bei mir nicht ging 
                        const char* title = "Von Tintenfischen und Voegeln: Konvergenz & Divergenz in der Evolution";
                        int titleWidth = MeasureText(title, 30);
                        if (titleWidth > screenWidth) titleWidth = screenWidth - 20; // Anpassung falls zu lang
                        
                        DrawText(title, (screenWidth - titleWidth)/2, 50, 30, WHITE);
                        
                        // "Bilder" als farbige Rechtecke
                        DrawRectangle(screenWidth/2 + infoStartX, startY, endX, endY, BLUE);
                        DrawRectangle(screenWidth/2 + + taskStartX, startY, endX, endY, RED);
                        
                        DrawText("Informationen", screenWidth/2 + infoStartX, startY-20, 20, WHITE);
                        DrawText("Aufgaben", screenWidth/2 + taskStartX, startY-20, 20, WHITE);
                    } break;
                    
                    case SCREEN_INFO:
                    // Platzhalter 
                    DrawText("Aufgabenseite (Inhalt kommt hier)", 50, 50, 30, WHITE);
    
                    // zurück Button muss vieleicht noch geändert werden 
                    Rectangle backButton = {50, 500, 100, 40};
                    DrawRectangleRec(backButton, GRAY);
                    DrawText("Zurück", backButton.x + 10, backButton.y + 10, 20, WHITE);
                    
                    // Check ob im bereich Button 
                    if (CheckCollisionPointRec(GetMousePosition(), backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = SCREEN_MAIN;
                    }
                    break;
                    
                    
                    case SCREEN_TASKS:
                    DrawText("Informationsseite (Inhalt kommt hier)", 50, 50, 30, WHITE);
    
                    // Einfacher Back-Button
                    Rectangle backButton2 = {50, 500, 100, 40};
                    DrawRectangleRec(backButton2, GRAY);
                    DrawText("Zurück", backButton2.x + 10, backButton2.y + 10, 20, WHITE);
    
                    if (CheckCollisionPointRec(GetMousePosition(), backButton2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = SCREEN_MAIN;
                    }
                    break;
                   
                }
            EndDrawing();
        }
    
        CloseWindow();
        return 0;
    }
        