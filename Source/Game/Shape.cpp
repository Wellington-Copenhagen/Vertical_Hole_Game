#include "Shape.h"

bool IsPointInTriangle(DirectX::XMVECTOR point, DirectX::XMVECTOR triangle[3], bool IsClockWise) {
	if ((DirectX::XMVector2Cross(DirectX::XMVectorSubtract(triangle[1], triangle[0]), DirectX::XMVectorSubtract(point, triangle[0])).m128_f32[0]
		> 0) ^ IsClockWise) {
		//XMVector2Cross�����ɂȂ�͎̂��v���Ŏw�肳��Ă�ꍇ��point�������ɂ���ꍇ
		return false;
	}
	if ((DirectX::XMVector2Cross(DirectX::XMVectorSubtract(triangle[2], triangle[1]), DirectX::XMVectorSubtract(point, triangle[1])).m128_f32[0]
		> 0) ^ IsClockWise) {
		//XMVector2Cross�����ɂȂ�͎̂��v���Ŏw�肳��Ă�ꍇ��point�������ɂ���ꍇ
		return false;
	}
	if ((DirectX::XMVector2Cross(DirectX::XMVectorSubtract(triangle[0], triangle[2]), DirectX::XMVectorSubtract(point, triangle[2])).m128_f32[0]
		> 0) ^ IsClockWise) {
		//XMVector2Cross�����ɂȂ�͎̂��v���Ŏw�肳��Ă�ꍇ��point�������ɂ���ꍇ
		return false;
	}
	return true;
}
bool IsPointInRect(DirectX::XMVECTOR point, DirectX::XMVECTOR rect[4]) {
	if (DirectX::XMVector2Cross(DirectX::XMVectorSubtract(rect[1], rect[0]), DirectX::XMVectorSubtract(point, rect[0])).m128_f32[0]
		> 0) {
		//XMVector2Cross�����ɂȂ�͎̂��v���Ŏw�肳��Ă�ꍇ��point�������ɂ���ꍇ
		return false;
	}
	if (DirectX::XMVector2Cross(DirectX::XMVectorSubtract(rect[2], rect[1]), DirectX::XMVectorSubtract(point, rect[1])).m128_f32[0]
		> 0) {
		//XMVector2Cross�����ɂȂ�͎̂��v���Ŏw�肳��Ă�ꍇ��point�������ɂ���ꍇ
		return false;
	}
	if (DirectX::XMVector2Cross(DirectX::XMVectorSubtract(rect[3], rect[2]), DirectX::XMVectorSubtract(point, rect[2])).m128_f32[0]
		> 0) {
		//XMVector2Cross�����ɂȂ�͎̂��v���Ŏw�肳��Ă�ꍇ��point�������ɂ���ꍇ
		return false;
	}
	if (DirectX::XMVector2Cross(DirectX::XMVectorSubtract(rect[0], rect[3]), DirectX::XMVectorSubtract(point, rect[3])).m128_f32[0]
		> 0) {
		//XMVector2Cross�����ɂȂ�͎̂��v���Ŏw�肳��Ă�ꍇ��point�������ɂ���ꍇ
		return false;
	}
}
float VectorDistance(DirectX::XMVECTOR inputA, DirectX::XMVECTOR inputB) {
	sqrtf(inputA.m128_f32[0] * inputB.m128_f32[0] + inputA.m128_f32[1] * inputB.m128_f32[1]);
}

void PerpendicularFoot(DirectX::XMVECTOR lineStart, DirectX::XMVECTOR lineEnd, DirectX::XMVECTOR point, float& distance, float& ratio) {
	DirectX::XMVECTOR pointDirection = DirectX::XMVectorSubtract(point, lineStart);
	DirectX::XMVECTOR line = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(lineEnd, lineStart));
	ratio = DirectX::XMVector2Dot(DirectX::XMVector2Normalize(pointDirection),line).m128_f32[0];
	distance = DirectX::XMVector2Cross(pointDirection, line).m128_f32[0];
}
bool IsColliding(Rect inputA, Circle inputB) {
	//�O���ɂ���ꍇ�A��
	float r = inputB.Radius;
	float xLeftRange = inputA.Wpos[0].m128_f32[0] - inputB.Center.m128_f32[0];
	float yDownRange = inputA.Wpos[0].m128_f32[1] - inputB.Center.m128_f32[1];
	float xRightRange = inputB.Center.m128_f32[0] - inputA.Wpos[3].m128_f32[0];
	float yUpRange = inputB.Center.m128_f32[1] - inputA.Wpos[3].m128_f32[1];
	if (yUpRange > r || yDownRange > r || xLeftRange > r || xRightRange > r)return false;
	if ((xLeftRange < 0 && xRightRange < 0) || (yUpRange < 0 && yDownRange < 0))return true;
	if (xLeftRange > 0 && yDownRange > 0) {
		if (VectorDistance(inputA.Wpos[0], inputB.Center)<r)return true;
	}
	if (xLeftRange > 0 && yUpRange > 0) {
		if (VectorDistance(inputA.Wpos[1], inputB.Center) < r)return true;
	}
	if (xLeftRange > 0 && yDownRange > 0) {
		if (VectorDistance(inputA.Wpos[2], inputB.Center) < r)return true;
	}
	if (xLeftRange > 0 && yUpRange > 0) {
		if (VectorDistance(inputA.Wpos[3], inputB.Center) < r)return true;
	}
	return false;
}
bool IsColliding(Rect inputA, Circle inputB, DirectX::XMVECTOR& MoveRange) {
	//�O���ɂ���ꍇ�A��
	float r = inputB.Radius;
	float xLeftRange = inputA.Wpos[0].m128_f32[0] - inputB.Center.m128_f32[0];
	float yDownRange = inputA.Wpos[0].m128_f32[1] - inputB.Center.m128_f32[1];
	float xRightRange = inputB.Center.m128_f32[0] - inputA.Wpos[3].m128_f32[0];
	float yUpRange = inputB.Center.m128_f32[1] - inputA.Wpos[3].m128_f32[1];
	if (yUpRange > r || yDownRange > r || xLeftRange > r || xRightRange > r)return false;
	float LDtoRU = DirectX::XMVector2Cross(DirectX::XMVectorSubtract(inputA.Wpos[3], inputA.Wpos[0]),
		DirectX::XMVectorSubtract(inputB.Center, inputA.Wpos[0])).m128_f32[0];
	float LUtoRD = DirectX::XMVector2Cross(DirectX::XMVectorSubtract(inputA.Wpos[1], inputA.Wpos[2]),
		DirectX::XMVectorSubtract(inputB.Center, inputA.Wpos[2])).m128_f32[0];
	//���̏ꍇ�͎΂߂̐�����㑤
	if ((xLeftRange < 0 && xRightRange < 0) || (yUpRange < 0 && yDownRange < 0)){
		if (LDtoRU && LUtoRD) {
			MoveRange = DirectX::XMVECTOR{ 0,r - yUpRange,0,0 };
		}
		if (!LDtoRU && LUtoRD) {
			MoveRange = DirectX::XMVECTOR{ r - xRightRange,0,0,0 };
		}
		if (LDtoRU && !LUtoRD) {
			MoveRange = DirectX::XMVECTOR{ xLeftRange - r,0,0,0 };
		}
		if (!LDtoRU && !LUtoRD) {
			MoveRange = DirectX::XMVECTOR{ 0,yDownRange - r ,0,0 };
		}
		return true;
	}
	if (xLeftRange > 0 && yDownRange > 0) {
		if (VectorDistance(inputA.Wpos[0], inputB.Center) < r) {
			MoveRange = DirectX::XMVectorSubtract(inputB.Center,inputA.Wpos[0]);
		}
		return true;
	}
	if (xLeftRange > 0 && yUpRange > 0) {
		if (VectorDistance(inputA.Wpos[1], inputB.Center) < r) {
			MoveRange = DirectX::XMVectorSubtract(inputB.Center, inputA.Wpos[1]);
		}
		return true;
	}
	if (xLeftRange > 0 && yDownRange > 0) {
		if (VectorDistance(inputA.Wpos[2], inputB.Center) < r) {
			MoveRange = DirectX::XMVectorSubtract(inputB.Center, inputA.Wpos[2]);
		}
		return true;
	}
	if (xLeftRange > 0 && yUpRange > 0) {
		if (VectorDistance(inputA.Wpos[3], inputB.Center) < r) {
			MoveRange = DirectX::XMVectorSubtract(inputB.Center, inputA.Wpos[3]);
		}
		return true;
	}
	return false;
}
bool IsColliding(Circle inputA, Strip inputB) {
	float r = inputA.Radius;
	if (inputA.Center.m128_f32[0] > inputB.HighestX + r ||
		inputA.Center.m128_f32[0] < inputB.LowestX - r ||
		inputA.Center.m128_f32[1] > inputB.HighestY + r ||
		inputA.Center.m128_f32[1] < inputB.LowestY - r)return false;
	for (int i = 0; i < inputB.VerCount; i++) {
		DirectX::XMVECTOR vertexToCenter = DirectX::XMVectorSubtract(inputA.Center,inputB.Wpos[i]);
		if (DirectX::XMVector2Dot(vertexToCenter, vertexToCenter).m128_f32[0] < r)return true;//���_���~�̒�
	}
	for (int i = 0; i < inputB.VerCount - 2; i++) {
		int innerCount = 0;
		if (i % 2) {
			//�����v���
			{
				float distance;
				float ratio;
				PerpendicularFoot(inputB.Wpos[i + 2], inputB.Wpos[i + 1], inputA.Center, distance, ratio);
				if (distance > r) {
					continue;
					//���̕ӂɂ��G��邱�Ƃ͂Ȃ��O���̗̈�
					//�O�p�`�͏�ɓʂȂ̂ł��̗̈�ɂ�����̂͐�΂ɂ��̎O�p�`�ƐG��Ȃ�
				}
				if (ratio >= 0 && ratio <= 1 && distance < r && distance > 0) {
					return true;
					//�K���ӂɐG��Ă���O���̗̈�
				}
				if (distance < 0) {
					innerCount++;
					//�ӂ������̗̈�
				}
			}
			{
				float distance;
				float ratio;
				PerpendicularFoot(inputB.Wpos[i + 1], inputB.Wpos[i + 0], inputA.Center, distance, ratio);
				if (distance > r) {
					continue;
					//���̕ӂɂ��G��邱�Ƃ͂Ȃ��O���̗̈�
					//�O�p�`�͏�ɓʂȂ̂ł��̗̈�ɂ�����̂͐�΂ɂ��̎O�p�`�ƐG��Ȃ�
				}
				if (ratio >= 0 && ratio <= 1 && distance < r && distance > 0) {
					return true;
					//�K���ӂɐG��Ă���O���̗̈�
				}
				if (distance < 0) {
					innerCount++;
					//�ӂ������̗̈�
				}
			}
			{
				float distance;
				float ratio;
				PerpendicularFoot(inputB.Wpos[i + 0], inputB.Wpos[i + 2], inputA.Center, distance, ratio);
				if (distance > r) {
					continue;
					//���̕ӂɂ��G��邱�Ƃ͂Ȃ��O���̗̈�
					//�O�p�`�͏�ɓʂȂ̂ł��̗̈�ɂ�����̂͐�΂ɂ��̎O�p�`�ƐG��Ȃ�
				}
				if (ratio >= 0 && ratio <= 1 && distance < r && distance > 0) {
					return true;
					//�K���ӂɐG��Ă���O���̗̈�
				}
				if (distance < 0) {
					innerCount++;
					//�ӂ������̗̈�
				}
			}
		}
		else {
			//���v����
			{
				float distance;
				float ratio;
				PerpendicularFoot(inputB.Wpos[i + 0], inputB.Wpos[i + 1], inputA.Center, distance, ratio);
				if (distance > r) {
					continue;
					//���̕ӂɂ��G��邱�Ƃ͂Ȃ��O���̗̈�
					//�O�p�`�͏�ɓʂȂ̂ł��̗̈�ɂ�����̂͐�΂ɂ��̎O�p�`�ƐG��Ȃ�
				}
				if (ratio >= 0 && ratio <= 1 && distance < r && distance > 0) {
					return true;
					//�K���ӂɐG��Ă���O���̗̈�
				}
				if (distance < 0) {
					innerCount++;
					//�ӂ������̗̈�
				}
			}
			{
				float distance;
				float ratio;
				PerpendicularFoot(inputB.Wpos[i + 1], inputB.Wpos[i + 2], inputA.Center, distance, ratio);
				if (distance > r) {
					continue;
					//���̕ӂɂ��G��邱�Ƃ͂Ȃ��O���̗̈�
					//�O�p�`�͏�ɓʂȂ̂ł��̗̈�ɂ�����̂͐�΂ɂ��̎O�p�`�ƐG��Ȃ�
				}
				if (ratio >= 0 && ratio <= 1 && distance < r && distance > 0) {
					return true;
					//�K���ӂɐG��Ă���O���̗̈�
				}
				if (distance < 0) {
					innerCount++;
					//�ӂ������̗̈�
				}
			}
			{
				float distance;
				float ratio;
				PerpendicularFoot(inputB.Wpos[i + 2], inputB.Wpos[i + 0], inputA.Center, distance, ratio);
				if (distance > r) {
					continue;
					//���̕ӂɂ��G��邱�Ƃ͂Ȃ��O���̗̈�
					//�O�p�`�͏�ɓʂȂ̂ł��̗̈�ɂ�����̂͐�΂ɂ��̎O�p�`�ƐG��Ȃ�
				}
				if (ratio >= 0 && ratio <= 1 && distance < r && distance > 0) {
					return true;
					//�K���ӂɐG��Ă���O���̗̈�
				}
				if (distance < 0) {
					innerCount++;
					//�ӂ������̗̈�
				}
			}
		}
	}
}
bool IsColliding(Circle inputA, Circle inputB) {
	DirectX::XMVECTOR centerToCenter = DirectX::XMVectorSubtract(inputA.Center, inputB.Center);
	if (DirectX::XMVector2Dot(centerToCenter, centerToCenter).m128_f32[0] < inputA.Radius + inputB.Radius)return true;
}
Strip::Strip(DirectX::XMVECTOR wpos[], DirectX::XMFLOAT2 uvs[], int verCount) {
	Wpos = new DirectX::XMVECTOR[VerCount];
	UVs = new DirectX::XMFLOAT2[VerCount];
	for (int i = 0; i < VerCount; i++) {
		Wpos[i] = wpos[i];
		UVs[i] = uvs[i];
		if (i == 0) {
			LowestY = wpos[i].m128_f32[1];
			HighestY = wpos[i].m128_f32[1];
			LowestX = wpos[i].m128_f32[0];
			HighestX = wpos[i].m128_f32[0];
		}
		else {
			if (wpos[i].m128_f32[0] < LowestX)LowestX = wpos[i].m128_f32[0];
			if (wpos[i].m128_f32[0] > HighestX)HighestX = wpos[i].m128_f32[0];
			if (wpos[i].m128_f32[1] < LowestY)LowestY = wpos[i].m128_f32[1];
			if (wpos[i].m128_f32[1] > HighestY)HighestY = wpos[i].m128_f32[1];
		}
	}
	VerCount = verCount;
}
Rect::Rect(DirectX::XMVECTOR wpos[4]) {
	Wpos[0] = wpos[0];
	Wpos[1] = wpos[1];
	Wpos[2] = wpos[2];
	Wpos[3] = wpos[3];
}
Circle::Circle(DirectX::XMVECTOR relCenterPos,DirectX::XMMATRIX world, float radius) {
	Center = DirectX::XMVector4Transform(relCenterPos, world);
	Radius = radius;
}
void Circle::SetCenter(DirectX::XMVECTOR relCenterPos) {
	Center = DirectX::XMVector4Transform(relCenterPos, world);
}