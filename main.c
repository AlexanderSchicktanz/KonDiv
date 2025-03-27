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

static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
static void DrawTextBoxedSelectable(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);

typedef enum {
    MENU,
    INFO,
    TEST,
    QUELLEN,
    LÖUNG
} Screen;

void setSize(int w, int h) {
    width = w * 4;
    height = h * 4;
}

static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}

// Draw text using font inside rectangle limits with support for text selection
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;          // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize/(float)font.baseSize;     // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;         // Index where to begin drawing (where a line begins)
    int endLine = -1;           // Index where to stop drawing (where a line ends)
    int lastk = -1;             // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width*scaleFactor : font.glyphs[index].advanceX*scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in startLine and endLine, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE)
        {
            // TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // Ref: http://jkorpela.fi/chars/spaces.html
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1)? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n') state = !state;

            if (state == DRAW_STATE)
            {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (codepoint == '\n')
            {
                if (!wordWrap)
                {
                    textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize*scaleFactor) > rec.height) break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec((Rectangle){ rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize*scaleFactor }, selectBackTint);
                    isGlyphSelected = true;
                }

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, (Vector2){ rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, isGlyphSelected? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;  // avoid leading spaces
    }
}

void DrawWrappedText(Font font, const char *text, Vector2 position, float fontSize, float spacing, float wrapWidth, Color color) {
    int length = strlen(text);
    char buffer[length + 1]; // Temporary buffer to store lines
    int bufferIndex = 0;

    Vector2 cursor = position;
    float spaceWidth = MeasureTextEx(font, " ", fontSize, spacing).x;

    for (int i = 0; i < length; i++) {
        buffer[bufferIndex++] = text[i];
        buffer[bufferIndex] = '\0';

        // Measure text width
        Vector2 textSize = MeasureTextEx(font, buffer, fontSize, spacing);

        if (text[i] == ' ' || text[i] == '\n' || i == length - 1) {
            if (textSize.x > wrapWidth) {
                // Find last space to break line
                int lastSpace = bufferIndex - 1;
                while (lastSpace > 0 && buffer[lastSpace] != ' ') {
                    lastSpace--;
                }
                if (lastSpace > 0) {
                    buffer[lastSpace] = '\0'; // Break at last space
                    DrawTextEx(font, buffer, cursor, fontSize, spacing, color);
                    cursor.y += fontSize + spacing; // Move cursor to next line
                    bufferIndex = 0;
                    i = lastSpace; // Continue after last space
                } else {
                    // No space found, force break
                    DrawTextEx(font, buffer, cursor, fontSize, spacing, color);
                    cursor.y += fontSize + spacing;
                    bufferIndex = 0;
                }
            }
        }
    }
    // Draw remaining text if any
    if (bufferIndex > 0) {
        DrawTextEx(font, buffer, cursor, fontSize, spacing, color);
    }
}

int main(void) {
    Image info = LoadImage("./resources/info.png");
    if(!info.data) {
        printf("Texture not loaded!\n");
        return 1;
    }
    Texture2D infoTex = LoadTextureFromImage(info);
    /*
    Texture2D task = LoadTexture("./resources/task.png");
    Texture2D quellen = LoadTexture("./resources/quellen.png");
    Texture2D loesung = LoadTexture("./resources/loesung.png");*/
    Rectangle infoButton = (Rectangle){width/4-width/12, height/2, width/6, width/6};
    Rectangle taskButton = (Rectangle){width/2-width/12, height/2, width/6, width/6};
    Rectangle quellenButton = (Rectangle){width*3/4-width/12, height/2, width/6, width/6};
    Rectangle back = (Rectangle){50,height*4/5,width/5,height/20};
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
                
                DrawRectangle(infoButton.x, infoButton.y, infoButton.width, infoButton.height, BLUE);
                //DrawTexture(infoTex,infoButton.x,infoButton.y,WHITE);
                DrawRectangle(taskButton.x, taskButton.y, taskButton.width, taskButton.height, RED);
                DrawRectangle(quellenButton.x, quellenButton.y, quellenButton.width, quellenButton.height, LIGHTBLUE);
                DrawTextEx(def, "Informationen", (Vector2){infoButton.x,infoButton.y-200}, 160, 40, WHITE);
                DrawTextEx(def, "Aufgaben", (Vector2){taskButton.x,infoButton.y-200}, 160, 40, WHITE);
                DrawTextEx(def, "Quellen", (Vector2){quellenButton.x,infoButton.y-200}, 160, 40, WHITE);
                
                if (CheckCollisionPointRec(mousePos, infoButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = INFO;
                }
                else if (CheckCollisionPointRec(mousePos, taskButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = TEST;
                }
                else if (CheckCollisionPointRec(mousePos, quellenButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = QUELLEN;
                }
                break;

            case INFO:
                DrawText("Informationen", 100, 100, 40, WHITE);
                DrawRectangleRec(back, GRAY);
                if (CheckCollisionPointRec(mousePos, back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                //DrawWrappedText(Font font, const char *text, Vector2 position, float fontSize, float spacing, float wrapWidth, Color color)
                char* kon = "Konvergente Evolution (Analogie)\n"
"Definition: das Entstehen ähnlicher Merkmale bei nicht miteinander verwandten Arten, Entwicklung von analogen Merkmalen bei nicht näher verwandten Arten\n"
"Ursache: im Laufe der Evolution durch Anpassung an eine ähnliche funktionale Anforderung und ähnliche Umweltbedingungen (ähnliche ökologische Nischen) ausgebildet, Wechselspiel von ähnlichem Selektionsdruck und Entwicklungskorridoren*1, gleiche Selektionsfaktoren, die zu vergleichbaren Anpassungen geführt haben\n"
"Folge:\n"
"Beispiele: Fische und Wale (Flossen), Vögel und Insekten (Flügel), Dornen und Stacheln*2, Tintenfische und Vögel (Schnabel)\n"
"ähnliche Merkmale lassen sich direkt auf ihre Funktion zurückführen und liefern nicht unbedingt einen Rückschluss auf nahe Verwandtschaft zwischen zwei Arten\n"
"Analoge Organe ähneln sich nicht nur in der Funktion, sondern teilweise auch äußerlich, teilweise sogar (oberflächlich) anatomisch, ABER: unabhängig voneinander entstanden\n"
"Analogie ist das Ergebnis von Konvergenz\n"
"*1 Bandbreite an möglichen Phänotypen, die ein bestimmter Genotyp unter verschiedenen Umweltbedingungen annehmen kann.\n"
"*2 In der Botanik gibt es ebenfalls analoge Bildungen der Pflanzen. So werden von Laien Stacheln oft mit Dornen verwechselt. Im Gegensatz zu den Dornen werden Stacheln aber nur aus den oberen Zellschichten (Epidermis, Rindengewebe) gebildet. Sie sind also nur Oberflächenstrukturen (Emergenzen). Dornen sind hingegen Umbildungen der Blätter oder der Sprossachse.\n"
"\nDivergente Evolution\n"
"Definition: das Entstehen unterschiedlicher Merkmale bei eng verwandten Arten, Form der Artbildung\n"
"Ursache: Starke inner- und außerartliche Konkurrenz, verschiedene Evolutionsfaktoren\n"
"Folge: Divergenz führt zur Besetzung und Bildung neuer ökologischer Nischen und damit zur adaptiven Radiation. Im Endeffekt entstehen so neue Arten (Speziation), Individuen werden an ihre neuen, sich voneinander unterscheidenden Lebensbedingungen individuell angepasst\n"
"Beispiele: Darwinfinken, Knochenbau bei Säugetieren\n"
"Trotz der divergenten Entwicklungen bleiben Merkmale aufgrund der gemeinsamen Abstammung vergleichbar. Diese grundsätzliche Übereinstimmung wird als Homologie bezeichnet\n";
                //DrawWrappedText(def, kon, (Vector2){100, 400}, 80, 10, 1000, WHITE);
                //DrawTextEx(def, kon, (Vector2){100, 400}, 80, 10, WHITE);
                //DrawTextRec(def, kon, (Rectangle){100, 400, 1000, 1000}, 80, 10, true, WHITE);
                DrawTextBoxed(def, kon, (Rectangle){ 100,400,width*4/5,height*3/4 }, 80.0f, 2.0f, true, WHITE);
            break;
            
            case TEST:
                DrawText("Aufgaben", 100, 100, 40, WHITE);
                DrawRectangleRec(back, GRAY);
                DrawRectangleRec((Rectangle){300, 500, 100, 40}, GRAY);
                if (CheckCollisionPointRec(mousePos, back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                if (CheckCollisionPointRec(mousePos, (Rectangle){300, 500, 100, 40}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = LÖUNG;
                }
                break;
            
            case QUELLEN:
                DrawText("Quellen", 100, 100, 40, WHITE);
                DrawRectangleRec(back, GRAY);
                if (CheckCollisionPointRec(mousePos, back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                DrawTextEx(def, "https://studyflix.de/biologie/konvergenz-6333\n"
"https://simpleclub.com/lessons/biologie-divergenz\n"
"https://de.wikipedia.org/wiki/Divergenz_(Biologie)\n"
"https://www.biologie-seite.de/Biologie/Konvergenz_(Biologie)\n", (Vector2){100, 400}, 40, 10, WHITE);
                break;
            case LÖUNG:
                DrawText("Lösungen", 100, 100, 40, WHITE);
                DrawRectangleRec(back, GRAY);
                if (CheckCollisionPointRec(mousePos, back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = TEST;
                }
                DrawTextEx(def, "1. Analogie ist das Ergebnis von Konvergenz\n", (Vector2){100, 400}, 40, 10, WHITE);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}