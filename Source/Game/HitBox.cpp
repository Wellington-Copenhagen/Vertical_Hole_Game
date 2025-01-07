#include "HitBox.h"
void BaseCollidbox::UpdateAbsoluteCenter(MotionInfo* Parent) {
	AbsoluteCenter = DirectX::XMVector4Transform(RelativeCenter, Parent->WorldMatrix);
}
void BaseCollidbox::UpdateTentativeCenter(MotionInfo* Parent) {
	TentativeCenter = DirectX::XMVectorAdd(Parent->WorldMatrix.r[3], Parent->Velocity.r[3]);
}
void CollidboxArray::UpdateAbsoluteCenter() {
	int size = Collidboxes.size();
	for (int i = 0; i < size; i++) {
		Collidboxes[i].UpdateAbsoluteCenter(Parent);
	}
}
void CollidboxArray::UpdateTentativeCenter() {
	int size = Collidboxes.size();
	for (int i = 0; i < size; i++) {
		Collidboxes[i].UpdateTentativeCenter(Parent);
	}
}
bool Collision::IsColliding(BaseCollidbox* A, BaseCollidbox* B) {
	if (typeid(*A) == typeid(CircleCollidbox) && typeid(*B) == typeid(CircleCollidbox)) {
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
bool Collision::IsColliding(CollidboxArray* A, CollidboxArray* B, bool AlwaysCollidPair) {
	if (A->Parent->CollisionGroup != B->Parent->CollisionGroup || AlwaysCollidPair)return false;
	int Asize = A->Collidboxes.size();
	int Bsize = B->Collidboxes.size();
	for (int a = 0; a < Asize; a++) {
		for (int b = 0; b < Bsize; b++) {
			if (IsColliding(&A->Collidboxes[a], &B->Collidboxes[b]))return true;
		}
	}
	return false;
}
void Collision::PenetDepth(BaseCollidbox* ToMove, BaseCollidbox* B, MotionInfo* parent) {
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
void Collision::PenetDepth(CollidboxArray* ToMove, CollidboxArray* B) {
	int Asize = ToMove->Collidboxes.size();
	int Bsize = B->Collidboxes.size();
	for (int a = 0; a < Asize; a++) {
		for (int b = 0; b < Bsize; b++) {
			PenetDepth(&ToMove->Collidboxes[a], &B->Collidboxes[b], ToMove->Parent);
		}
	}
}