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
Color purple = {190,30, 85, 255};
// defining size of window grid in which the game will operate.
int cellSize = 40;
int cellCount = 20;

// designing cell cutout shapes
class cell{
    void right(int x,int y) {
        DrawRectangle((x*cellSize+2.5), (y*cellSize)+2.5, cellSize-2.5, cellSize-5, white);
    }
};

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

    node(int posX, int posY, int directionX=0, int directionY=0) : direction{directionX,directionY} {}

    void setDirection(int newDirX, int newDirY) {
        direction.x = newDirX;
        direction.y = newDirY;
    }
    // node drawing function for debugging
    void Draw(int posX, int posY) {


        if (direction.x == 0) {
            if (direction.y == -1) {
                //upward node
                DrawCircle((cellSize*posX)+cellSize/2, (cellSize*posY)+cellSize/2, cellSize/3, green);
            }
            else if (direction.y == 1) {
                //downward node
                DrawCircle((cellSize*posX)+cellSize/2, (cellSize*posY)+cellSize/2, cellSize/3, red);

            }
        }
        else if (direction.y == 0) {
            if (direction.x == -1) {
                //left-facing node
                DrawCircle((cellSize*posX)+cellSize/2, (cellSize*posY)+cellSize/2, cellSize/3, white);
            }
            else if (direction.x == 1) {
                //right-facing node
                cell.right();
            }
        }
        else if (direction.x == 0 & direction.y == 0) {
            DrawCircle((cellSize*posX)+cellSize/2, (cellSize*posY)+cellSize/2, cellSize/3, purple);
        }
    }

};

class maze {
public:
    int width, height;
    vector<vector<node>> map;
    Vector2 origin;
    Vector2 nextOrigin;
    vector<Vector2> possibleDirections;

    maze(int width, int height):
        width(width),
        height(height),
        origin(Vector2{(float)(width - 1) , (float)(height - 1)}),
        nextOrigin(Vector2{0,0}){
        possibleDirections = {
            {-1,0},
            {0, -1},
            {1, 0},
            {0, 1}
        };
        map = newMap(width, height);
    }

    // initialise a perfect maze for the algorithm to base off of
    vector<vector<node>> newMap(int width, int height) {
        for (int x=0; x<width; x++) {
            map.push_back(vector<node>());
            for (int y=0; y<(height-1); y++) {
                map[x].push_back(node(x, y, 1, 0));
            }
            map[x].push_back(node(x, height-1, 0, 1));
        }
        map[height-1][width-1].setDirection(0,0);
        return map;
    }

    void setOrigin(int x,int y) {
        origin.x = x;
        origin.y = y;
    }

    void setNextOrigin(int x,int y) {
        nextOrigin.x = x;
        nextOrigin.y = y;
    }

    void drawMap() {
        //drawing nodes for debugging

        for (int x=0; x<width; x++) {
            for (int y=0; y<height; y++) {
                map[y][x].Draw(x,y);
            }
        }
    }

};

// call classes
player player;
maze maze(cellCount, cellCount);
bool wasMazeInitialised = false;



int main() {
    //opening a window
    cout << "initialising game..\nglhf" << endl;
    InitWindow(cellSize*cellCount,cellSize*cellCount,"Be aMazed");
    SetTargetFPS(69);
    // initialise default map
    maze.newMap(cellCount,cellCount);

    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {

        //event handling
        player.Move();
        //updating positions

        // create a function called origin.Shift() that would be cool

        //drawing updates

    BeginDrawing();

        maze.drawMap();
        ClearBackground(BLACK);

    EndDrawing();


    };
    //window closing for end of game
    CloseWindow();
    return 0;
}