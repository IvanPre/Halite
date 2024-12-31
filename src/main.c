#include "raylib.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

#include "graph.h"


BallArray createRandomBalls(unsigned int number, float posmin, float posmax, float min_radius, float max_radius){
    if(min_radius < 1.0f) {min_radius = 1.0f;}

    BallArray ball_list = allocateBallArray(number);
    float pos_range = posmax - posmin;
    float rad_range = max_radius - min_radius;

    for(unsigned int i=0; i<number; i++){
        Vector2 ballPosition = (Vector2) {
            randfloat() * pos_range + posmin,
            randfloat() * pos_range + posmin
        };
        Color ballColor = (Color) { GetRandomValue(0,255), GetRandomValue(0,255), GetRandomValue(0,255), 255 };
        float radius = randfloat() * rad_range + min_radius;

        addBallToArray(&ball_list, ballPosition, radius, ballColor);
    }
    return ball_list;
}


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    srand(time(NULL));
    int screenWidth = 800;
    int screenHeight = 450;
    Color ballColor = DARKBLUE;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Halite");

    BallArray ball_list = createRandomBalls(10, 0.0f, 450.0f, 20.0f, 30.0f);


    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        float dt = GetFrameTime();
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();


        // Update
        //----------------------------------------------------------------------------------
        applyScreenBoundaries(ball_list, screenWidth, screenHeight);
        // updateBallPhysics(ball_list, dt, 1000.0f, 20.0f);
        updateBallPhysics(ball_list, dt, 5E-2, 0.995f);
        // updateBallPhysics(ball_list, dt, 1E+4, 0.0f, 1.0f);
        // applyScreenBoundaries(ball_list, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        drawBalls(ball_list);
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization--------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}