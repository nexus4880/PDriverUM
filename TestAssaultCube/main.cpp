#include <iostream>
#include <thread>
#include "../TestUserMode/driverinterop.hpp"

struct Vector3 {
    float x, y, z;
};

struct Vector2 {
    float x, y;
};

struct Camera3D {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    float fovy;
    int projection;
};

struct rlFPCamera {
    int ControlsKeys[11];
    Vector3 MoveSpeed;
    Vector2 TurnSpeed;
    bool UseMouse;
    bool InvertY;
    float MouseSensitivity;
    float MinimumViewY;
    float MaximumViewY;
    float ViewBobbleFreq;
    float ViewBobbleMagnatude;
    float ViewBobbleWaverMagnitude;
    Vector3 CameraPosition;
    float PlayerEyesPosition;
    Vector2 FOV;
    float TargetDistance;
    Vector2 ViewAngles;
    float CurrentBobble;
    bool Focused;
    bool AllowFlight;
    Camera3D ViewCamera;
    Vector3 Forward;
    Vector3 Right;
    double NearPlane;
    double FarPlane;
};

int main() {
	HANDLE handle = {};
	std::cout << "finding driver";
	while ((handle = CreateFileW(L"\\\\.\\TestDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "\nfound driver\n";
    DriverInterop driver(&handle, L"CPPRaylib.exe");
    rlFPCamera cam{};
    PTRW base = 115189976;
	while (driver.read<rlFPCamera>(base, cam)) {
        driver.write<float>(base + offsetof(rlFPCamera, PlayerEyesPosition), cam.PlayerEyesPosition + 1.f);
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}

	return 0;
}