#include <time.h>
#include <stdlib.h>
#include <raylib.h>

#define CELL_SIZE  40
#define CELL_COUNT 20 // MUST ALWAYS BE EVEN NUMBER!!!!!!!!!!

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
// defining size of window grid in which the game will operate.

// designing cell cutout shapes

struct Player {
    Vector2 position;
};

void MovePlayer(struct Player* player) {
    if (IsKeyPressed(KEY_RIGHT)) {
        player->position.x += 2;
    }
    else if (IsKeyPressed(KEY_LEFT)) {
        player->position.x -= 2;
    }
    else if (IsKeyPressed(KEY_UP)) {
        player->position.y -= 2;
    }
    else if (IsKeyPressed(KEY_DOWN)) {
        player->position.y += 2;
    }
};

void DrawPlayer(struct Player* player) {
    DrawCircle(player->position.x*CELL_COUNT, player->position.y*CELL_COUNT, CELL_SIZE, white);

};

void InitializePlayer(struct Player* player) {
    player->position = (Vector2) { 1, 1};
}


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

// Node Def
void SetNodeDirection(struct Node* node, int x, int y) {
    node->direction = (Vector2) {(float) x, (float) y};
}
void DrawNode(struct Node* node) {
    if (node->direction.x == 0) {
        if (node->direction.y == -1) {
            //upward node
            DrawCircle((CELL_SIZE * node->position.x) + CELL_SIZE / 2,
                (CELL_SIZE * node->position.y) + CELL_SIZE / 2,
                CELL_SIZE / 3, green
                );
        }
        else if (node->direction.y == 1) {
            //downward node
            DrawCircle((CELL_SIZE*node->position.x)+CELL_SIZE/2,
                (CELL_SIZE*node->position.y)+CELL_SIZE/2,
                CELL_SIZE/3, red);
        }
    }
    else if (node->direction.y == 0) {
        if (node->direction.x == -1) {
            //left-facing node
            DrawCircle((CELL_SIZE * node->position.x) + CELL_SIZE / 2,
                (CELL_SIZE * node->position.y) + CELL_SIZE / 2,
                CELL_SIZE / 3, white);
        }
        else if (node->direction.x == 1) {
            DrawCircle((CELL_SIZE * node->position.x) + CELL_SIZE / 2,
                (CELL_SIZE * node->position.y) + CELL_SIZE / 2,
                CELL_SIZE / 3, darkGreen);
        }
    }
    else if (node->direction.x == 0 & node->direction.y == 0) {
        DrawCircle((CELL_SIZE * node->position.x)+CELL_SIZE/2,
            (CELL_SIZE * node->position.y) + CELL_SIZE / 2,
            CELL_SIZE / 3, purple);
    }
}

// Maze Functions
void SetOrigin(struct Maze* maze, int x, int y) {
    maze->origin = (Vector2) {(float) x, (float) y};
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

    SetNodeDirection(&(maze->map[height-1][width-1]), 0, 0);
    SetOrigin(maze, width - 1, height - 1);
}

void DrawMap(struct Maze* maze) {
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            DrawNode(&(maze->map[x][y]));
        }
    }
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
    InitializePlayer(&player);

    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {
        //event handling
        MovePlayer(&player);

        //-> Step
        {
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

            SetNextOrigin(&maze, (int)(maze.nextOrigin.x + direction.x), (int)(maze.nextOrigin.y + direction.y));


            //ensure direction doesn't point out of bounds
            if (maze.nextOrigin.x > maze.width || maze.nextOrigin.y > maze.height || maze.nextOrigin.x < 0 || maze.nextOrigin.y < 0) {

            }
            else {
                SetNodeDirection(&(maze.map[(int) maze.origin.x][(int) maze.origin.y]), (int) direction.x, (int) direction.y);
            }
        }

        //updating positions

        // create a function called origin.Shift() that would be cool

        //drawing updates

        BeginDrawing();
            DrawMap(&maze);
        ClearBackground(BLACK);

        EndDrawing();
    };
    //window closing for end of game
    CloseWindow();
    return 0;
}