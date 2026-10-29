/*******************************************************************************************
 *
 *   raylib [core] example - world screen
 *
 *   Example complexity rating: [★★☆☆] 2/4
 *
 *   Example originally created with raylib 1.3, last time updated with raylib 1.4
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2015-2025 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

const int NUM_FACES = 6;

enum faces
{
    UP_FACE,
    DOWN_FACE,
    RIGHT_FACE,
    LEFT_FACE,
    FRONT_FACE,
    BACK_FACE
};

struct cubie
{
    Vector3 pos;
    Matrix orientation;
    Color stickers[NUM_FACES];
};

struct turn_state
{
    bool is_turning;
    int axis;
    int layer;
    int direction;
    float angle;
};

struct cube
{
    cubie corners[8];
    turn_state turn;
};

void init_cube(cube *cube)
{
    int i = 0;

    const float V[2] = {-0.5f, 0.5f};

    for (int x = 0; x < 2; x++)
        for (int y = 0; y < 2; y++)
            for (int z = 0; z < 2; z++)
            {
                cubie *c = &cube->corners[i++];

                c->pos = (Vector3){V[x], V[y], V[z]};
                c->orientation = MatrixIdentity();

                for (int f = 0; f < NUM_FACES; f++)
                {
                    c->stickers[f] = BLACK;
                }

                c->stickers[x ? RIGHT_FACE : LEFT_FACE] = x ? RED : ORANGE;
                c->stickers[y ? UP_FACE : DOWN_FACE] = y ? WHITE : YELLOW;
                c->stickers[z ? FRONT_FACE : BACK_FACE] = z ? GREEN : BLUE;
            }
}

// cubie corners[8] = {{Vector3{-1.0f, -1.0f, -1.0f}, MatrixIdentity()}, {Vector3{1.0f, -1.0f, -1.0f}, Matrix{}},
//                     {Vector3{-1.0f, 1.0f, -1.0f}, Matrix{}},  {Vector3{1.0f, 1.0f, -1.0f}, Matrix{}},
//                     {Vector3{-1.0f, -1.0f, 1.0f}, Matrix{}},  {Vector3{1.0f, -1.0f, 1.0f}, Matrix{}},
//                     {Vector3{-1.0f, 1.0f, 1.0f}, Matrix{}},   {Vector3{1.0f, 1.0f, 1.0f}, Matrix{}}};

Vector3 draw_cubie(cubie cube)
{
    // Vector2 position = GetWorldToScreen((Vector3){cube.pos.x, cube.pos.y + 2.5f, cube.pos.z}, camera);
    DrawCube(cube.pos, 0.9f, 0.9f, 0.9f, BLACK);
}

static float GetAxisValue(Vector3 v, int axis)
{
    return axis == 0 ? v.x : (axis == 1 ? v.y : v.z);
}

// Offset of each sticker's center and its size, indexed by Face
static void GetStickerGeometry(int face, Vector3 *center, Vector3 *size)
{
    const float off = 0.475f + 0.01f; // half body (0.95/2) + half sticker thickness
    const float thick = 0.02f;
    const float span = 0.8f; // smaller than the face, leaves a black border

    switch (face)
    {
    case RIGHT_FACE:
        *center = {off, 0, 0};
        *size = {thick, span, span};
        break;
    case LEFT_FACE:
        *center = {-off, 0, 0};
        *size = {thick, span, span};
        break;
    case UP_FACE:
        *center = {0, off, 0};
        *size = {span, thick, span};
        break;
    case DOWN_FACE:
        *center = {0, -off, 0};
        *size = {span, thick, span};
        break;
    case FRONT_FACE:
        *center = {0, 0, off};
        *size = {span, span, thick};
        break;
    case BACK_FACE:
        *center = {0, 0, -off};
        *size = {span, span, thick};
        break;
    }
}

void DrawCubie(const cubie *c, const turn_state *turn)
{
    rlPushMatrix();

    // (b) Turn animation: only for cubies in the turning layer.
    // Called first, so it is applied last (swings the piece around the center).
    if (turn->is_turning && GetAxisValue(c->pos, turn->axis) * turn->layer > 0)
    {
        float ax = (turn->axis == 0) ? 1.0f : 0.0f;
        float ay = (turn->axis == 1) ? 1.0f : 0.0f;
        float az = (turn->axis == 2) ? 1.0f : 0.0f;
        rlRotatef(turn->angle * turn->direction, ax, ay, az);
    }

    // (c) Move to the cubie's slot
    rlTranslatef(c->pos.x, c->pos.y, c->pos.z);

    // (d) Apply the piece's own accumulated orientation
    rlMultMatrixf(MatrixToFloat(c->orientation));

    // (e) Black body
    DrawCube((Vector3){0, 0, 0}, 0.95f, 0.95f, 0.95f, BLACK);

    // (f) Stickers
    for (int f = 0; f < NUM_FACES; f++)
    {
        Color col = c->stickers[f];
        if (col.r == 0 && col.g == 0 && col.b == 0)
            continue; // hidden face

        Vector3 center, size;
        GetStickerGeometry(f, &center, &size);
        DrawCube(center, size.x, size.y, size.z, col);
    }

    rlPopMatrix();
}

void DrawCubeModel(const cube *cube_state)
{
    for (int i = 0; i < 8; i++)
        DrawCubie(&cube_state->corners[i], &cube_state->turn);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - world screen");

    // Define the camera to look into our 3d world
    Camera camera = {0};
    camera.position = (Vector3){5.0f, 5.0f, 5.0f}; // Camera position
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};   // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};       // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                           // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;        // Camera projection type

    cube cube_state;
    init_cube(&cube_state);

    Vector3 cubePosition = {0.0f, 0.0f, 0.0f};
    Vector2 cubeScreenPosition = {0.0f, 0.0f};

    // DisableCursor(); // Limit cursor to relative movement inside the window

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        // Calculate cube screen space position (with a little offset to be in top)
        cubeScreenPosition = GetWorldToScreen((Vector3){cubePosition.x, cubePosition.y + 2.5f, cubePosition.z}, camera);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
        // DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

        // DrawGrid(10, 1.0f);
        DrawCubeModel(&cube_state);

        EndMode3D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}