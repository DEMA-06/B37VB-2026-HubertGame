//
// Created by h on 26/02/2026.
//
#include <raylib.h>

class Ball {
    public:
    float x, y;
    float speed, xSpeed,ySpeed;
    int radius;

    void draw()
    {
        DrawCircle(x,y,radius,WHITE);
    }
    void direction() {
        if (IsKeyPressed(KEY_UP) && ySpeed==0) {
            xSpeed = 0;
            ySpeed = -speed;
        }

        else if (IsKeyPressed(KEY_DOWN) && ySpeed==0) {
            xSpeed = 0;
            ySpeed = speed;
        }

        else if (IsKeyPressed(KEY_LEFT) && xSpeed==0) {
            xSpeed = -speed;
            ySpeed = 0;
        }

        else if (IsKeyPressed(KEY_RIGHT) && xSpeed==0) {
            xSpeed = speed;
            ySpeed = 0;
        }

    }

    void update() {
        x += xSpeed;
        y += ySpeed;
    }
};


Ball ball;

int main() {
    //defining the position coordinate values of the ball
    const int screenWidth = 1200;
    const int screenHeight = 800;
    ball.radius = 20;
    ball.x= screenWidth/2;
    ball.y= screenHeight/2;
    ball.speed=7;


    Color green {20,169,133,255};

    //opening a window
    InitWindow(screenWidth,screenHeight,"My First Raylib Game");
    SetTargetFPS(69);


    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {

        //event handling
            ball.direction();
        //updating positions

            ball.update();

        //drawing updates
    BeginDrawing();
        ball.draw();
        ClearBackground(green);

    EndDrawing();


    };
    //window closing for end of game
    CloseWindow();
    return 0;
}