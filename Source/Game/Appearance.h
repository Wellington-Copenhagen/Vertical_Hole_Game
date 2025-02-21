#pragma once
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"

//�}�X�N��
// r->1���ڃe�N�X�`���Z�x
// g->2���ڃe�N�X�`���Z�x
// b->3���ڃe�N�X�`���Z�x(�e�ł��������Ǝv����)
// 
//���_�f�[�^�̒ǉ��A�X�V�͂����ōs���Ă���
//Buffers��Instances�̃|�C���^���󂯎�邾��

// : public Interface::IAppearances<DCType,IType>

template <class DCType, class IType, int InstancePerEntity, int InstanceMaxCount>
class Appearances{
public:
	std::vector<Interface::EntityPointer>* pINtoIndex;
	//���ۂ̃C���X�^���X�f�[�^
	IType* Instances;
	//�o�^�����G���e�B�e�B
	Interface::EntId* InstanceIds;
	//�h���[�R�[���̏��
	DCType DrawCall[4];
	//�e���ނ��Ƃ̓o�^��
	Interface::RectAppId InstanceCount;
	Appearances(std::vector<Interface::EntityPointer>* pintoindex, int separation, float bottomXscale=1, float topXscale=1, float Yscale=1, float Yoffset=0) {
		pINtoIndex = pintoindex;
		Instances = new IType[InstanceMaxCount * InstancePerEntity];
		InstanceIds = new Interface::EntId[InstanceMaxCount];
		ZeroMemory(Instances, sizeof(Instances));
		ZeroMemory(InstanceIds, sizeof(InstanceIds));
		ZeroMemory(DrawCall, sizeof(DrawCall));
		InstanceCount = 0;
		float width = 4.0f / 6.0f;
		DrawCall[0].Pos = { -1 * bottomXscale * width,-1 * Yscale * width + Yoffset,0.0f,1.0f };
		DrawCall[1].Pos = { -1 * topXscale * width,Yscale * width+Yoffset,0.0f,1.0f };
		DrawCall[2].Pos = { bottomXscale * width,-1 * Yscale * width + Yoffset,0.0f,1.0f };
		DrawCall[3].Pos = { topXscale * width,Yscale * width + Yoffset,0.0f,1.0f };
		DrawCall[0].UV = { 0.0f,1.0f / separation,(float)separation };
		DrawCall[1].UV = { 0.0f,0.0f,(float)separation };
		DrawCall[2].UV = { 1.0f / separation,1.0f / separation,(float)separation };
		DrawCall[3].UV = { 1.0f / separation,0.0f,(float)separation };
	}
	Appearances() {
		Instances = nullptr;
		InstanceIds = nullptr;
	}
	//���̃C���X�^���X�̃|�C���^��Ԃ��Ƃ������Ƃ�
	inline Interface::RectAppId Add(Interface::EntId Id, IType** pInstanceData) {
		if (InstanceCount >= InstanceMaxCount - 1) {
			return -1;
		}
		*pInstanceData = &Instances[InstanceCount*InstancePerEntity];
		InstanceIds[InstanceCount] = Id;
		InstanceCount++;
		return InstanceCount - 1;
	}
	inline void Update(Interface::RectAppId Id, IType** pInstanceData) {
		if (Id != -1) {
			*pInstanceData = &Instances[Id*InstancePerEntity];
		}
	}
	//
	inline Interface::SameArchIndex Delete(Interface::RectAppId toDelete) {
		if (toDelete != -1) {
			//���_�f�[�^�̈ړ�
			for (int i = 0; i < InstancePerEntity;i++) {
				Instances[toDelete * InstancePerEntity + i] = Instances[InstanceCount * InstancePerEntity + i];
			}
			//EntId�̈ړ�
			InstanceIds[toDelete] = InstanceIds[InstanceCount];
			//�J�E���g�����炷
			InstanceCount--;
			//�ړ����������̂�SameArchIndex���o��
			return pINtoIndex->at(InstanceIds[toDelete]).Index;
		}
		else {
			int a = pINtoIndex->size();
			return -1;
		}
	}
};