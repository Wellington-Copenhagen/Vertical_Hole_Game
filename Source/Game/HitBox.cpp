#include "HitBox.h"
void BaseHitbox::UpdateAbsoluteCenter(MotionInfo* Parent) {
	AbsoluteCenter = DirectX::XMVector4Transform(RelativeCenter, Parent->WorldMatrix);
}
void BaseHitbox::UpdateTentativeCenter(MotionInfo* Parent) {
	TentativeCenter = DirectX::XMVectorAdd(Parent->WorldMatrix.r[3], Parent->Velocity.r[3]);
}
void HitboxArray::UpdateAbsoluteCenter() {
	int size = Hitboxes.size();
	for (int i = 0; i < size; i++) {
		Hitboxes[i]->UpdateAbsoluteCenter(Parent);
	}
}
void HitboxArray::UpdateTentativeCenter() {
	int size = Hitboxes.size();
	for (int i = 0; i < size; i++) {
		Hitboxes[i]->UpdateTentativeCenter(Parent);
	}
}
bool Collision::IsColliding(BaseHitbox* A, BaseHitbox* B) {
	if (typeid(*A) == typeid(CircleHitbox) && typeid(*B) == typeid(CircleHitbox)) {
		float length = DirectX::XMVector4Length(DirectX::XMVectorSubtract(A->AbsoluteCenter, B->AbsoluteCenter)).m128_f32[0];
		if (length < A->GetRadius() + B->GetRadius()) {
			return true;
		}
	}
	else {
		DirectX::XMVECTOR diff = DirectX::XMVectorAbs(DirectX::XMVectorSubtract(A->AbsoluteCenter, B->AbsoluteCenter));
		if (diff.m128_f32[0] < A->GetWidth() + B->GetWidth() && diff.m128_f32[1] < A->GetHeight() + B->GetHeight()) {
			return true;
		}
	}
	return false;
}
bool Collision::IsColliding(HitboxArray* A, HitboxArray* B, bool AlwaysCollidPair) {
	if (A->Parent->CollisionGroup != B->Parent->CollisionGroup || AlwaysCollidPair)return false;
	int Asize = A->Hitboxes.size();
	int Bsize = B->Hitboxes.size();
	for (int a = 0; a < Asize; a++) {
		for (int b = 0; b < Bsize; b++) {
			if (IsColliding(A->Hitboxes[a], B->Hitboxes[b]))return true;
		}
	}
	return false;
}
void Collision::PenetDepth(BaseHitbox* ToMove, BaseHitbox* B, MotionInfo* parent) {
	DirectX::XMVECTOR dif = DirectX::XMVectorSubtract(ToMove->TentativeCenter, ToMove->AbsoluteCenter);
	if (dif.m128_f32[0] == 0 &&	dif.m128_f32[1] == 0) {
		return;
	}
	float xHigher = B->AbsoluteCenter.m128_f32[0] + (B->GetWidth() + ToMove->GetWidth());
	float xLower = B->AbsoluteCenter.m128_f32[0] - (B->GetWidth() + ToMove->GetWidth());
	float yHigher = B->AbsoluteCenter.m128_f32[1] + (B->GetHeight() + ToMove->GetHeight());
	float yLower = B->AbsoluteCenter.m128_f32[1] - (B->GetHeight() + ToMove->GetHeight());
	xHigher = (xHigher - ToMove->AbsoluteCenter.m128_f32[0]) / dif.m128_f32[0];
	xLower = (xLower - ToMove->AbsoluteCenter.m128_f32[0]) / dif.m128_f32[0];
	yHigher = (yHigher - ToMove->AbsoluteCenter.m128_f32[1]) / dif.m128_f32[1];
	yLower = (yLower - ToMove->AbsoluteCenter.m128_f32[1]) / dif.m128_f32[1];
	if (parent->MoveLimit > xHigher && ((yLower<xHigher)^ (yHigher < xHigher))) {
		parent->MoveLimit = xHigher;
	}
	if (parent->MoveLimit > xLower && ((yLower < xLower) ^ (yHigher < xLower))) {
		parent->MoveLimit = xLower;
	}
	if (parent->MoveLimit > yHigher && ((xLower < yHigher) ^ (xHigher < yHigher))) {
		parent->MoveLimit = yHigher;
	}
	if (parent->MoveLimit > yLower && ((xLower < yLower) ^ (xHigher < yLower))) {
		parent->MoveLimit = yLower;
	}
}
void Collision::PenetDepth(HitboxArray* ToMove, HitboxArray* B) {
	int Asize = ToMove->Hitboxes.size();
	int Bsize = B->Hitboxes.size();
	for (int a = 0; a < Asize; a++) {
		for (int b = 0; b < Bsize; b++) {
			PenetDepth(ToMove->Hitboxes[a], B->Hitboxes[b], ToMove->Parent);
		}
	}
}