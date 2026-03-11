//
// Created by h on 26/02/2026.
//
#include <iostream>
#include <ostream>
#include <raylib.h>

using namespace std;

// initialise colour presets
Color green = {173,204,96,255};
Color darkGreen = {43, 51, 24, 255};
Color blue = {43,22,133,255};
Color white = {255,255,255,255};
Color red = {190,30,40,255};

// defining size of window grid in which the game will operate.
int cellSize = 40;
int cellCount = 10;

class player {
    public:
    Vector2 position = {5,6};

    void Draw()
    {
        DrawCircle(position.x*cellSize, position.y*cellSize, cellSize/2, white);
    };

};
class node {
    public:
    struct Direction {
        int x;
        int y;
    } direction;
    node(int directionX=0, int directionY=0) : direction{directionX,directionY} {};
};

class maze {
    public:

};

// call classes
player player;


int main() {
    //opening a window
    cout << "initialising game..." << endl;
    InitWindow(cellSize*cellCount,cellSize*cellCount,"My First Raylib Game");
    SetTargetFPS(69);


    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {

        //event handling
        //updating positions

        //drawing updates
    BeginDrawing();

        player.Draw();
        ClearBackground(blue);

    EndDrawing();


    };
    //window closing for end of game
    CloseWindow();
    return 0;
}