#include "Appearance.h"
/*
IAppearanceHandle AllAppearance::AddNew(MotionInfo* pWorld,int drawCallIndex) {
	return DrawCalls[drawCallIndex].AddNew(pWorld);
}
void AllAppearance::Draw() {
	int DCsize = DrawCalls.size();
	for (int i = 0; i < DCsize; i++) {
		DrawCalls[i].SetToBufferData(pBuffer);
	}
}
void RectAppearence::SetToBufferData(IAllGraphicBuffer* pBuffer) {
	RectInstanceType* pNext = pBuffer->GetNextRectIPointer();
	DirectX::XMStoreFloat4(&pNext->World1, pWorld->WorldMatrix.r[0]);
	DirectX::XMStoreFloat4(&pNext->World2, pWorld->WorldMatrix.r[1]);
	DirectX::XMStoreFloat4(&pNext->World3, pWorld->WorldMatrix.r[2]);
	DirectX::XMStoreFloat4(&pNext->World4, pWorld->WorldMatrix.r[3]);
}
void RectDrawCallData::SetToBufferData(IAllGraphicBuffer* pBuffer) {
	RectDrawCallType* pNext = pBuffer->GetNextRectDCPointer();
	for (int i = 0; i < 4; i++) {
		pNext[i].Pos = Poss[i];
		pNext[i].UV = UVs[i];
	}
}
void ColorVarRectAppearence::SetToBufferData(IGraphicBuffer* pBuffer) {
	ColorVarRectInstanceType* pNext = pBuffer->GetNextColorVarRectVPointer();
	DirectX::XMStoreFloat4(&pNext->World1, pWorld->WorldMatrix.r[0]);
	DirectX::XMStoreFloat4(&pNext->World2, pWorld->WorldMatrix.r[1]);
	DirectX::XMStoreFloat4(&pNext->World3, pWorld->WorldMatrix.r[2]);
	DirectX::XMStoreFloat4(&pNext->World4, pWorld->WorldMatrix.r[3]);
	pNext->Color1 = Color1;
	pNext->Color2 = Color2;
}
void ColorVarRectDrawCallData::SetToBufferData(IGraphicBuffer* pBuffer) {
	ColorVarRectDrawCallType* pNext = pBuffer->GetNextColorVarRectDCPointer();
	for (int i = 0; i < 4; i++) {
		pNext[i].Pos = Poss[i];
		pNext[i].UV = UVs[i];
	}
}
IAppearanceHandle BaseDrawCallData::AddNew(MotionInfo* pWorld) {
	BelongingInstances.push_front(&RectAppearence(pWorld));
	return BaseAppearanceHandle(&BelongingInstances, BelongingInstances.begin());
}*/
