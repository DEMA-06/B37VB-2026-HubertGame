#include <time.h>
#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>


#define CELL_SIZE  40
#define CELL_COUNT 15
#define FRAME_RATE 1000

#define UP    (Vector2) {  0, -1 }
#define DOWN  (Vector2) {  0,  1 }
#define LEFT  (Vector2) { -1,  0 }
#define RIGHT (Vector2) {  1,  0 }

// initialise colour presets
Color green = {120,255,96,200};
Color darkGreen = {43, 51, 24, 255};
Color blue = {43,22,133,150};
Color white = {255,255,255,255};
Color red = {190,30,40,180};
Color purple = {190,30, 85, 255};

struct Player {
    Vector2 position;
    bool canMoveLeft;
    bool canMoveRight;
    bool canMoveUp;
    bool canMoveDown;
    int score;
};
struct Node {
    Vector2 position;
    Vector2 direction;
};
struct Maze {
    int width, height;
    int iterations;
    bool isMapA;
    bool isGenerating;
    struct Node (*map)[CELL_COUNT];
    struct Node (*visibleMap)[CELL_COUNT];
    struct Node mapA[CELL_COUNT][CELL_COUNT];
    struct Node mapB[CELL_COUNT][CELL_COUNT];
    Vector2 origin;
    Vector2 nextOrigin;
    Vector2 visibleOrigin;
    Vector2 possibleDirections[];
};
struct Time {
    float timeLeft;
    float addTime;
    Vector2 timerPos;
    Vector2 timerSize;
};
struct HUD {
    int scoreSize, scoreLength;
    Vector2 scorePos;
    Vector2 scoreBackgndPos;

};

// Node Functions
void SetNodeDirection(struct Node* node, int x, int y) {
    node->direction = (Vector2) {(float) x, (float) y};
}
void DrawNode(struct Node* node) {
    if (node->direction.x == 0) {
        if (node->direction.y == -1) {
            //upward node
            if (node->position.x == 0  && node->position.y != (CELL_COUNT-1)) {
                DrawRectangle((CELL_SIZE * node->position.x + 5) , (CELL_SIZE * node->position.y) , CELL_SIZE - 10, CELL_SIZE, BLACK);
            }
            else if (node->position.y == (CELL_COUNT-1) && node->position.x != 0) {
                DrawRectangle((CELL_SIZE * node->position.x ) , (CELL_SIZE * node->position.y - 5) , CELL_SIZE - 5, CELL_SIZE , BLACK);
            }
            else if (node->position.x == 0 && node->position.y == (CELL_COUNT-1)) {
                DrawRectangle((CELL_SIZE * node->position.x + 5) , (CELL_SIZE * node->position.y) , CELL_SIZE - 10, CELL_SIZE - 5, BLACK);
            }
            else {
                DrawRectangle((CELL_SIZE * node->position.x) , (CELL_SIZE * node->position.y) , CELL_SIZE - 5, CELL_SIZE, BLACK);
            }
        }
        else if (node->direction.y == 1) {
            //downward node
            if (node->position.x == 0) {
                DrawRectangle((CELL_SIZE * node->position.x + 5) , (CELL_SIZE * node->position.y) + 5, CELL_SIZE - 10, CELL_SIZE, BLACK);
            }
            else {
                DrawRectangle((CELL_SIZE * node->position.x) , (CELL_SIZE * node->position.y) + 5, CELL_SIZE - 5, CELL_SIZE, BLACK);
            }

        }
    }
    else if (node->direction.y == 0) {
        if (node->direction.x == -1) {
            //left-facing node
            if (node->position.y == (CELL_COUNT-1)) {
                DrawRectangle((CELL_SIZE * node->position.x) - 10 , (CELL_SIZE * node->position.y + 5) , CELL_SIZE + 5, CELL_SIZE - 10, BLACK);

            }
            else {
                DrawRectangle((CELL_SIZE * node->position.x) - 10 , (CELL_SIZE * node->position.y) + 5, CELL_SIZE + 5, CELL_SIZE - 5, BLACK);
            }
        }
            //right-facing node
        else if (node->direction.x == 1) {
            if (node->position.x == 0  && node->position.y != (CELL_COUNT-1)) {
                DrawRectangle((CELL_SIZE * node->position.x + 5) , (CELL_SIZE * node->position.y) + 5, CELL_SIZE , CELL_SIZE - 5, BLACK);
            }
            else if (node->position.y == (CELL_COUNT-1) && node->position.x != 0){
                DrawRectangle((CELL_SIZE * node->position.x) , (CELL_SIZE * node->position.y) + 5, CELL_SIZE + 5, CELL_SIZE - 10, BLACK);
            }
            else if (node->position.x == 0 && node->position.y == (CELL_COUNT-1)) {
                DrawRectangle((CELL_SIZE * node->position.x + 5) , (CELL_SIZE * node->position.y) + 5, CELL_SIZE , CELL_SIZE - 10, BLACK);
            }
            else {
                DrawRectangle((CELL_SIZE * node->position.x) , (CELL_SIZE * node->position.y) + 5, CELL_SIZE + 5, CELL_SIZE - 5, BLACK);
            }

        }
    }
}
void DrawOrigin(struct Maze* maze) {
            if (maze->visibleOrigin.x == 0 && maze->visibleOrigin.y != CELL_COUNT - 1) {
                DrawRectangle((CELL_SIZE * maze->visibleOrigin.x + 5) , (CELL_SIZE * maze->visibleOrigin.y) + 5, CELL_SIZE - 10, CELL_SIZE - 5, purple);

            }
            else if (maze->visibleOrigin.x != 0 && maze->visibleOrigin.y == CELL_COUNT - 1) {
                DrawRectangle((CELL_SIZE * maze->visibleOrigin.x) , (CELL_SIZE * maze->visibleOrigin.y ) + 5, CELL_SIZE - 5, CELL_SIZE - 10, purple);
            }
            else if (maze->visibleOrigin.x == 0 && maze->visibleOrigin.y == CELL_COUNT - 1) {
                DrawRectangle((CELL_SIZE * maze->visibleOrigin.x + 5) , (CELL_SIZE * maze->visibleOrigin.y) + 5, CELL_SIZE - 10, CELL_SIZE - 10, purple);

            }
            else {
                DrawRectangle((CELL_SIZE * maze->visibleOrigin.x) , (CELL_SIZE * maze->visibleOrigin.y) + 5, CELL_SIZE - 5, CELL_SIZE - 5, purple);

            }
}

// Timer Functions
void InitTime(struct Time* time) {
    time->timeLeft = FRAME_RATE;
    time->addTime = FRAME_RATE / 10;
    time->timerPos.x = CELL_SIZE/4;
    time->timerPos.y = CELL_SIZE/4;
    time->timerSize.x = CELL_SIZE;
    time->timerSize.y = time->timeLeft/3;
};
void CalcTimer(struct Time* time) {
    time->timeLeft -= 0.05;
    time->timerSize.y = time->timeLeft/3;
    if (time->timeLeft >= FRAME_RATE) {
        time->timeLeft = FRAME_RATE;
    }

}
void ResetTimer(struct Time* time) {
    time->timeLeft += time->addTime;
}

//HUD functions
void InitScore(struct HUD* hud) {
    hud->scorePos.x = CELL_COUNT*CELL_SIZE - CELL_SIZE * 5;
    hud->scorePos.y = CELL_SIZE/4;
    hud->scoreSize = CELL_SIZE;
    hud->scoreBackgndPos.y = CELL_SIZE/4 - 5;
}
void AddScore(struct Player* player) {
    player->score += 100;
}
void CalcScorePos(struct Player* player, struct HUD* hud) {
// calculating position of text
    if (player->score < 100) {
        hud->scorePos.x = CELL_COUNT*CELL_SIZE - CELL_SIZE * 5;
        hud->scoreLength = CELL_SIZE * 4.5;
    }
    else if (player->score >= 100 && player->score < 1000) {
        hud->scorePos.x = CELL_COUNT*CELL_SIZE - CELL_SIZE * 6;
        hud->scoreLength = CELL_SIZE * 5.8;
    }
    else if (player->score >= 1000 && player->score < 10000) {
        hud->scoreLength = CELL_SIZE * 6;
    }

    // calculating position of background rectangle
    hud->scoreBackgndPos.x = hud->scorePos.x - CELL_SIZE / 6;
    hud->scoreBackgndPos.y = hud->scorePos.y - CELL_SIZE / 6;
}
void DrawScore(struct Player* player, struct HUD* hud) {
    DrawRectangle(hud->scoreBackgndPos.x, 10, hud->scoreLength, hud->scoreSize, blue);
    DrawText (TextFormat("Score: %i", player->score), hud->scorePos.x, hud->scorePos.y, hud->scoreSize, green);
}
void DrawTime (struct Time* time) {
    DrawRectangle( time->timerPos.x , time->timerPos.y , time->timerSize.y , time->timerSize.x, red);
}

// Maze Functions
void SetOrigin(struct Maze* maze, int x, int y, struct Node* node) {
    maze->origin = (Vector2) {(float) x, (float) y};
    SetNodeDirection(node, 0, 0 );
}
void SetNextOrigin(struct Maze* maze, int x, int y) {
    maze->nextOrigin = (Vector2) {(float) x, (float) y};
}
void SetVisibleOrigin(struct Maze* maze, int x, int y) {
    maze->visibleOrigin = (Vector2) {(float) x, (float) y};
}
void SwitchMap(struct Maze* maze) {
    if (maze->isMapA == true) {
        maze->isMapA = false;
        maze->map = maze->mapA;
        maze->visibleMap = maze->mapB;
        maze->iterations = 0;
        SetVisibleOrigin(maze, maze->origin.x, maze->origin.y);

    }
    else if (maze->isMapA == false) {
        maze->isMapA = true;
        maze->map = maze->mapB;
        maze->visibleMap = maze->mapA;
        maze->iterations = 0;
        SetVisibleOrigin(maze, maze->origin.x, maze->origin.y);
    }


};
void Shift(struct Maze* maze){
    int randIndex = (rand() % 4);
    Vector2 direction;

    //-> Select random direction and set it as the provisional direction of the origin node
    switch (randIndex) {
        case 0: {
            direction = UP;
            break;
        }
        case 1: {
            direction = DOWN;
            break;
        }
        case 2: {
            direction = LEFT;
            break;
        }
        case 3: {
            direction = RIGHT;
            break;
        }
        default: {
            direction = UP; // This case should NOT happen. If so, Math broken
            break;
        }
    }

    SetNextOrigin(maze, (int)(maze->origin.x + direction.x), (int)(maze->origin.y + direction.y));
    //ensure direction doesn't point out of bounds
    if (!((maze->origin.x + direction.x) >= maze->width)) {
        if (!((maze->origin.y + direction.y) >= maze->height)) {
            if (!((maze->origin.x + direction.x) < 0)) {
                if (!((maze->origin.y + direction.y) < 0)) {
                    {
                        SetNodeDirection(&maze->map[(int) maze->origin.x][(int) maze->origin.y], (int) direction.x, (int) direction.y);
                         // set new origin position and remove its direction
                        SetOrigin(maze, ((int) maze->nextOrigin.x), ((int) maze->nextOrigin.y), &(maze->map[(int) maze->nextOrigin.x][(int) maze->nextOrigin.y]));
                        }
                    }
                }
            }
        }

}
void InitMaze(struct Maze* maze, int width, int height) {
    maze->width      = width;
    maze->height     = height;
    maze->origin     = (Vector2) {(float)(width - 1) ,(float)(height - 1)};
    maze->nextOrigin = (Vector2) {0,0};
    maze->isMapA = true; //starting the map present in mapA
    maze->visibleMap = maze->mapA;
    maze->map = maze->mapA;
    maze->isGenerating = false;

    maze->iterations = 0;

    //initialise first perfect maze
    for (int y = 0; y < width; y++) {
        for (int x=0; x<(height - 1); x++) {
            maze->mapA[x][y] = (struct Node){ .position.x=(float)x, .position.y=(float)y, .direction.x=1, .direction.y=0 };
        }
        maze->mapA[width - 1][y] =
            (struct Node){ .position.x=(float)(width - 1), .position.y= (float) y, .direction.x=0, .direction.y=1 };
    }
    SetOrigin(maze, width - 1, height - 1, &(maze->mapA[height-1][height-1]));

    for (int i = 0; i < (CELL_COUNT*CELL_COUNT*100); i++) {
        Shift(maze);
    }
    SetVisibleOrigin(maze, maze->origin.x, maze->origin.y);
    maze->map = maze->mapB;

    //initialise second perfect maze
    for (int y = 0; y < width; y++) {
        for (int x=0; x<(height - 1); x++) {
            maze->mapB[x][y] = (struct Node){ .position.x=(float)x, .position.y=(float)y, .direction.x=1, .direction.y=0 };
        }
        maze->mapB[width - 1][y] =
            (struct Node){ .position.x=(float)(width - 1), .position.y= (float) y, .direction.x=0, .direction.y=1 };
    }
    SetOrigin(maze, width - 1, height - 1, &(maze->mapB[height-1][height-1]));

}
void DrawMap(struct Maze* maze) {
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
                DrawNode(&(maze->visibleMap[x][y]));
        }
    }
}
void SwitchOnCommand(struct Maze* maze, struct Player* player) {
    if (IsKeyPressed(KEY_SPACE)) {
        SwitchMap(maze);
        AddScore(player);
    }
}
void PrepNextMap(struct Maze* maze) {
    if (maze->iterations<CELL_COUNT*CELL_COUNT*CELL_COUNT) {
        Shift(maze);
        maze->iterations++;
        maze->isGenerating = true;
    }
    else {
        maze->isGenerating = false;
    }
}
void RenderText(struct Player* player, struct Maze* maze, struct HUD* hud, struct Time* time) {
    if (maze->isGenerating == true) {
        DrawText(TextFormat("GENERATING. . ."), 5, CELL_COUNT*CELL_SIZE - 25, 20, green);
    }
    DrawScore(player, hud);
    DrawTime(time);
}

//Player Functions
void InitPlayer(struct Player* player) {
    player->position = (Vector2) { 0, 0};
    player->canMoveRight = false;
    player->canMoveLeft  = false;
    player->canMoveUp    = false;
    player->canMoveDown  = false;
    player->score        = 0;
}
void DrawPlayer(struct Player* player) {
    DrawCircle((CELL_SIZE*(((((int)player->position.x) * 2)  + 1) / 2)) + (CELL_SIZE/2) , (CELL_SIZE*(((((int)player->position.y) * 2)  + 1) / 2)) + 20, CELL_SIZE/ 3, green);
};
void CheckWalls(struct Maze* maze, struct Player* player) {
    //check movement right
    if (maze->map == maze->mapA) {
        //printf("Checking Walls A\n");
    }
    if (maze->map == maze->mapB) {
        //printf("Checking Walls B\n");
    }

    if   ((maze->visibleMap[ (int)player->position.x]      [(int) player->position.y].direction.x ==  1)  ||
        (((maze->visibleMap[((int)player->position.x)]     [(int) player->position.y].direction.y != 0)   ||
          (maze->visibleMap[((int)player->position.x)]     [(int) player->position.y].direction.x == -1)) &&
          (maze->visibleMap[((int)player->position.x + 1)] [(int) player->position.y].direction.x == -1)))
        {
        player->canMoveRight = true;
        }
    else
        {
        player->canMoveRight = false;
        }

    //check movement left
    if   ((maze->visibleMap[ (int)player->position.x]      [(int) player->position.y].direction.x ==  -1) ||
        (((maze->visibleMap[((int)player->position.x)]     [(int) player->position.y].direction.y != 0)   ||
          (maze->visibleMap[((int)player->position.x)]     [(int) player->position.y].direction.x == 1))  &&
          (maze->visibleMap[((int)player->position.x - 1)] [(int) player->position.y].direction.x == 1)))
    {
        player->canMoveLeft = true;
    }
    else
    {
        player->canMoveLeft = false;
    }

    //check movement down
    if   (maze->visibleMap[(int)player->position.x][(int)player->position.y].direction.y == 1    ||
        ((maze->visibleMap[(int)player->position.x][(int)player->position.y].direction.x != 0    ||
          maze->visibleMap[(int)player->position.x][(int)player->position.y].direction.y == -1 ) &&
          maze->visibleMap[(int)player->position.x][(int)player->position.y + 1].direction.y == -1))
    {
        player->canMoveDown = true;
    }
    else
    {
        player->canMoveDown = false;
    }

    //check movement up

    if   (maze->visibleMap[(int)player->position.x][(int)player->position.y].direction.y == -1  ||
        ((maze->visibleMap[(int)player->position.x][(int)player->position.y].direction.x != 0   ||
          maze->visibleMap[(int)player->position.x][(int)player->position.y].direction.y == 1 ) &&
          maze->visibleMap[(int)player->position.x][(int)player->position.y - 1].direction.y == 1))
    {
        player->canMoveUp = true;
    }
    else
    {
        player->canMoveUp = false;
    }
};
void CheckOnOrigin(struct Maze* maze, struct Player* player, struct Time* time) {
    if (player->position.x == maze->visibleOrigin.x) {
        if (player->position.y == maze->visibleOrigin.y) {
                SwitchMap(maze);
                AddScore(player);
                ResetTimer(time);
        }
    }
};
void MovePlayer(struct Player* player) {
    if      (IsKeyPressed(KEY_RIGHT) && player->canMoveRight == true) {
        player->position.x += 1;
    }
    else if (IsKeyPressed(KEY_LEFT)  && player->canMoveLeft  == true) {
        player->position.x -= 1;
    }
    else if (IsKeyPressed(KEY_UP)    && player->canMoveUp    == true) {
        player->position.y -= 1;
    }
    else if (IsKeyPressed(KEY_DOWN)  && player->canMoveDown  == true) {
        player->position.y += 1;
    }
};

//main game content
int main() {
    //-> Init random
    srand(time(NULL)); // NOLINT(*-msc51-cpp)

    //opening a window
    InitWindow(CELL_SIZE * CELL_COUNT,CELL_SIZE * CELL_COUNT,"Be aMazed");
    SetTargetFPS(FRAME_RATE);

    // initialise default map
    struct Player player;
    struct Maze maze;
    struct Time time;
    struct HUD hud;
    InitMaze(&maze, CELL_COUNT,CELL_COUNT);
    InitPlayer(&player);
    InitScore(&hud);
    InitTime(&time);

    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {
            //event handling
                //map functions
                    PrepNextMap(&maze);
                    SwitchOnCommand(&maze, &player);

                //player functions
                    CheckOnOrigin(&maze, &player, &time);
                    CheckWalls(&maze, &player);
                    MovePlayer(&player);

                //time function
                    CalcTimer(&time);
                //hud functions
                    CalcScorePos(&player, &hud);

            //drawing updates
            BeginDrawing();
                ClearBackground(white);
                DrawMap(&maze);
                DrawOrigin(&maze);
                DrawPlayer(&player);
                RenderText(&player, &maze, &hud, &time);
            EndDrawing();
    };

    //window closing for end of game
    CloseWindow();
    return 0;
}