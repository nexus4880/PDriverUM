#include <iostream>
#include <thread>
#include "../TestUserMode/driverinterop.hpp"

struct Vector3 {
	float x, y, z;
};

struct Vector2 {
	float x, y;
};

namespace Offsets {
	int LocalPlayer = 0x0018AC00;
	int PlayerCount = 0x18AC0C;

	int CameraPosition = 0x0004; //0x0004 (Vector3)
	int Position = 0x0028; //0x0028 (Vector3)
	int ViewAngles = 0x0034; //0x0034 (Vector3)
	int IsGrounded = 0x005C; //0x005C (int, take second byte, could read as bool but I think there's more to this value)
	int Health = 0x00EC; //0x00EC (int)
	int Armor = 0x00F0; //0x00F0 (int)
	int Name = 0x205;
};

class Player {
public:
	Player(DriverInterop* pDriver, long long base_player) {
		pDriver->read(base_player, this->m_basePlayer);
		this->m_pDriver = pDriver;
	}

	Vector3 GetPosition() {
		Vector3 result = { 0.f, 0.f,0.f };
		if (this->m_pDriver) {
			if (!this->m_pDriver->read<Vector3>(this->m_basePlayer + Offsets::Position, result)) {
				std::cout << "failed to read GetPosition" << std::endl;
			}
		}

		return result;
	}

	Vector3 GetCameraPosition() {
		Vector3 result = { 0.f, 0.f,0.f };
		if (this->m_pDriver) {
			if (!this->m_pDriver->read<Vector3>(this->m_basePlayer + Offsets::CameraPosition, result)) {
				std::cout << "failed to read GetCameraPosition" << std::endl;
			}
		}

		return result;
	}

	Vector3 GetViewAngles() {
		Vector3 result = { 0.f, 0.f, 0.f };
		if (this->m_pDriver) {
			if (!this->m_pDriver->read<Vector3>(this->m_basePlayer + Offsets::ViewAngles, result)) {
				std::cout << "failed to read GetViewAngles" << std::endl;
			}
		}

		return result;
	}

	bool IsGrounded() {
		bool result = true;
		if (this->m_pDriver) {
			int readNumber = 0;
			if (!this->m_pDriver->read<int>(this->m_basePlayer + Offsets::IsGrounded, readNumber)) {
				std::cout << "failed to read IsGrounded" << std::endl;
			}
			else {
				result = (readNumber >> 8) & 0xff;
			}
		}

		return result;
	}

	int GetHealth() {
		int result = true;
		if (this->m_pDriver) {
			if (!this->m_pDriver->read<int>(this->m_basePlayer + Offsets::Health, result)) {
				std::cout << "failed to read GetHealth" << std::endl;
			}
		}

		return result;
	}

	int GetArmor() {
		int result = true;
		if (this->m_pDriver) {
			if (!this->m_pDriver->read<int>(this->m_basePlayer + Offsets::Armor, result)) {
				std::cout << "failed to read GetArmor" << std::endl;
			}
		}

		return result;
	}

	std::string GetName() {
		std::string result{};
		if (this->m_pDriver) {
			if (!this->m_pDriver->read_with_size(this->m_basePlayer + Offsets::Name, &result, 16)) {
				std::cout << "failed to read GetName" << std::endl;
			}
		}

		return result;
	}
private:
	DriverInterop* m_pDriver;
	long long m_basePlayer;
};

int main()
{
	HANDLE handle = {};
	std::cout << "finding driver";
	while ((handle = CreateFileW(L"\\\\.\\TestDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "\nfound driver\n";
    DriverInterop driver(&handle, L"ac_client.exe");
	const long base_address = driver.get_proc_base_address();
	if (base_address == -1) {
		std::cout << "failed to get base address";
		return -1;
	}

	while (!(GetAsyncKeyState(VK_INSERT) & 1) && driver.is_valid()) {
		int playerCount = 0;
		if (driver.read<int>(base_address + Offsets::PlayerCount, playerCount)) {
			printf_s("%i players\n", playerCount);
		}
		else {
			printf_s("failed to read PlayerCount\n");
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}