#include <stdio.h>
#include <math.h>

// 3D Point or Vector Structure
typedef struct {
    float x;
    float y;
    float z;
} Vector3;

// Virtual Camera Structure
typedef struct {
    Vector3 position;      // Camera Position
    Vector3 rotation;      // Camera Rotation (in degrees)
    float focusDistance;   // Focus Distance (Lens Focus)
    float focalLength;     // Focal Length (lens in mm, e.g., 50mm)
} VirtualCamera;

// Function to calculate the distance between two points (For Focus Distance)
float calculateDistance(Vector3 point1, Vector3 point2) {
    float dx = point2.x - point1.x;
    float dy = point2.y - point1.y;
    float dz = point2.z - point1.z;
    
    // Euclidean Distance Formula: sqrt(dx^2 + dy^2 + dz^2)
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

// Function that locks the camera onto a target (Look-At Logic)
void updateCameraFocusAndDirection(VirtualCamera *cam, Vector3 targetPosition) {
    // 1. Calculate the direction vector from the camera to the target
    Vector3 direction;
    direction.x = targetPosition.x - cam->position.x;
    direction.y = targetPosition.y - cam->position.y;
    direction.z = targetPosition.z - cam->position.z;

    // 2. Update the auto-focus distance of the camera
    cam->focusDistance = calculateDistance(cam->position, targetPosition);

    // 3. Print the direction vector to the console (Data to be passed to the Render Pipeline)
    printf("--- VIRTUAL CAMERA UPDATED ---\n");
    printf("Camera Position   : (%.1f, %.1f, %.1f)\n", cam->position.x, cam->position.y, cam->position.z);
    printf("Target Position   : (%.1f, %.1f, %.1f)\n", targetPosition.x, targetPosition.y, targetPosition.z);
    printf("Look-At Vector    : (%.1f, %.1f, %.1f)\n", direction.x, direction.y, direction.z);
    printf("Focus Distance    : %.2f units\n", cam->focusDistance);
    printf("Used Lens         : %.0fmm\n", cam->focalLength);
    printf("--------------------------------\n\n");
}

int main() {
    // 1. Define a Virtual Camera (Position: X:0, Y:1.5, Z:-5, Lens: 85mm)
    VirtualCamera myVirtualCam = {
        .position = {0.0f, 1.5f, -5.0f}, // 1.5 meters high, 5 meters back
        .rotation = {0.0f, 0.0f, 0.0f},
        .focusDistance = 0.0f,
        .focalLength = 85.0f             // 85mm Portrait Lens
    };

    // 2. Position of the Character/Target in the Scene
    Vector3 actorPosition = {2.5f, 1.8f, 10.0f};

    // 3. Camera Focuses on the Target
    printf("Frame 1: Character is Standing Still\n");
    updateCameraFocusAndDirection(&myVirtualCam, actorPosition);

    // 4. Character Moves (Camera Tracking Logic)
    actorPosition.x += 1.5f;
    actorPosition.z += 3.0f;

    printf("Frame 2: Character Moved\n");
    updateCameraFocusAndDirection(&myVirtualCam, actorPosition);

    return 0;
}
