#include "Graphic.h"


AllGraphics::AllGraphics() {
	Rects = std::vector<std::list<RectGraphic>>();
	Strips = std::vector<std::list<StripGraphic>>();
	RectCI = std::vector<RectCommonInfo>();
	StripCI = std::vector<StripCommonInfo>();
}
void AllGraphics::Draw() {
	{
		int count = Rects.size();
		for (int i = 0; i < count; i++) {
			DP.ResetPositionVertexBufferData();
			DP.SetTexture(RectCI[i].Tex);
			DP.SetRectVertexes(RectCI[i].vDatas);
			if (RectCI[i].OffsetZero) {
				for (std::list<RectGraphic>::iterator itr = Rects[i].begin(); itr != Rects[i].end(); itr++) {
					DP.AddRectInstance(*(itr->World));
				}
			}
			else {
				for (std::list<RectGraphic>::iterator itr = Rects[i].begin(); itr != Rects[i].end(); itr++) {
					if (itr->IsWorldRollable) {
						DP.AddRectInstance(DirectX::XMMatrixMultiply(itr->Offset, *(itr->World)));
					}
					else {
						DirectX::XMMATRIX toAdd = *(itr->World);
						toAdd.r[3] = DirectX::XMVectorAdd(toAdd.r[3], itr->Offset.r[3]);
						DP.AddRectInstance(toAdd);
					}
				}
			}
			DP.DrawAsRectTransparent();
		}
	}
	{
		int count = Strips.size();
		for (int i = 0; i < count; i++) {
			DP.ResetPositionVertexBufferData();
			DP.SetTexture(StripCI[i].Tex);
			for (std::list<StripGraphic>::iterator itr = Strips[i].begin(); itr != Strips[i].end(); itr++) {
				DP.AddStrip(itr->vDatas,(itr->count));
			}
			DP.DrawAsStripTransparent();
		}
	}
}
std::list<RectGraphic>::iterator AllGraphics::AddRect(int index, std::list<RectObject>::iterator relatedTo) {
	RectGraphic toPush = RectGraphic(relatedTo);
	Rects[index].push_back(toPush);
}
std::list<StripGraphic>::iterator AllGraphics::AddStrip(int index, std::list<StripEffect>::iterator relatedTo) {
	StripGraphic toPush = StripGraphic(relatedTo);
	Strips[index].push_back(toPush);
}
void AllGraphics::LoadRectDefaults(std::string filePath) {
	OneLayer::LoadFromFile(filePath);
	std::vector<std::string> names = OneLayer::Root->GetAllLeafName();
	for (int i = 0; i < names.size(); i++) {
		OneLayer* pThisLayer = OneLayer::Root->Get(names[i]);
		RectDictionary[names[i]] = i;
		RectCI[i].OffsetRotatable = pThisLayer->Get("OffsetRotatable")->GetAsBool();
		RectCI[i].OffsetZero = pThisLayer->Get("OffsetZero")->GetAsBool();
		RectCI[i].Tex.Load(pThisLayer->Get("TexturePath")->GetAsString());
		DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3{ 0,0,-1 };
		RectCI[i].vDatas[0].Normal = normal;
		RectCI[i].vDatas[1].Normal = normal;
		RectCI[i].vDatas[2].Normal = normal;
		RectCI[i].vDatas[3].Normal = normal;
		float width = pThisLayer->Get("width")->GetAsFloat();
		float height = pThisLayer->Get("height")->GetAsFloat();
		if (pThisLayer->Get("Shrink")->GetAsBool()) {
			if (width > height) {
				float UVheight = height / (width * 2.0);
				RectCI[i].vDatas[0].UV = { 0,0.5 + UVheight };
				RectCI[i].vDatas[1].UV = { 0,0.5 - UVheight };
				RectCI[i].vDatas[2].UV = { 1,0.5 + UVheight };
				RectCI[i].vDatas[3].UV = { 1,0.5 - UVheight };
			}
			else {
				float UVwidth = width / (height * 2.0);
				RectCI[i].vDatas[0].UV = { 0.5 - UVwidth,1 };
				RectCI[i].vDatas[1].UV = { 0.5 - UVwidth,0 };
				RectCI[i].vDatas[2].UV = { 0.5 + UVwidth,1 };
				RectCI[i].vDatas[3].UV = { 0.5 + UVwidth,0 };
			}
		}
		else {
			RectCI[i].vDatas[0].UV = { 0,1 };
			RectCI[i].vDatas[1].UV = { 0,0 };
			RectCI[i].vDatas[2].UV = { 1,1 };
			RectCI[i].vDatas[3].UV = { 1,0 };
		}
		RectCI[i].vDatas[0].Pos = { width / -2.0,height / -2.0 ,0 };
		RectCI[i].vDatas[1].Pos = { width / -2.0,height / 2.0 ,0 };
		RectCI[i].vDatas[2].Pos = { width / 2.0,height / -2.0 ,0 };
		RectCI[i].vDatas[3].Pos = { width / 2.0,height / 2.0 ,0 };
	}
}

void AllGraphics::LoadStripDefaults(std::string filePath) {
	OneLayer::LoadFromFile(filePath);
	std::vector<std::string> names = OneLayer::Root->GetAllLeafName();
	for (int i = 0; i < names.size(); i++) {
		OneLayer* pThisLayer = OneLayer::Root->Get(names[i]);
		StripDictionary[names[i]] = i;
		StripCI[i].Tex.Load(pThisLayer->Get("TexturePath")->GetAsString());
	}
}