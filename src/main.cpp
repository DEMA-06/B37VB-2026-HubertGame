//
// Created by h on 26/02/2026.
//
#include <iostream>
#include <ostream>
#include <raylib.h>
#include <vector>

using namespace std;

// initialise colour presets
Color green = {173,204,96,255};
Color darkGreen = {43, 51, 24, 255};
Color blue = {43,22,133,255};
Color white = {255,255,255,255};
Color red = {190,30,40,255};

// defining size of window grid in which the game will operate.
int cellSize = 40;
int cellCount = 20;

class player {
    public:

    float playerX, playerY;
    Vector2 position = {1, 1};

    void Move() {
        if (IsKeyPressed(KEY_RIGHT)) {
            position.x += 2;
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            position.x -= 2;
        }
        else if (IsKeyPressed(KEY_UP)) {
            position.y -= 2;
        }
        else if (IsKeyPressed(KEY_DOWN)) {
            position.y += 2;
        }
    }
    void Draw()
    {
        DrawCircle(position.x*cellCount, position.y*cellCount, cellSize/3, white);
    };

};

class node {
    public:
    struct Direction {
        int x, y;
    } direction;

    node(int directionX=0, int directionY=0) : direction{directionX,directionY} {}

    void setDirection(int newDirX, int newDirY) {
        direction.x = newDirX;
        direction.y = newDirY;
    }
    // temporary node drawing function for visual debugging
    void Draw(int posX, int posY) {
        DrawCircle(posX+(cellSize/2), posY+(cellSize/2), cellSize/8, red);
    }

};

class maze {

    int width, height;
    vector<vector<node>> map;
    Vector2 origin;
    Vector2 nextOrigin;
    vector<Vector2> possibleDirections;

    vector<vector<node>> newMap() {

    }

public:
    maze(int width, int height)
    :   width(width),
        height(height),
        origin(Vector2{(float)(width - 1) , (float)(height - 1)}),
        nextOrigin(Vector2{0,0})
        {
        possibleDirections = {
            {-1,0},
            {0, -1},
            {1, 0},
            {0, 1}
        };

    }



};

// call classes
player player;
maze maze(cellCount, cellCount);
node node;


int main() {
    //opening a window
    cout << "initialising game..\nglhf" << endl;
    InitWindow(cellSize*cellCount,cellSize*cellCount,"Be aMazed");
    SetTargetFPS(69);


    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {

        //event handling
        //updating positions

        //drawing updates
    BeginDrawing();
        player.Move();
        player.Draw();
        ClearBackground(blue);

    EndDrawing();


    };
    //window closing for end of game
    CloseWindow();
    return 0;
}