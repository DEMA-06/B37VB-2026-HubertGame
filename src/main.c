#include <time.h>
#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>

#define CELL_SIZE  60
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
    bool alive;
    Vector2 menuPos;
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
    time->addTime = FRAME_RATE / 8;
    time->timerPos.x = CELL_SIZE/4;
    time->timerPos.y = CELL_SIZE/4;
    time->timerSize.x = CELL_SIZE;
    time->timerSize.y = time->timeLeft/3;
};
void CalcTimer(struct Time* time) {
    time->timeLeft -= 0.035;
    time->timerSize.y = time->timeLeft/3;
    if (time->timeLeft >= FRAME_RATE) {
        time->timeLeft = FRAME_RATE;
    }

}
void ResetTimer(struct Time* time) {
    time->timeLeft += time->addTime;
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
void InitMaze(struct Maze* maze,struct HUD* hud, int width, int height) {
    maze->width      = width;
    maze->height     = height;
    maze->origin     = (Vector2) {(float)(width - 1) ,(float)(height - 1)};
    maze->nextOrigin = (Vector2) {0,0};
    maze->isMapA = true; //starting the map present in mapA
    maze->visibleMap = maze->mapA;
    maze->map = maze->mapA;
    maze->isGenerating = false;
    maze->iterations = 0;
    hud->alive = true;

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


//HUD functions
void InitScore(struct HUD* hud) {
    hud->scorePos.x = CELL_COUNT*CELL_SIZE - CELL_SIZE * 5;
    hud->scorePos.y = CELL_SIZE/4;
    hud->scoreSize = CELL_SIZE;
    hud->scoreBackgndPos.y = CELL_SIZE/4 - 5;
    hud->menuPos.x = CELL_COUNT*CELL_SIZE/6;
    hud->menuPos.y = CELL_COUNT*CELL_SIZE/4;
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
void DrawMenu (struct HUD* hud, struct Player* player) {
    DrawRectangle(hud->menuPos.x, hud->menuPos.y, CELL_SIZE*10, CELL_SIZE*6, darkGreen);
    DrawText(TextFormat("You Died! Good try though, you got"), hud->menuPos.x + CELL_SIZE/2, hud->menuPos.y + CELL_SIZE/2, 20, WHITE);
    DrawText(TextFormat("%i Points!", player->score), hud->menuPos.x + CELL_SIZE*3, hud->menuPos.y + CELL_SIZE*2, 40, WHITE);
    DrawText(TextFormat("You should gloat to your friends"), hud->menuPos.x + CELL_SIZE/2, hud->menuPos.y + CELL_SIZE*4, 20, WHITE);
    DrawText(TextFormat("Press Space to restart"), hud->menuPos.x + CELL_SIZE*3, hud->menuPos.y + CELL_SIZE*5, 15, WHITE);
};
void CheckAlive(struct Time* time, struct HUD* hud) {
    if (time->timeLeft <= 0) {
        hud->alive = false;
    }
}
void Restart(struct Player* player, struct HUD* hud, struct Time* time) {
    if (IsKeyPressed(KEY_SPACE))
    {
        player->score = 0;
        player->position.x = 0;
        player->position.y = 0;
        hud->alive = true;
        time->timeLeft = FRAME_RATE;
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
    DrawCircle((CELL_SIZE*(((((int)player->position.x) * 2)  + 1) / 2)) + (CELL_SIZE/2) , (CELL_SIZE*(((((int)player->position.y) * 2)  + 1) / 2)) + (CELL_SIZE/2), CELL_SIZE/ 3, green);
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
        {Introduction
Early on, brainstorming went through a few concepts, with things such as a basic buggy building game, or something along the lines of a rhythm game like Guitar hero – but upon realising that synchronising everything to music and having a scrolling map to design and implement would simply be too complex – a basic maze runner game was settled on.
Theory
My concept phase began with a very basic thought of how to generate a maze. With the initial concept being a scrolling game that had an entry at the bottom, and an exit at the top – with the intention of travelling between „Sections” of the maze infinitely. I was planning on achieving this with some sort of random walk that generates the initial path between the start and finish points, with smaller random walks that branch out every few steps that the walk makes. I later realised that this would end up with quite plain looking mazes that were easy to decode, so I decided to do some research on maze generation algorithms.
When researching, I realised that maze generation was less about cells and walls and more about nodes with directions, which piqued my interest in the topic and got me excited about what method of bringing this into a practical game. A term i started coming across very early on was the term „Perfect Maze” which refers to a maze in which any two points in the maze are connected by no more than one path, this meaning the maze is devoid of any inaccesible isolated areas, or any loops that branch out and connect back in. I immedeatly opted to find some sort of algorithm that enabled this, as that would naturally make for a more interesting game.
After looking through a wide range of algorithms, the one I settled on was Origin Shift. I chose this because it guarantees a perfect maze regardless of how long the maze cycle is run, it is fairly simple to implement in code due to fewer steps as opposed to something like the „Prim’s Maze” algorithm, and is more efficient than that of „Aldus/Broder”.
To set up this algorithm, you must start with a perfect maze, regardless of how simple it is, and as such a very basic arrangement like in Fig.1 can be used.

Figure 1
 From this point on, rather than conceptualising a mase as a series of walkways seperated by walls, it’s more practical to see them as a series of nodes with connections that draw the possible paths. In origin shift, each node has a direction that then dictates what node it’s connected to. If we map this to the previous arrangement, then every row is pointing left, and the final column points down – leading to an array of notes that all point to a root node, referred to as the „origin”. Origin shift is based around manipulating the position of the origin over the course of three recurring steps.
 The first step is to choose a random direction from the origin, ensuring it points within the bounds of the map. The node in that direction becomes the new origin, and the new origin gets stripped of its direction. These steps are iterated over and over, as the origin performs a random walk across the map, breaking down pathways that all end up leading to it somehow, since any node has either one or no direction, it is impossible for a loop to occur.
 The initial concept for this game was to give the player 3 lives and a set amount of time per life, to go around the maze to reach the origin, with the map shifting every time the player reached that goal, and the player being awarded points for it, and the map shifting if they ran out of time, reducing how many lives the player had left, however during the programming phase of the project I shifted to a concept more centered around having one life, with a timer that constantly counts down and gets partially replenished when the origin is reached. This concept seemed significantly more fun, with its constantly reducing timer providing a sense of tension and urgency with gameplay, all the while making it less clunky to play.
Experimental Method
 The first task that I had to undertake was to lay out a grid of nodes to work with. Using the Raylib C library, I opted for a game window that was built relative to the size of the map, for the sake of simplicity. An initial arbitrary size of 20 by 20 cells, with each cells being 40 pixels wide and tall. Setting everything out with equal heights and widths made things much easier to work with.
 From this grid of nodes I initially opted to represent them as circles, with different colours representing what direction they had (fig. 2 & fig 3) – as i felt that would be simpler and quicker to implement to debug whether the algorithm was working correctly or not – with better visual representation later. (The white square in fig.3 was the displaced origin)

Once the alrgoithm functioned correctly (this took some time, as keeping the origin from wandering off the screen seemed to be a bit more of a task than initially anticipated), I made the representation of the maze more readable, by imposing white rectangles onto a black background (fig 4), and slightly elongating, or shortening, them in the respective axis that they were pointing.
This was a simple and effective method of displaying the actual maze mapping, and the only changes that were made to that implementation were that I decided to reverse the colours (to black squares on a white background) as I liked the „Pong” era aesthetic, as well as adjusting the sizes of the rectangles on the borders to make borders around the edges of the map more even (fig. 5).
Enabling player movement was also quite a challenge, as I had to figure out the logic that it would take to prevent the player from moving through walls. The creation of this feature was quite perplexing, as my initial implementation of player movement was based on a different co-ordinate system from the one used to draw the map. This resulted in the player being a growing power of 2 grid squares away from where the actual „hitbox” was being registered.

After realising that this was the issue, however, the player drawing mechanics were thrown completely off balance – as my method to draw the player was much more rudimental. In the end – I determined that this was the equation that was needed to draw the player square accurately on all axes to maintain balance with the hitbox.
x=(C (2P+1)/2)+ C/2
Where x is the respective x (or y, this equation is used in both axes) co-ordinate (in pixels), C is the cell size (in pixels), and P is the grid position of the player. The C/2 is a half cell offset, since the circle is drawn from the centre of the cell, whereas the rest of the maths was determined by dividing the desired pixel position by the cell size for multiple results (20px, 60px, 100 px, etc.) and finding the correlating factor. This equation allows for use regardless of the size of the cells, nor their quantity.
The wall checking logic was quite fun to develop as well. The fact that each cell had a direction made for quite a straightforward set of rules to apply for each movement axis. To be able to implement this I also had to introduce a new value to the player model – the canMove[direction] boolean that would be constantly off unless logic permitted it to. The hitbox logic functions as fig.6 and fig. 7 below represent (these examplse are specifically for movement in the right direction, there are adjustments to the variables used in different directions).

The initial design of this program was intended to have a fixed blocking loop of roughly cell-count cubed iterations (with 20 cells that would be 8000 iterations of the origin shift algorithm, i found cell-count cubed was the most reliable amount of calculations to fully shuffle a maze) every time the player reached the origin. This proved to work well as a simple implementation, however when i took the code from my pc and tried it on my laptop, the program would hang every time the loop was called. I believe this was something to do with my laptop processor being weaker and the calculation taking long enough for my operating system to consider the process as „not responding” due to the lack of updates in the program.
To work around this, I decided to try and design a non blocking generation function. I achieved this by having a map switching function (fig. 8). At all times there are two maps, the visible map, that is used to read hitbox positions and the visible origin position, and an invisible one that is being generated constantly in the background. This allows for a function that constantly runs in parallel to the normal operation of the game and does not block it. To indicate background generation I made a function that displated text on screen, visible to the player. Due to the great amount of calculations that are needed to be performed, I increased the frame rate at which the game ran to 1000 frames per second, up from 60. Since 8000~ calculations were expected that would save the amount of time needed to calculate the new map. This number wasn’t raised to 8000+ because I did not want to overload weaker processors, but with the 1000fps it seemed to work on every machine I tested on (I tested on around 5 machines of varying CPU, operating system, and hardware).

Finally I needed to implement a timer function and score, as well as a „death” mechanic. Score was easy to implement, as I just needed to add another variable and add to it every time „CheckOnOrigin” returns true. I initially wanted the timer to be numerical, with fractions of a second counting down – but the time libraries seemed to be clunky to use and I wanted something easier to implement. Thus I opted to simply have a rectangle of some sort of width, that would reduce in width as the game ran (as featured. This ended up being extremely simple and a much more engaging visual representation of the remaining time, seeing the bar refill and drain with every map refresh ended up being very exciting.
Results
 This was a very exciting and fun project to do – I feel like I learned a lot about C and C++(I made a lot of the early code in the wrong language by mistake). I shared my game with a few friends and a lot of them found the game fun, but quite difficult – so I reduced the size of the map.
 There is a bug that ended up being part of the final game, but I reckoned that it was insignificant enough to ignore – as it would take more time to fix the issue than it would be worth. This bug involved the map switching function. When the maps switched before the generation went once over fully (or at least if the origin hadn’t gone back over itself at least once) then a white square would appear. Technically it does not affect gameplay, as that square does not prevent the maze from being perfect, and it is impossible to enter the square and „get stuck”. Thus I left the bug in due to the tradeoff of time saved.
 Another few improvements I would have made would be HUD related. I found a lot of the time that when the route necessary went under HUD elements, it was difficult to discern whether or not there was a wall in place of the HUD. A function to shift the hud to the bottom of the screen whenever the player was underneath HUD elements was attempted, but it ended up running over my deadline for when to complete the game. Another similar function would be to simply reduce the opacity of those elements whenever the player was underneath could be another way to improve such issues.
 Conclusions
              In conclusion, I found this to be a very fun project and a great way to get my mind back into programming after a while of being away from the subject. A thing that this project helped me realise was the difficulty of accrately balancing games. Balancing map size, timer amount, and amount of time granted per map completion were all a thing that required a careful balance that I remain unsure as to whether or not i achieved. The importance of avoiding magic numbers was also quite important – but I believe that I could have put more effort in with that, and it’s something I will actively work on in future projects.

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
    InitMaze(&maze, &hud, CELL_COUNT,CELL_COUNT);
    InitPlayer(&player);
    InitScore(&hud);
    InitTime(&time);

    //Game loop - will run indefinitely until variable changes
    while (WindowShouldClose() == false) {

        if (hud.alive == true) {
            //event handling
                //map functions
                    PrepNextMap(&maze);

                //player functions
                    CheckOnOrigin(&maze, &player, &time);
                    CheckWalls(&maze, &player);
                    MovePlayer(&player);

                //time function
                    CalcTimer(&time);
                //hud functions
                    CalcScorePos(&player, &hud);
                    CheckAlive(&time,&hud);

            //drawing updates
                BeginDrawing();
                    ClearBackground(white);
                    DrawMap(&maze);
                    DrawOrigin(&maze);
                    DrawPlayer(&player);
                    RenderText(&player, &maze, &hud, &time);
                EndDrawing();
        }
        else if (hud.alive == false) {
            BeginDrawing();
                ClearBackground(BLACK);
                DrawMenu(&hud, &player);
            EndDrawing();
            Restart(&player, &hud ,&time);
        }
    };

    //window closing for end of game
    CloseWindow();
    return 0;
}