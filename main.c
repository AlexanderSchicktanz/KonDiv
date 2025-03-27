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
    LOESUNG
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
    if(width<height) printf("Mobile.\n");
    /*
    Image info = LoadImage("./resources/info.png");
    if(!info.data) {
        printf("Texture not loaded!\n");
        return 1;
    }
    Texture2D infoTex = LoadTextureFromImage(info);
    
    Texture2D task = LoadTexture("./resources/task.png");
    Texture2D quellen = LoadTexture("./resources/quellen.png");
    Texture2D loesung = LoadTexture("./resources/loesung.png");*/
    Rectangle infoButton = (Rectangle){width/4-width/12, height/2, width/6, width/6};
    Rectangle taskButton = (Rectangle){width/2-width/12, height/2, width/6, width/6};
    Rectangle quellenButton = (Rectangle){width*3/4-width/12, height/2, width/6, width/6};
    Rectangle back = (Rectangle){50,height*4/5,width/5,height/20};
    Rectangle forward = (Rectangle){100+width/5,height*4/5,width/5,height/20};
    InitWindow(width, height, "KonDiv");
    SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    SetTargetFPS(60);
    Image info = LoadImage("./resources/info.png");
    Texture2D infoTex = LoadTextureFromImage(info);//this breaks the website
    UnloadImage(info);
    Image test = LoadImage("./resources/test.png");
    Texture2D testTex = LoadTextureFromImage(test);
    UnloadImage(test);
    Image quellen = LoadImage("./resources/quellen.png");
    Texture2D quellenTex = LoadTextureFromImage(quellen);
    UnloadImage(quellen);

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
                //DrawTexture(infoTex,0,0,WHITE);
                DrawTextBoxed(def, "Von Tintenfischen und Vögeln:\nKonvergenz und Divergenz in der Evolution", (Rectangle){ 100,100,width*4/5,height*3/4 }, width/25, width/250, true, WHITE);
                
                //DrawRectangle(infoButton.x, infoButton.y, infoButton.width, infoButton.height, BLUE);
                Rectangle source = { 0, 0, infoTex.width, infoTex.height };  // Full texture
                Vector2 origin = { 0, 0 };  // Top-left corner as rotation origin
                float rotation = 0.0f;  // No rotation

                DrawTexturePro(infoTex, source, infoButton, origin, rotation, WHITE);
                source = (Rectangle){ 0, 0, testTex.width, testTex.height };
                DrawTexturePro(testTex, source, taskButton, origin, rotation, WHITE);
                source = (Rectangle){ 0, 0, quellenTex.width, quellenTex.height };
                DrawTexturePro(quellenTex, source, quellenButton, origin, rotation, WHITE);
                //DrawTexture(infoTex,infoButton.x,infoButton.y,WHITE);
                //DrawRectangle(taskButton.x, taskButton.y, taskButton.width, taskButton.height, RED);
                //DrawRectangle(quellenButton.x, quellenButton.y, quellenButton.width, quellenButton.height, LIGHTBLUE);
                //DrawTextEx(def, "Informationen", (Vector2){infoButton.x,infoButton.y-200}, 200, 40, WHITE);
                //DrawTextEx(def, "Aufgaben", (Vector2){taskButton.x,infoButton.y-200}, 200, 40, WHITE);
                //DrawTextEx(def, "Quellen", (Vector2){quellenButton.x,infoButton.y-200}, 200, 40, WHITE);
                DrawTextBoxed(def, "Informationen", (Rectangle){ infoButton.x,infoButton.y-width/50,width*4/5,height*3/4 }, width/50, width/500, true, WHITE);
                DrawTextBoxed(def, "Aufgaben", (Rectangle){ taskButton.x,infoButton.y-width/50,width*4/5,height*3/4 }, width/50, width/500, true, WHITE);
                DrawTextBoxed(def, "Quellen", (Rectangle){ quellenButton.x,infoButton.y-width/50,width*4/5,height*3/4 }, width/50, width/500, true, WHITE);
                
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
                DrawTextEx(def,"Informationen", (Vector2){100, 100}, 200, 40, WHITE);
                DrawRectangleRec(back, GRAY);
                DrawTextEx(def, "Zurück", (Vector2){back.x, back.y}, width/40, 10, WHITE);
                if (CheckCollisionPointRec(mousePos, back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                //DrawWrappedText(Font font, const char *text, Vector2 position, float fontSize, float spacing, float wrapWidth, Color color)
                char* kon = "Konvergente Evolution und Analogie\n"
""
"Unter konvergenter Evolution versteht man das Entstehen ähnlicher bzw. analoger Merkmale bei nicht näher verwandten Arten. Im Laufe der Evolution geschah dies durch die Anpassung an eine ähnliche funktionale Anforderung und ähnliche Umweltbedingungen (ähnliche ökologische Nischen). Der Grund für konvergente Entwicklungen besteht also in einem Wechselspiel von ähnlichem Selektionsdruck und Entwicklungskorridoren*, sowie gleichen Selektionsfaktoren, die zu vergleichbaren Anpassungen geführt haben.\n"
"Analoge Organe ähneln sich nicht nur in der Funktion, sondern manchmal auch äußerlich, teilweise sogar (oberflächlich) anatomisch. Allerdings sind sie unabhängig voneinander entstanden.\n"
"Das Prinzip der Konvergenz lässt sich zum Beispiel bei den Schnäbeln von Vögeln und Tintenfischen beobachten. Obwohl Tintenfische im Wasser leben, bildeten sie ähnliche Strukturen wie Vögel aus, deren Lebensraum sich an Land und in der Luft befindet. Dabei sind die Arten gar nicht miteinander verwandt. Auch in der Pflanzenwelt lassen sich konvergente Vorgänge beobachten: So werden von Laien Stacheln oft mit Dornen verwechselt. Im Gegensatz zu den Dornen werden Stacheln aber nur aus den oberen Zellschichten gebildet. Sie sind also nur Oberflächenstrukturen. Dornen sind hingegen Umbildungen der Blätter oder der Sprossachse.\n"
"Aus der konvergenten Evolution lässt sich ableiten, dass sich ähnliche Merkmale direkt auf ihre Funktion zurückführen lassen, aber nicht unbedingt einen Rückschluss auf nahe Verwandtschaft zwischen zwei Arten liefern.\n"
"* Die Bandbreite an möglichen Phänotypen, die ein bestimmter Genotyp unter verschiedenen Umweltbedingungen annehmen kann.\n"
                "\nDivergente Evolution und Homologie\n"
"Divergente Evolution beschreibt das Entstehen unterschiedlicher Merkmale bei eng verwandten Arten, ist also eine Form der Artbildung. Als Ursache dafür gilt die starke inner- und außerartliche Konkurrenz einer Art, sowie unterschiedliche Evolutionsfaktoren. Trotz divergenter Entwicklungen bleiben Merkmale aber aufgrund der gemeinsamen Abstammung vergleichbar. Diese grundsätzliche Übereinstimmung wird dann als Homologie bezeichnet.\n"
"Beobachtbar ist dieser Vorgang beispielsweise beim Knochenbau von Säugetieren. Obwohl die vorderen Extremitäten z.B. bei Fledermäusen Flügel und beim Menschen Hände ausbildeten, blieben sie von der Grundstruktur und dem Aufbau gleich.\n"
"Divergenz führt zur Besetzung und Bildung neuer ökologischer Nischen und damit zur adaptiven Radiation. Im Endeffekt entstehen so neue Arten (Speziation) und Individuen werden an ihre neuen, sich voneinander unterscheidenden Lebensbedingungen individuell angepasst.\n";
                //DrawWrappedText(def, kon, (Vector2){100, 400}, 80, 10, 1000, WHITE);
                //DrawTextEx(def, kon, (Vector2){100, 400}, 80, 10, WHITE);
                //DrawTextRec(def, kon, (Rectangle){100, 400, 1000, 1000}, 80, 10, true, WHITE);
                DrawTextBoxed(def, kon, (Rectangle){ 100,400,width*4/5,height*3/4 }, width/80, 2.0f, true, WHITE);
            break;
            
            case TEST:
                DrawTextEx(def,"Aufgaben", (Vector2){100, 100}, 200, 40, WHITE);
                DrawRectangleRec(back, GRAY);
                DrawTextEx(def, "Zurück", (Vector2){back.x, back.y}, width/40, 10, WHITE);
                if (CheckCollisionPointRec(mousePos, back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                char* aufgaben = "1. Aufgabe:\nDefinieren sie die Begriffe „Konvergente und Divergente Evolution“ und erläutern Sie anhand eines Beispieles, wie analoge Organe entstehen.\n\n"
                "2. Aufgabe\nVergleichen Sie die Entstehung analoger und homologer Organe und erklären sie deren Entstehung.\n\n"
                "3. Aufgabe\nDiskutieren sie die Aussage: „Analoge Merkmale liefern keine Rückschlüsse auf die Verwandtschaft zwischen Arten, während homologe Merkmale dafür entscheidend sind.“  Beziehen Sie dabei die Begriffe Analogie, Homologie, konvergente und divergente Evolution ein. "
                "";
                DrawRectangleRec(forward, GRAY);
                DrawTextEx(def, "EWH", (Vector2){forward.x, forward.y}, width/40, 10, WHITE);
                DrawTextBoxed(def, aufgaben, (Rectangle){ 100,400,width*4/5,height*3/4 }, width/80, 2.0f, true, WHITE);
                if (CheckCollisionPointRec(mousePos, forward) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = LOESUNG;
                }
                break;
            
            case QUELLEN:
                DrawTextEx(def,"Quellen", (Vector2){100, 100}, 200, 40, WHITE);
                DrawRectangleRec(back, GRAY);
                DrawTextEx(def, "Zurück", (Vector2){back.x, back.y}, width/40, 10, WHITE);
                if (CheckCollisionPointRec(mousePos, back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = MENU;
                }
                char **quellen = (char*[]){"https://studyflix.de/biologie/konvergenz-6333",
"https://simpleclub.com/lessons/biologie-divergenz",
"https://de.wikipedia.org/wiki/Divergenz_(Biologie)",
"https://www.biologie-seite.de/Biologie/Konvergenz_(Biologie)",
"https://icon-library.com/images/info-icon/info-icon-22.jpg",
"https://static.vecteezy.com/system/resources/previews/019/004/690/original/source-of-river-color-icon-illustration-vector.jpg",
"https://static.vecteezy.com/system/resources/previews/014/988/380/original/paper-test-icon-outline-exam-answer-vector.jpg"};
                DrawTextEx(def, "https://studyflix.de/biologie/konvergenz-6333\n"
"https://simpleclub.com/lessons/biologie-divergenz\n"
"https://de.wikipedia.org/wiki/Divergenz_(Biologie)\n"
"https://www.biologie-seite.de/Biologie/Konvergenz_(Biologie)\n"
"https://icon-library.com/images/info-icon/info-icon-22.jpg\n"
"https://static.vecteezy.com/system/resources/previews/019/004/690/original/source-of-river-color-icon-illustration-vector.jpg\n"
"https://static.vecteezy.com/system/resources/previews/014/988/380/original/paper-test-icon-outline-exam-answer-vector.jpg\n", (Vector2){100, 400}, 80, 10, WHITE);
                for(int i = 0; i < 7; i++){
                    if(CheckCollisionPointRec(mousePos, (Rectangle){100, 400 + i*width/80, 1000, 100}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        OpenURL(quellen[i]);
                    }
                }
                break;
            case LOESUNG:
                DrawTextEx(def,"Lösungen", (Vector2){100, 100}, 200, 40, WHITE);
                DrawRectangleRec(back, GRAY);
                DrawTextEx(def, "Zurück", (Vector2){back.x, back.y}, width/40, 10, WHITE);
                if (CheckCollisionPointRec(mousePos, back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = TEST;
                }
                //DrawTextEx(def, "1. Analogie ist das Ergebnis von Konvergenz\n", (Vector2){100, 400}, 40, 10, WHITE);
                char *los = "Definition Konvergenz:\n"
"Entstehung ähnlicher Merkmale bei nicht miteinander verwandten Arten aufgrund ähnlicher Umweltbedingungen und Selektionsfaktoren.\n"
""
"Definition Divergenz: \n"
"Entstehung unterschiedlicher Merkmale bei eng miteinander verwandten Arten durch starke inner- und außerartlicher Konkurrenz und verschiedene Selektionsfaktoren.\n"
""
"Entstehung analoge Organe:\n"
"Analoge Organe entstehen wenn auf zwei nicht verwandte Arten ähnliche Selektionsfaktoren wirken. Dies ist der Fall zum Beispiel bei den Flossen von Fischen und Walen, welche durch eine Anpassung an eine aquatische Lebensweise entstanden sind.\n\n"
"Analoge Organe: \n"
"Entstehen durch Konvergente Evolution und haben eine ähnliche oder gleiche Funktion und Erscheinungsbild. Dabei haben sie keine gemeinsame Abstammung.\n"
""
"Homologe Organe:\n"
"Entstehen durch divergente Evolution und haben verschiedene Funktionen und Erscheinungsbilder.\n"
"Dabei haben sie eine gemeinsame Abstammung. \n"
""
"Erklärung:\n"
"Analoge Organe entwickeln sich durch konvergente Evolution und homologe Organe durch divergente Evolution. Diese unterschiedlichen Evolutionen treten auf, da auf die Arten unterschiedliche Selektionsfaktoren wirken. Die Arten passen sich unterschiedlich an diese Selektionsfaktoren an und entwickeln sich so entweder auseinander oder zusammen. \n\n"
"Mögliche Argumente:\n"
"Analoge Merkmale entstehen durch konvergenter Evolution und zeigen somit nur auf ihre Funktion und damit den Selektionsdruck, welcher auf die Art wirkt. \n"
"Homologe Merkmale entstehen durch divergente Evolution und haben damit unterschiedliche Funktionen und Aussehen. Damit lässt sich auch nicht auf eine Gemeinsame Abstammung schließen.\n"
""
""
"Diskutieren:\n"
"Analoge Merkmale sind eine Reaktion auf Umweltbedingen und nicht auf Abstammung und Homologe Merkmale basieren auf einer gemeinsamen Herkunft haben aber unterschiedliche Funktionen. Deswegen kann man nicht von homologen oder analogen Merkmalen auf die Verwandtschaft schließen.\n";
                DrawTextBoxed(def, los, (Rectangle){ 100,400,width*4/5,height*3/4 }, width/80, 2.0f, true, WHITE);
                break;
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}