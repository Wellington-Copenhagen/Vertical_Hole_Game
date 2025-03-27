#pragma once
#include "framework.h"
#include "Direct3D.h"
#include "Interfaces.h"
#include "Texture.h"
#include "Buffers.h"
#include "DrawPipe.h"

// 色1 色2 色の変化率(傾き) UV=(0,1)~UV=(1,0)の間で傾斜を計算するか、三角形の場合は計算するべきだし四角形の場合はすべきでない
// 図形の端部分では色1で描画される。傾き1では図形の中心で色2になり、傾き2では外から幅の1/4いったところで色2になりそれより内側では色2で描画される
// 傾き<=0では全体が色1で描画される

template<int vertexCount>
class OneColorGeometryDraw {
	Interface::OCGDrawCallType DrawCalls[vertexCount];
};
class StripDraw {

};