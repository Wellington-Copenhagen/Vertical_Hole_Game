#pragma once
#include "framework.h"
#include "Direct3D.h"
#include "Interfaces.h"
#include "Texture.h"
#include "Buffers.h"
#include "DrawPipe.h"

// �F1 �F2 �F�̕ω���(�X��) UV=(0,1)~UV=(1,0)�̊ԂŌX�΂��v�Z���邩�A�O�p�`�̏ꍇ�͌v�Z����ׂ������l�p�`�̏ꍇ�͂��ׂ��łȂ�
// �}�`�̒[�����ł͐F1�ŕ`�悳���B�X��1�ł͐}�`�̒��S�ŐF2�ɂȂ�A�X��2�ł͊O���畝��1/4�������Ƃ���ŐF2�ɂȂ肻��������ł͐F2�ŕ`�悳���
// �X��<=0�ł͑S�̂��F1�ŕ`�悳���

template<int vertexCount>
class OneColorGeometryDraw {
	Interface::OCGDrawCallType DrawCalls[vertexCount];
};
class StripDraw {

};