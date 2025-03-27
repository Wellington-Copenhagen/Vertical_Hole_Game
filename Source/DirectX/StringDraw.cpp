#include "StringDraw.h"


void StringDraw::Update() {
	int size = lStringInfo.size();
	for (int i = 0; i < size; i++) {
		//lStringInfo[i].DeleteTick--;
		if (lStringInfo[i].DeleteTick < 0) {
			lStringInfo[i] = lStringInfo.back();
			lStringInfo.pop_back();
			i--;
			size--;
		}
	}
}
void StringDraw::Append(Interface::VisibleStringInfo toAppend) {
	lStringInfo.push_back(toAppend);
}
void StringDraw::Draw(HDC hdc) {
	SelectObject(hdc, mhFont);
	SetBkMode(hdc, TRANSPARENT);
	int size = lStringInfo.size();
	for (int i = 0; i < size; i++) {
		SetTextColor(hdc, lStringInfo[i].Color);
		RECT rect;
		int x = (int)lStringInfo[i].Pos.m128_f32[0];
		int y = (int)lStringInfo[i].Pos.m128_f32[1];
		switch (DrawPos)
		{
		case AsCenter:
			rect.left = -1920 + x;
			rect.right = 1920 + x;
			rect.bottom = Height/2 + y;
			rect.top = -1 * Height / 2 + y;
			DrawTextA(hdc, lStringInfo[i].Content.c_str(), -1, &rect, DT_CENTER || DT_VCENTER || DT_SINGLELINE);
			break;
		case AsTopLeftCorner:
			rect.left = x;
			rect.right = 1920 + x;
			rect.bottom = Height + y;
			rect.top = y;
			DrawTextA(hdc, lStringInfo[i].Content.c_str(), -1, &rect, DT_LEFT || DT_TOP);
			break;
		case AsBottomLeftCorner:
			rect.left = x;
			rect.right = 1920 + x;
			rect.bottom = y;
			rect.top = -1 * Height + y;
			DrawTextA(hdc, lStringInfo[i].Content.c_str(), -1, &rect, DT_LEFT || DT_BOTTOM);
			break;
		default:
			break;
		}
	}
}
void StringDraw::DrawAsOnMapObject(HDC hdc, DirectX::XMMATRIX CameraMatrix) {
	SelectObject(hdc, mhFont);
	SetBkMode(hdc, TRANSPARENT);
	int size = lStringInfo.size();
	for (int i = 0; i < size; i++) {
		SetTextColor(hdc, lStringInfo[i].Color);
		RECT rect;
		DirectX::XMVECTOR worldPos = DirectX::XMVector4Transform(lStringInfo[i].Pos, CameraMatrix);
		int x = (int)((worldPos.m128_f32[0] + 1.0f) * WindowWidth / 2.0f);
		int y = (int)((worldPos.m128_f32[1] - 1.0f) * WindowHeight / -2.0f);
		if (x < WindowWidth * 3 / 2 && x > WindowWidth * -1 / 2 &&
			y < WindowHeight * 3 / 2 && y > WindowHeight * -1 / 2) {
			switch (DrawPos)
			{
			case AsCenter:
				rect.left = -1920 + x;
				rect.right = 1920 + x;
				rect.bottom = Height / 2 + y;
				rect.top = -1 * Height / 2 + y;
				DrawTextA(hdc, lStringInfo[i].Content.c_str(), -1, &rect, DT_CENTER || DT_VCENTER || DT_SINGLELINE);
				break;
			case AsTopLeftCorner:
				rect.left = x;
				rect.right = 1920 + x;
				rect.bottom = Height + y;
				rect.top = y;
				DrawTextA(hdc, lStringInfo[i].Content.c_str(), -1, &rect, DT_LEFT || DT_TOP);
				break;
			case AsBottomLeftCorner:
				rect.left = x;
				rect.right = 1920 + x;
				rect.bottom = y;
				rect.top = -1 * Height + y;
				DrawTextA(hdc, lStringInfo[i].Content.c_str(), -1, &rect, DT_LEFT || DT_BOTTOM);
				break;
			default:
				break;
			}
		}
	}
}
