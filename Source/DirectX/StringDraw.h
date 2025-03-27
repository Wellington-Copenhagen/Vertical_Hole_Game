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
		WindowHeight = windowHeight;
		WindowWidth = windowWidth;
	}
	StrDrawPos DrawPos;
	HFONT mhFont;
	int Height;
	int WindowHeight;
	int WindowWidth;
	std::vector<Interface::VisibleStringInfo> lStringInfo;
	void Update();
	void Append(Interface::VisibleStringInfo toAppend);
	void Draw(HDC hdc);
	void DrawAsOnMapObject(HDC hdc, DirectX::XMMATRIX CameraMatrix);
};
// �傫���A�F�A�ʒu�A���e�͕ς�����
// ���̎�(�t�H���g�A�����Α̂Ȃ�)��ς������ꍇ�͕ʂ̃C���X�^���X���K�v
template<int maxCharCount,int maxCharKind,int texHeight>
class GraphicalStringDraw {
public:
	// ������TextureArray��ł̃C���f�b�N�X
	std::unordered_map<UINT, int> TextureIndexDict;

	// �h���[�R�[���̏��
	Interface::CharDrawCallType DrawCalls[4];
	VertexBuffer<Interface::CharDrawCallType, 4, 0> mDrawCallBuffer;

	// �e�����C���X�^���X�̏��
	int InstanceCount;
	std::vector<std::tuple<int,bool>> TickToDelete;// 2�߂�true�̏ꍇ�͏�������Ȃ�
	Interface::CharInstanceType* Instances = new Interface::CharInstanceType[maxCharCount];
	VertexBuffer<Interface::CharInstanceType, maxCharCount, 1> mInstanceBuffer;

	// �s�̍�����1�Ƃ���
	// �e�N�X�`���S�ʂ̏��
	LONG tmHeight;
	LONG tmDescent;
	SameFormatTextureArray<texHeight> mTextureArray;
	HFONT mhFont;

	// �e�����e�N�X�`���̏��
	std::vector<DirectX::XMVECTOR> Offsets;
	std::vector<float> CellWidths;
	std::vector<float> BlackBoxWidths;
	std::vector<float> BlackBoxHeights;

	GraphicalStringDraw(){}
	GraphicalStringDraw(HFONT hFont) {
		TextureIndexDict = std::unordered_map<UINT, int>();
		mDrawCallBuffer = VertexBuffer < Interface::CharDrawCallType, 4, 0>(DrawCalls);
		mInstanceBuffer = VertexBuffer<Interface::CharInstanceType, maxCharCount, 1>(Instances);
		TickToDelete = std::vector<std::tuple<int, bool>>();
		mTextureArray = SameFormatTextureArray<texHeight>(maxCharKind,false);
		mhFont = hFont;
		InstanceCount = 0;
		DrawCalls[0].Pos = { 0.0f,0.0f,0.0f,1.0f };
		DrawCalls[1].Pos = { 0.0f,1.0f,0.0f,1.0f };
		DrawCalls[2].Pos = { 1.0f,0.0f,0.0f,1.0f };
		DrawCalls[3].Pos = { 1.0f,1.0f,0.0f,1.0f };
		DrawCalls[0].UV = { 0.0f,1.0f };
		DrawCalls[1].UV = { 0.0f,0.0f };
		DrawCalls[2].UV = { 1.0f,1.0f };
		DrawCalls[3].UV = { 1.0f,0.0f };


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
	void Draw() {
		//���Ԍo�߂ł̏���
		for (int i = 0; i < InstanceCount; i++) {
			std::get<0>(TickToDelete[i])--;
			if (std::get<0>(TickToDelete[i]) < 0 && !std::get<1>(TickToDelete[i])) {
				InstanceCount--;
				Instances[i] = Instances[InstanceCount];
				TickToDelete[i] = TickToDelete.back();
				TickToDelete.pop_back();
				i--;
			}
		}
		// �`�揈��
		mTextureArray.SetToGraphicPipeLine();
		GraphicProcessSetter::SetAsCharacter();
		mInstanceBuffer.UpdateAndSet(Instances, 0, InstanceCount);
		mDrawCallBuffer.UpdateAndSet(DrawCalls, 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, InstanceCount, 0, 0);
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
				DWORD bufferSize = GetGlyphOutlineA(hdc, code, GGO_GRAY8_BITMAP, &glyphMatrix, 0, nullptr, &mat2);
				if (bufferSize == GDI_ERROR) {
					throw("");
				}
				byte* pData = new byte[bufferSize];
				bufferSize = GetGlyphOutlineA(hdc, code, GGO_GRAY8_BITMAP, &glyphMatrix, bufferSize, (void*)pData, &mat2);
				if (bufferSize == GDI_ERROR) {
					throw("");
				}
				int adjustedWidth = glyphMatrix.gmBlackBoxX + (4 - (glyphMatrix.gmBlackBoxX % 4)) % 4;
				byte* pInit = new byte[texHeight * texHeight * 4];
				ZeroMemory(pInit, bufferSize * 4);
				for (int y = 0; y < texHeight; y++) {
					for (int x = 0; x < texHeight; x++) {
						if (x < adjustedWidth && y < glyphMatrix.gmBlackBoxY) {
							pInit[(y * texHeight + x) * 4] = pData[y * adjustedWidth + x];
						}
						else {
							pInit[(y * texHeight + x) * 4] = 0;
						}
					}
				}
				int texIndex = mTextureArray.Append((void*)pInit, texHeight * texHeight * 4, texHeight * 4);
				delete pData;
				delete pInit;

				TextureIndexDict.emplace(code, texIndex);
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
			totalWidth = totalWidth + CellWidths[TextureIndexDict[code]] * height;
		}
		// 1�����ڂ̍����̈ʒu�𓾂�
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
			Instances[InstanceCount].TexIndex = charIndex;
			DirectX::XMStoreFloat4(&Instances[InstanceCount].Color, *pColor);
			// 
			DirectX::XMMATRIX world = {
				{height * BlackBoxWidths[charIndex],0,0,0},
				{0,height * BlackBoxHeights[charIndex],0,0},
				{0,0,1,0},
				DirectX::XMVectorAdd(leftBottom,
					DirectX::XMVectorScale(Offsets[charIndex],height))
			};
			DirectX::XMStoreFloat4x4(&Instances[InstanceCount].World,world);
			Instances[InstanceCount].BlackBoxShape = {
				BlackBoxWidths[charIndex] * (float)tmHeight / (float)texHeight,
				BlackBoxHeights[charIndex] * (float)tmHeight / (float)texHeight,
			};
			TickToDelete.push_back({ tickToDelete ,isEternal});
			leftBottom = DirectX::XMVectorAdd(leftBottom, { CellWidths[charIndex] * height,0,0,0 });
			InstanceCount++;
		}
	}
};