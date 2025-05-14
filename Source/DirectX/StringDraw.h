#pragma once
#include "framework.h"
#include "Direct3D.h"
#include "Interfaces.h"
#include "Appearance.h"
#include "Texture.h"
#include "Buffers.h"
#include "DrawPipe.h"

enum StrDrawPos {
	// �w�肵���ʒu�ɕ�����̒��S������悤�ɂ���
	AsCenter,
	// �w�肵���ʒu��������̏㍶�[�ɂȂ�悤�ɂ���
	AsTopLeftCorner,
	// �w�肵���ʒu��������̉����[�ɂȂ�悤�ɂ���
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
// �傫���A�F�A�ʒu�A���e�͕ς�����
// ���̎�(�t�H���g�A�����Α̂Ȃ�)��ς������ꍇ�͕ʂ̃C���X�^���X���K�v
class GraphicalStringDraw {
public:
	// ������TextureArray��ł̃C���f�b�N�X
	std::vector<short> TextureIndex;

	TimeBindAppearances<Interface::EffectDCType, Interface::EffectIType, 1> mAppearances;
	VertexBuffer<Interface::EffectIType> mInstanceBuffer;
	VertexBuffer<Interface::EffectDCType> mDrawCallBuffer;

	// �s�̍�����1�Ƃ���
	// �e�N�X�`���S�ʂ̏��
	LONG tmHeight;
	LONG tmDescent;
	SameFormatTextureArray mTextureArray;
	HFONT mhFont;
	int TexHeight;

	// �e�����e�N�X�`���̏��
	std::vector<DirectX::XMVECTOR> Offsets;
	std::vector<float> CellWidths;
	std::vector<float> BlackBoxWidths;
	std::vector<float> BlackBoxHeights;

	GraphicalStringDraw(){}
	GraphicalStringDraw(HFONT hFont, int maxCharCount, int maxCharKind, int texHeight) {
		TexHeight = texHeight;
		// code��256*256��ނ��肤��
		TextureIndex = std::vector<short>(256 * 256, -1);
		mAppearances = TimeBindAppearances<Interface::EffectDCType, Interface::EffectIType, 1>(maxCharCount);
		Interface::InitDrawCallUV(mAppearances.DrawCall, 0);
		Interface::InitDrawCallPos(mAppearances.DrawCall, 0);
		mDrawCallBuffer = VertexBuffer < Interface::EffectDCType>(mAppearances.GetDrawCallPointer(0), 4, 0);
		mInstanceBuffer = VertexBuffer<Interface::EffectIType>(mAppearances.GetInstancePointer(0), maxCharCount, 1);
		mTextureArray = SameFormatTextureArray(maxCharKind, true, TexHeight);
		mhFont = hFont;


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
	void Shutdown() {
		mTextureArray.Release();
		mInstanceBuffer.Release();
		mDrawCallBuffer.Release();
	}
	void Update() {
		//���Ԍo�߂ł̏���
		mAppearances.EraseExpired();
	}
	void Draw(ConstantBuffer* pCBuffer) {
		// �`�揈��
		mTextureArray.SetToGraphicPipeLine();
		pCBuffer->UpdateAndSet(nullptr, &mTextureArray.BlackboxMatrices);
		GraphicProcessSetter::SetAsEffect();
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
			if (TextureIndex[code] == -1) {
				// �܂��e�N�X�`���ɂȂ�����
				HDC hdc = GetDC(nullptr);
				SelectObject(hdc, mhFont);
				GLYPHMETRICS glyphMatrix = GLYPHMETRICS();
				MAT2 mat2 = MAT2{
					{0,1},
					{0,0},
					{0,0},
					{0,1},
				};
				DWORD bufferSize = TexHeight * TexHeight;
				std::vector<byte> pData(bufferSize);
				GetGlyphOutlineA(hdc, code, GGO_GRAY8_BITMAP, &glyphMatrix, bufferSize, (void*)&pData[0], &mat2);
				if (bufferSize == GDI_ERROR) {
					throw("");
				}
				int estLength = glyphMatrix.gmBlackBoxX * glyphMatrix.gmBlackBoxY;
				int adjustedWidth = glyphMatrix.gmBlackBoxX + (4 - (glyphMatrix.gmBlackBoxX % 4)) % 4;
				std::vector<byte> pInit = std::vector<byte>(TexHeight * TexHeight * 4, 0);
				for (int y = 0; y < TexHeight; y++) {
					for (int x = 0; x < TexHeight; x++) {
						if (glyphMatrix.gmptGlyphOrigin.x<=x && x < glyphMatrix.gmptGlyphOrigin.x + adjustedWidth
							&& (tmHeight - tmDescent - glyphMatrix.gmptGlyphOrigin.y)<=y && y < (tmHeight - tmDescent - glyphMatrix.gmptGlyphOrigin.y) + glyphMatrix.gmBlackBoxY 
							&& y * adjustedWidth + x < pData.size()) {
							pInit[(y * TexHeight + x) * 4] = min(pData[(y- (tmHeight - tmDescent - glyphMatrix.gmptGlyphOrigin.y)) * adjustedWidth + x- glyphMatrix.gmptGlyphOrigin.x] * 4, 255);
						}
					}
				}
				int texIndex = mTextureArray.AppendImage(SameFormatTextureArray::MakeImageFromColorVector(pInit, TexHeight, TexHeight, 4, 1));

				TextureIndex[code] = texIndex;
				// �����
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
			totalWidth = totalWidth + CellWidths[TextureIndex[code]] * height;
		}
		// 1�����ڂ̍����̈ʒu�𓾂�
		float left;
		float bottom;
		switch (drawPos)
		{
		case AsCenter:
			left = pPos->m128_f32[0] - totalWidth / 2.0f;
			bottom = pPos->m128_f32[1] - height / 2.0f;
			break;
		case AsTopLeftCorner:
			left = pPos->m128_f32[0];
			bottom = pPos->m128_f32[1] - height;
			break;
		case AsBottomLeftCorner:
			left = pPos->m128_f32[0];
			bottom = pPos->m128_f32[1];
			break;
		default:
			left = pPos->m128_f32[0];
			bottom = pPos->m128_f32[1];
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
			int charIndex = TextureIndex[code];
			Interface::EffectIType* pNewInstance;
			if (mAppearances.Add(tickToDelete, isEternal, &pNewInstance)) {
				DirectX::XMMATRIX world = {
					{height,0,0,0},
					{0,height,0,0},
					{0,0,1,0},
					{left + height / 2.0f,bottom + height / 2.0f,0,0}
				};
				pNewInstance->Set(&world, charIndex, pColor, pColor);
			}
			left += CellWidths[charIndex] * height;
		}
	}
};