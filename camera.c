#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846f
#define RAD2DEG (180.0f / PI)
#define SENSOR_WIDTH_MM 36.0f   // Full-frame sensor width

// 3D Point or Vector
typedef struct {
    float x;
    float y;
    float z;
} Vector3;

// Virtual Camera
// Coordinate system: left-handed, Y up, Z forward (Unity-like)
typedef struct {
    Vector3 position;      // Camera position
    Vector3 rotation;      // In degrees: x = pitch, y = yaw, z = roll
    float focusDistance;   // Focus distance (lens focus)
    float focalLength;     // Focal length (mm, e.g. 50mm)
    float fov;             // Horizontal field of view (degrees)
} VirtualCamera;

// --- Vector helpers ---

static Vector3 vec3Sub(Vector3 a, Vector3 b) {
    Vector3 r = { a.x - b.x, a.y - b.y, a.z - b.z };
    return r;
}

static float vec3Length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static Vector3 vec3Normalize(Vector3 v) {
    float len = vec3Length(v);
    if (len < 1e-6f) {
        Vector3 zero = { 0.0f, 0.0f, 0.0f };
        return zero;
    }
    Vector3 r = { v.x / len, v.y / len, v.z / len };
    return r;
}

// Computes the horizontal FOV from the focal length
static float focalLengthToFov(float focalLengthMm) {
    return 2.0f * atanf(SENSOR_WIDTH_MM / (2.0f * focalLengthMm)) * RAD2DEG;
}

// --- Camera logic (calculation only, no printing) ---

// Locks the camera onto a target: updates focus distance, rotation and FOV.
// Returns 1 on success, 0 if the target is at the camera position.
int updateCameraFocusAndDirection(VirtualCamera *cam, Vector3 target) {
    Vector3 direction = vec3Sub(target, cam->position);
    float distance = vec3Length(direction);

    if (distance < 1e-6f) {
        return 0; // Target and camera coincide, direction is undefined
    }

    Vector3 forward = vec3Normalize(direction);

    cam->focusDistance = distance;
    cam->rotation.y = atan2f(forward.x, forward.z) * RAD2DEG;  // yaw
    cam->rotation.x = -asinf(forward.y) * RAD2DEG;             // pitch (looking down is positive)
    cam->rotation.z = 0.0f;                                    // roll
    cam->fov = focalLengthToFov(cam->focalLength);

    return 1;
}

// --- Logging ---

static void printCameraState(const VirtualCamera *cam, Vector3 target) {
    Vector3 dir = vec3Normalize(vec3Sub(target, cam->position));

    printf("--- VIRTUAL CAMERA UPDATED ---\n");
    printf("Camera Position   : (%.1f, %.1f, %.1f)\n", cam->position.x, cam->position.y, cam->position.z);
    printf("Target Position   : (%.1f, %.1f, %.1f)\n", target.x, target.y, target.z);
    printf("Look-At (normal.) : (%.3f, %.3f, %.3f)\n", dir.x, dir.y, dir.z);
    printf("Rotation (P/Y/R)  : (%.2f, %.2f, %.2f) degrees\n", cam->rotation.x, cam->rotation.y, cam->rotation.z);
    printf("Focus Distance    : %.2f units\n", cam->focusDistance);
    printf("Lens              : %.0fmm (FOV: %.1f degrees)\n", cam->focalLength, cam->fov);
    printf("------------------------------\n\n");
}

int main(void) {
    // 1. Define a virtual camera (Position: X:0, Y:1.5, Z:-5, Lens: 85mm)
    VirtualCamera myVirtualCam = {
        .position      = { 0.0f, 1.5f, -5.0f }, // 1.5 meters high, 5 meters back
        .rotation      = { 0.0f, 0.0f, 0.0f },
        .focusDistance = 0.0f,
        .focalLength   = 85.0f,                 // 85mm portrait lens
        .fov           = 0.0f
    };

    // 2. Position of the character/target in the scene
    Vector3 actorPosition = { 2.5f, 1.8f, 10.0f };

    // 3. Camera focuses on the target
    printf("Frame 1: Character is standing still\n");
    if (updateCameraFocusAndDirection(&myVirtualCam, actorPosition)) {
        printCameraState(&myVirtualCam, actorPosition);
    }

    // 4. Character moves (camera tracking)
    actorPosition.x += 1.5f;
    actorPosition.z += 3.0f;

    printf("Frame 2: Character moved\n");
    if (updateCameraFocusAndDirection(&myVirtualCam, actorPosition)) {
        printCameraState(&myVirtualCam, actorPosition);
    }

    return 0;
}
