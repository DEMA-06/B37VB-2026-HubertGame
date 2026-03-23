#include <time.h>
#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>

#define CELL_SIZE  40
#define CELL_COUNT 35

#define UP    (Vector2) {  0, -1 }
#define DOWN  (Vector2) {  0,  1 }
#define LEFT  (Vector2) { -1,  0 }
#define RIGHT (Vector2) {  1,  0 }

// initialise colour presets
Color green = {173,204,96,255};
Color darkGreen = {43, 51, 24, 255};
Color blue = {43,22,133,255};
Color white = {255,255,255,255};
Color red = {190,30,40,255};
Color purple = {190,30, 85, 255};

struct Player {
    Vector2 position;
    bool canMoveLeft;
    bool canMoveRight;
    bool canMoveUp;
    bool canMoveDown;
    int score;
    int lives;
};
struct Node {
    Vector2 position;
    Vector2 direction;
};
struct Maze {
    int width, height;
    struct Node map[CELL_COUNT][CELL_COUNT];
    Vector2 origin;
    Vector2 nextOrigin;
    Vector2 possibleDirections[];
};


// Node Functions
void SetNodeDirection(struct Node* node, int x, int y) {
    node->direction = (Vector2) {(float) x, (float) y};
}
void DrawNode(struct Node* node) {
    if (node->direction.x == 0) {
        if (node->direction.y == -1) {
            //upward node
            DrawRectangle((CELL_SIZE * node->position.x) , (CELL_SIZE * node->position.y) , CELL_SIZE - 5, CELL_SIZE, white);
        }
        else if (node->direction.y == 1) {
            //downward node
            DrawRectangle((CELL_SIZE * node->position.x) , (CELL_SIZE * node->position.y) + 5, CELL_SIZE - 5, CELL_SIZE, white);
        }
    }
    else if (node->direction.y == 0) {
        if (node->direction.x == -1) {
            //left-facing node
            DrawRectangle((CELL_SIZE * node->position.x) - 10 , (CELL_SIZE * node->position.y) + 5, CELL_SIZE + 5, CELL_SIZE - 5, white);
        }
            //right-facing node
        else if (node->direction.x == 1) {
            DrawRectangle((CELL_SIZE * node->position.x) , (CELL_SIZE * node->position.y) + 5, CELL_SIZE + 5, CELL_SIZE - 5, white );
        }
    }
}
void DrawOrigin(struct Maze* maze) {
            DrawRectangle((CELL_SIZE * maze->origin.x) , (CELL_SIZE * maze->origin.y) + 5, CELL_SIZE - 5, CELL_SIZE - 5, blue);

}
// Maze Functions
void SetOrigin(struct Maze* maze, int x, int y, struct Node* node) {
    maze->origin = (Vector2) {(float) x, (float) y};
    SetNodeDirection(node, 0, 0 );
}
void SetNextOrigin(struct Maze* maze, int x, int y) {
    maze->nextOrigin = (Vector2) {(float) x, (float) y};
}
void InitializeMaze(struct Maze* maze, int width, int height) {
    maze->width      = width;
    maze->height     = height;
    maze->origin     = (Vector2) {(float)(width - 1) ,(float)(height - 1)};
    maze->nextOrigin = (Vector2) {0,0};

    for (int y = 0; y < width; y++) {
        for (int x=0; x<(height - 1); x++) {
            maze->map[x][y] = (struct Node){ .position.x=(float)x, .position.y=(float)y, .direction.x=1, .direction.y=0 };
        }
        maze->map[width - 1][y] =
            (struct Node){ .position.x=(float)(width - 1), .position.y= (float) y, .direction.x=0, .direction.y=1 };
    }
    SetOrigin(maze, width - 1, height - 1, &(maze->map[height-1][height-1]));
}
void DrawMap(struct Maze* maze) {
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            DrawNode(&(maze->map[x][y]));
        }
    }
}
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
                        SetNodeDirection(&(maze->map[(int) maze->origin.x][(int) maze->origin.y]), (int) direction.x, (int) direction.y);

                        // set new origin position and remove its direction
                        SetOrigin(maze, ((int) maze->nextOrigin.x), ((int) maze->nextOrigin.y), &(maze->map[(int) maze->nextOrigin.x][(int) maze->nextOrigin.y]));
                    }
                }
            }
        }
    }
}

//Player Functions
void CheckWalls(struct Maze* maze, struct Player* player) {
    //check movement right
    if ((maze->map[ (int)player->position.x]  [(int) player->position.y].direction.x ==  1) ||
        (((maze->map[((int)player->position.x)] [(int) player->position.y].direction.y != 0) || (maze->map[((int)player->position.x)] [(int) player->position.y].direction.x == -1)) &&
        (maze->map[((int)player->position.x + 1)] [(int) player->position.y].direction.x == -1)))
        {
        player->canMoveRight = true;
        }
    else
        {
        player->canMoveRight = false;
        }

    //check movement left
    if ((maze->map[ (int)player->position.x]  [(int) player->position.y].direction.x ==  -1) ||
        (((maze->map[((int)player->position.x)] [(int) player->position.y].direction.y != 0) || (maze->map[((int)player->position.x)] [(int) player->position.y].direction.x == 1)) &&
        (maze->map[((int)player->position.x - 1)] [(int) player->position.y].direction.x == 1)))
    {
        player->canMoveLeft = true;
    }
    else
    {
        player->canMoveLeft = false;
    }

    //check movement down
    if   (maze->map[(int)player->position.x][(int)player->position.y].direction.y == 1 ||
        ((maze->map[(int)player->position.x][(int)player->position.y].direction.x != 0 ||
          maze->map[(int)player->position.x][(int)player->position.y].direction.y == -1 ) &&
          maze->map[(int)player->position.x][(int)player->position.y + 1].direction.y == -1))
    {
        player->canMoveDown = true;
    }
    else
    {
        player->canMoveDown = false;
    }

    //check movement up

    if   (maze->map[(int)player->position.x][(int)player->position.y].direction.y == -1 ||
        ((maze->map[(int)player->position.x][(int)player->position.y].direction.x != 0 ||
          maze->map[(int)player->position.x][(int)player->position.y].direction.y == 1 ) &&
          maze->map[(int)player->position.x][(int)player->position.y - 1].direction.y == 1))
    {
        player->canMoveUp = true;
    }
    else
    {
        player->canMoveUp = false;
    }
};
void CheckOnOrigin(struct Maze* maze, struct Player* player) {
    if (player->position.x == maze->origin.x) {
        if (player->position.y == maze->origin.y) {
            for (int i; i<(CELL_COUNT*CELL_COUNT*CELL_COUNT); i++) {
                Shift(maze);
            }
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
void DrawPlayer(struct Player* player) {
    DrawCircle((CELL_SIZE*(((((int)player->position.x) * 2)  + 1) / 2)) + 20 , (CELL_SIZE*(((((int)player->position.y) * 2)  + 1) / 2)) + 20, CELL_SIZE/ 3, purple);
};
void InitializePlayer(struct Player* player) {
    player->position = (Vector2) { 0, 0};
    player->canMoveRight = false;
    player->canMoveLeft  = false;
    player->canMoveUp    = false;
    player->canMoveDown  = false;
    player->score        = 0;
    player->lives        = 3;
}

int main() {
    //-> Init random
    srand(time(NULL)); // NOLINT(*-msc51-cpp)

    //opening a window
    InitWindow(CELL_SIZE * CELL_COUNT,CELL_SIZE * CELL_COUNT,"Be aMazed");
    SetTargetFPS(120);

    // initialise default map
    struct Player player;
    struct Maze maze;
    InitializeMaze(&maze, CELL_COUNT,CELL_COUNT);
    for (int i; i<(CELL_COUNT*CELL_COUNT*CELL_COUNT); i++) {
        Shift(&maze);
    }

    InitializePlayer(&player);

    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {
        //event handling
        CheckOnOrigin(&maze, &player);
        CheckWalls(&maze, &player);
        MovePlayer(&player);
        //drawing updates
        BeginDrawing();
            ClearBackground(BLACK);
            DrawMap(&maze);
            DrawOrigin(&maze);
            DrawPlayer(&player);
        EndDrawing();
    };

    //window closing for end of game
    CloseWindow();
    return 0;
}