#pragma once
#include "../../Interfaces.h"

//入力を受け取って各所で求められたときに必要な形で返す
class Input {
public:
	static std::bitset<256> KeyPushed;
	static std::bitset<256> KeyDown;
	static bool MouseLeftPushed;
	static bool MouseRightPushed;
	static bool MouseLeftDown;
	static bool MouseRightDown;
	static int WheelRotate;
	static int MouseX;
	static int MouseY;
	static void Reset() {
		WheelRotate = 0;
		KeyDown.reset();
		MouseLeftDown = false;
		MouseRightDown = false;
	}
	static void SetMousePos(LPARAM lParam) {
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
	}
	inline DirectX::XMVECTOR GetMoveForAI() {

	}
};