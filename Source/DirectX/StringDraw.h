#pragma once
#include "framework.h"
#include "Direct3D.h"
#include "Interfaces.h"
#include "Appearance.h"
#include "Texture.h"
#include "Buffers.h"
#include "DrawPipe.h"

enum StrDrawPos {
	// 指定した位置に文字列の中心が来るようにする
	AsCenter,
	// 指定した位置が文字列の上左端になるようにする
	AsTopLeftCorner,
	// 指定した位置が文字列の下左端になるようにする
	AsBottomLeftCorner,
};
class StringDraw {
public:
	StringDraw(){}
	StringDraw(HFONT hFont, StrDrawPos drawPos,int height,int windowHeight,int windowWidth) {
		mhFont = hFont;
		DrawPos = drawPos;
		Height = height;
	}
	StrDrawPos DrawPos;
	HFONT mhFont;
	int Height;
	std::vector<Interface::VisibleStringInfo> lStringInfo;
	void Update();
	void Append(Interface::VisibleStringInfo toAppend);
	void Draw(HDC hdc);
	void DrawAsOnMapObject(HDC hdc, DirectX::XMMATRIX CameraMatrix);
};
// 大きさ、色、位置、内容は変えられる
// 他の事(フォント、太字斜体など)を変えたい場合は別のインスタンスが必要
class GraphicalStringDraw {
public:
	// 文字とTextureArray上でのインデックス
	std::unordered_map<UINT, int> TextureIndexDict;

	TimeBindAppearances<Interface::CharDrawCallType, Interface::CharInstanceType, 1> mAppearances;
	VertexBuffer<Interface::CharInstanceType> mInstanceBuffer;
	VertexBuffer<Interface::CharDrawCallType> mDrawCallBuffer;

	// 行の高さを1とする
	// テクスチャ全般の情報
	LONG tmHeight;
	LONG tmDescent;
	SameFormatTextureArray mTextureArray;
	HFONT mhFont;
	int TexHeight;

	// 各文字テクスチャの情報
	std::vector<DirectX::XMVECTOR> Offsets;
	std::vector<float> CellWidths;
	std::vector<float> BlackBoxWidths;
	std::vector<float> BlackBoxHeights;

	GraphicalStringDraw(){}
	GraphicalStringDraw(HFONT hFont, int maxCharCount, int maxCharKind, int texHeight) {
		TexHeight = texHeight;
		TextureIndexDict = std::unordered_map<UINT, int>();
		mAppearances = TimeBindAppearances<Interface::CharDrawCallType, Interface::CharInstanceType, 1>(maxCharCount);
		Interface::InitDrawCallUV(mAppearances.DrawCall, 0);
		mDrawCallBuffer = VertexBuffer < Interface::CharDrawCallType>(mAppearances.GetDrawCallPointer(0), 4, 0);
		mInstanceBuffer = VertexBuffer<Interface::CharInstanceType>(mAppearances.GetInstancePointer(0), maxCharCount, 1);
		mTextureArray = SameFormatTextureArray(maxCharKind, true, TexHeight);
		mhFont = hFont;
		mAppearances.DrawCall[0].Pos = { 0.0f,0.0f,0.0f,1.0f };
		mAppearances.DrawCall[1].Pos = { 0.0f,1.0f,0.0f,1.0f };
		mAppearances.DrawCall[2].Pos = { 1.0f,0.0f,0.0f,1.0f };
		mAppearances.DrawCall[3].Pos = { 1.0f,1.0f,0.0f,1.0f };


		TEXTMETRIC tm;
		HDC hdc = GetDC(nullptr);
		SelectObject(hdc, mhFont);
		GetTextMetrics(hdc, &tm);
		tmHeight = tm.tmHeight;
		tmDescent = tm.tmDescent;

		Offsets = std::vector<DirectX::XMVECTOR>();
		CellWidths = std::vector<float>();
		BlackBoxWidths = std::vector<float>();
		BlackBoxHeights = std::vector<float>();


	}
	void Update() {
		//時間経過での消滅
		mAppearances.EraseExpired();
	}
	void Draw() {
		// 描画処理
		mTextureArray.SetToGraphicPipeLine();
		GraphicProcessSetter::SetAsCharacter();
		mInstanceBuffer.UpdateAndSet(mAppearances.GetInstancePointer(0), 0, mAppearances.InstanceCount);
		mDrawCallBuffer.UpdateAndSet(mAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mAppearances.InstanceCount, 0, 0);
	}
	void SimpleAppend(std::string content, float red,float green,float blue, DirectX::XMVECTOR pos, float height, int tickToDelete, StrDrawPos drawPos) {
		DirectX::XMVECTOR color = { red,green,blue,1 };
		Append(content, &color, &pos, height, tickToDelete, drawPos);
	}
	void Append(std::string content, DirectX::XMVECTOR* pColor, DirectX::XMVECTOR* pPos,float height,int tickToDelete,StrDrawPos drawPos,bool isEternal=false) {
		float totalWidth = 0;
		for (int i = 0; i < content.size();i++) {
			UINT code;
			// Double Byte Character Set
			if (IsDBCSLeadByte(content[i])) {
				code = (BYTE)content[i] <<8 | (BYTE)content[i + 1];
				i++;
			}
			else {
				code = content[i];
			}
			if (!TextureIndexDict.contains(code)) {
				// まだテクスチャにない文字
				HDC hdc = GetDC(nullptr);
				SelectObject(hdc, mhFont);
				GLYPHMETRICS glyphMatrix = GLYPHMETRICS();
				MAT2 mat2 = MAT2{
					{0,1},
					{0,0},
					{0,0},
					{0,1},
				};
				DWORD bufferSize = GetGlyphOutlineA(hdc, code, GGO_GRAY8_BITMAP, &glyphMatrix, 0, nullptr, &mat2);
				if (bufferSize == GDI_ERROR) {
					throw("");
				}
				std::vector<byte> pData(bufferSize);
				GetGlyphOutlineA(hdc, code, GGO_GRAY8_BITMAP, &glyphMatrix, bufferSize, (void*)&pData[0], &mat2);
				if (bufferSize == GDI_ERROR) {
					throw("");
				}
				int adjustedWidth = glyphMatrix.gmBlackBoxX + (4 - (glyphMatrix.gmBlackBoxX % 4)) % 4;
				std::vector<byte> pInit = std::vector<byte>(TexHeight * TexHeight * 4, 0);
				for (int y = 0; y < TexHeight; y++) {
					for (int x = 0; x < TexHeight; x++) {
						if (x < adjustedWidth && y < glyphMatrix.gmBlackBoxY && y * adjustedWidth + x < pData.size()) {
							pInit[(y * TexHeight + x) * 4] = min(pData[y * adjustedWidth + x] * 4, 255);
						}
						else {
							pInit[(y * TexHeight + x) * 4] = 0;
						}
					}
				}
				int texIndex = mTextureArray.Append((void*)&pInit[0], TexHeight * TexHeight * 4, TexHeight * 4);

				TextureIndexDict.emplace(code, texIndex);
				// 左下基準
				Offsets.push_back({
					glyphMatrix.gmptGlyphOrigin.x / (float)tmHeight,
					(tmDescent + glyphMatrix.gmptGlyphOrigin.y - glyphMatrix.gmBlackBoxY) / (float)tmHeight,
					0.0f,
					0.0f
				});
				CellWidths.push_back((float)glyphMatrix.gmCellIncX / (float)tmHeight);
				BlackBoxWidths.push_back((float)glyphMatrix.gmBlackBoxX / (float)tmHeight);
				BlackBoxHeights.push_back((float)glyphMatrix.gmBlackBoxY / (float)tmHeight);
			}
			totalWidth = totalWidth + CellWidths[TextureIndexDict[code]] * height;
		}
		// 1文字目の左下の位置を得る
		DirectX::XMVECTOR leftBottom;
		switch (drawPos)
		{
		case AsCenter:
			leftBottom = DirectX::XMVectorAdd(*pPos, { totalWidth / -2.0f,height / -2.0f,0.0f,0.0f });
			break;
		case AsTopLeftCorner:
			leftBottom = DirectX::XMVectorAdd(*pPos, { 0.0f,height * -1.0f,0.0f,0.0f });
			break;
		case AsBottomLeftCorner:
			leftBottom = *pPos;
			break;
		default:
			leftBottom = *pPos;
			break;
		}
		for (int i = 0; i < content.size(); i++) {
			UINT code;
			// Double Byte Character Set
			if (IsDBCSLeadByte(content[i])) {
				code = (BYTE)content[i] << 8 | (BYTE)content[i + 1];
				i++;
			}
			else {
				code = content[i];
			}
			int charIndex = TextureIndexDict[code];
			Interface::CharInstanceType* pNewInstance;
			mAppearances.Add(tickToDelete, isEternal, &pNewInstance);
			pNewInstance->TexIndex = charIndex;
			DirectX::XMStoreFloat4(&pNewInstance->Color, *pColor);
			// 
			DirectX::XMMATRIX world = {
				{height * BlackBoxWidths[charIndex],0,0,0},
				{0,height * BlackBoxHeights[charIndex],0,0},
				{0,0,1,0},
				DirectX::XMVectorAdd(leftBottom,
					DirectX::XMVectorScale(Offsets[charIndex],height))
			};
			DirectX::XMStoreFloat4x4(&pNewInstance->World,world);
			pNewInstance->BlackBoxShape = {
				BlackBoxWidths[charIndex] * (float)tmHeight / (float)TexHeight,
				BlackBoxHeights[charIndex] * (float)tmHeight / (float)TexHeight,
			};
			leftBottom = DirectX::XMVectorAdd(leftBottom, { CellWidths[charIndex] * height,0,0,0 });
		}
	}
};