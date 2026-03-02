//
// Created by h on 26/02/2026.
//
#include <raylib.h>

class Ball {
    public:
    float x, y;
    float speed, gravity, xSpeed,ySpeed;
    int radius;
    bool touchingGround;

    void draw()
    {
        DrawCircle(x,y,radius,WHITE);
    }
    void isTouchingGround() {
        if (y > 800 - radius-1){
            ySpeed=0;
            touchingGround = true;
        }
        else {
            ySpeed=gravity;
            touchingGround = false;
        }

    }

    void jump() {
        if (IsKeyDown(KEY_UP)==true) {
            ySpeed=-speed;
        }
    }

    void teleport(){
        if (x<0) {
            x=1200;
        }
        else if (x>1200) {
            x=0;

        }
        if (y<0) {
            y=800;
        }

        else if (y>800) {
            y=0;
        }
    };
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
    ball.gravity=7
    ;


    Color green {20,169,133,255};

    //opening a window
    InitWindow(screenWidth,screenHeight,"My First Raylib Game");
    SetTargetFPS(69);


    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {

        //event handling

        ball.teleport();
        ball.isTouchingGround();
        ball.jump();
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