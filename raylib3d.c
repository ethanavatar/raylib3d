#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

const int screenWidth = 1000;
const int screenHeight = 600;

const int pixelSize = 2;
const int canvasWidth = screenWidth / pixelSize;
const int canvasHeight = screenHeight / pixelSize;

Camera3D worldCamera = { 0 };

// only allow one box to be grabbed at a time
bool isGrabbing = false;
float grabDistance = 0.0f;

typedef struct {
    Vector3 position;
    BoundingBox bounds;
    bool isLookedAt;
    bool isGrabbed;
} Box;

const int boxCount = 5;
Box boxes[boxCount] = { 0 };

void MoveBox(Box *box, Vector3 position) {
    box->position = position;
    box->bounds.min = (Vector3) {
        box->position.x - 0.5f,
        box->position.y - 0.5f,
        box->position.z - 0.5f
    };
    box->bounds.max = (Vector3) {
        box->position.x + 0.5f,
        box->position.y + 0.5f,
        box->position.z + 0.5f
    };
}

void BoxPhysics(float deltaTime, Ray facingRay) {
    (void) deltaTime; // unused
    for (int i = 0; i < boxCount; i++) {
        Box *box = &boxes[i];
        RayCollision collision = GetRayCollisionBox(facingRay, box->bounds);
        
        box->isLookedAt = false;
        if (!collision.hit) continue;
        box->isLookedAt = true;

        if (!box->isGrabbed && isGrabbing) continue;

        if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            box->isGrabbed = false;
            isGrabbing = false;
            continue;
        }

        if (!box->isGrabbed) {
            box->isGrabbed = true;
            isGrabbing = true;

            grabDistance = Vector3Distance(worldCamera.position, box->position);
        }

        grabDistance += GetMouseWheelMove() * 0.5f;

        Vector3 scaled = Vector3Scale(facingRay.direction, grabDistance);
        Vector3 newPos = Vector3Add(worldCamera.position, scaled);

        MoveBox(box, newPos);
    }
}

int main(void) {

    InitWindow(screenWidth, screenHeight, "Basic Window");
    SetTargetFPS(60);
    DisableCursor();

    // create a render texture to draw to
    RenderTexture2D target = LoadRenderTexture(canvasWidth, canvasHeight);

    worldCamera.position = (Vector3) { 10.0f, 2.0f, 10.0f };
    worldCamera.target = (Vector3) { 0.0f, 0.0f, 0.0f };
    worldCamera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    worldCamera.fovy = 60.0f;
    worldCamera.projection = CAMERA_PERSPECTIVE;

    Camera2D screenCamera = { 0 };
    screenCamera.zoom = 1.0f;

    // flip the image vertically because OpenGL
    Rectangle sourceRec = {
        0.0f,
        0.0f, 
        (float) target.texture.width,
        -(float) target.texture.height
    };

    // scale the image to fit the screen
    Rectangle destRec = {
        -pixelSize,
        -pixelSize,
        screenWidth + (pixelSize * 2),
        screenHeight + (pixelSize * 2)
    };

    // create some boxes
    for (int i = 0; i < boxCount; i++) {
        boxes[i].position = (Vector3) {
            GetRandomValue(-5, 5),
            GetRandomValue(1, 2),
            GetRandomValue(-5, 5)
        };
        boxes[i].bounds.min = (Vector3) {
            boxes[i].position.x - 0.5f,
            boxes[i].position.y - 0.5f,
            boxes[i].position.z - 0.5f
        };
        boxes[i].bounds.max = (Vector3) {
            boxes[i].position.x + 0.5f,
            boxes[i].position.y + 0.5f,
            boxes[i].position.z + 0.5f
        };
        boxes[i].isLookedAt = false;
        boxes[i].isGrabbed = false;
    }

    while (!WindowShouldClose()) {

        float deltaTime = GetFrameTime();
        float deltaTimeMs = deltaTime * 1000.0f;

        const char *title = TextFormat("Basic Window - %.2f ms", deltaTimeMs);
        SetWindowTitle(title);

        UpdateCamera(&worldCamera, CAMERA_FIRST_PERSON);
        
        // Raylib has a builtin function to lock the mouse in place,
        // but It doesnt work, so this is the workaround:
        // setting the mouse position to the center of the screen every frame
        SetMousePosition(screenWidth / 2, screenHeight / 2);

        // Cast a ray from the camera to the mouse position
        Ray facingRay = GetMouseRay(GetMousePosition(), worldCamera);

        BoxPhysics(deltaTime, facingRay);

        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            
            BeginMode3D(worldCamera);
                DrawGrid(10, 1.0f);

                for (int i = 0; i < boxCount; i++) {
                    DrawCube(boxes[i].position, 1.0f, 1.0f, 1.0f, RED);
                    DrawCubeWires(boxes[i].position, 1.0f, 1.0f, 1.0f, MAROON);
                }
            EndMode3D(); 

        EndTextureMode();

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(screenCamera);
                DrawTexturePro(
                    target.texture,
                    sourceRec,
                    destRec,
                    (Vector2) { 0, 0 },
                    0.0f,
                    WHITE
                );

                // Draw a crosshair
                // Vertical line
                DrawRectangle(
                    screenWidth / 2 - 1, screenHeight / 2 - 5,
                    2, 10,
                    BLACK
                );
                
                // Horizontal line
                DrawRectangle(
                    screenWidth / 2 - 5, screenHeight / 2 - 1,
                    10, 2,
                    BLACK
                );

            EndMode2D();

        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
