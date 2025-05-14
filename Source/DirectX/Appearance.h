#pragma once
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"
#include "Source/Game/Component.h"
extern int Tick;
//�}�X�N��
// r->1���ڃe�N�X�`���Z�x
// g->2���ڃe�N�X�`���Z�x
// b->3���ڃe�N�X�`���Z�x(�e�ł��������Ǝv����)
// 
//���_�f�[�^�̒ǉ��A�X�V�͂����ōs���Ă���
//Buffers��Instances�̃|�C���^���󂯎�邾��

// : public Interface::IAppearances<DCType,IType>

template <class DCType, class IType,class CompType,int InstancePerEntity>
class EntityBindAppearances{
public:
	int InstanceMaxCount;
	//���ۂ̃C���X�^���X�f�[�^
	std::vector<IType> Instances;
	//�o�^�����G���e�B�e�B
	std::vector<entt::entity> InstanceEntities;
	//�h���[�R�[���̏��
	std::vector<DCType> DrawCall;
	//�e���ނ��Ƃ̓o�^��
	Interface::RectAppId InstanceCount;
	EntityBindAppearances(int instanceMaxCount) {
		InstanceMaxCount = instanceMaxCount;
		Instances = std::vector<IType>(InstanceMaxCount * InstancePerEntity);
		DrawCall = std::vector<DCType>(4 * InstancePerEntity);
		InstanceEntities = std::vector<entt::entity>(InstanceMaxCount);
		InstanceCount = 0;
	}
	EntityBindAppearances() {
	}
	inline IType* GetInstancePointer(int index) {
		return &Instances[index * InstanceMaxCount];
	}
	inline DCType* GetDrawCallPointer(int index) {
		return &DrawCall[index * 4];
	}

	//���̃C���X�^���X�̃|�C���^��Ԃ��Ƃ������Ƃ�
	inline Interface::RectAppId Add(entt::entity entity, IType* pInstanceData[InstancePerEntity]) {
		if (InstanceCount >= InstanceMaxCount - 1) {
			pInstanceData = nullptr;
			return -1;
		}
		for (int i = 0; i < InstancePerEntity; i++) {
			pInstanceData[i] = &Instances[i * InstanceMaxCount + InstanceCount];
		}
		InstanceEntities[InstanceCount] = entity;
		InstanceCount++;
		return InstanceCount - 1;
	}
	inline void Update(Interface::RectAppId Id, IType* pInstanceData[InstancePerEntity]) {
		if (Id >= InstanceCount) {
			//throw("");
		}
		if (Id != -1) {
			for (int i = 0; i < InstancePerEntity; i++) {
				pInstanceData[i] = &Instances[i * InstanceMaxCount + Id];
			}
		}
	}
	// �폜���ɐ\��������������Ɣ������h���Ȃ��̂őS�`�F�b�N�ɂ���
	void EraseDeletedEntities(entt::registry* pRegistry) {
		for (int i = 0; i < InstanceCount; i++) {
			if (!pRegistry->valid(InstanceEntities[i])) {
				//���_�f�[�^�̈ړ�
				for (int j = 0; j < InstancePerEntity; j++) {
					Instances[i * InstancePerEntity + j] = Instances[(InstanceCount - 1) * InstancePerEntity + j];
					ZeroMemory(&Instances[(InstanceCount - 1) * InstancePerEntity + j], sizeof(IType));
				}
				//EntId�̈ړ�
				InstanceEntities[i] = InstanceEntities[InstanceCount - 1];
				// �ړ��������G���e�B�e�B�ɐV��������^����
				CompType* replacedComp = pRegistry->try_get<CompType>(InstanceEntities[i]);
				if (replacedComp!=nullptr) {
					if (replacedComp->BufferDataIndex > InstanceCount) {
						//throw("");
					}
					replacedComp->SetInstanceId(i);
				}
				//�J�E���g�����炷
				InstanceCount--;
				i--;
			}
		}
	}
};


template <class DCType, class IType,int InstancePerEntity>
class TimeBindAppearances {
public:
	int InstanceMaxCount;
	//���ۂ̃C���X�^���X�f�[�^
	std::vector<IType> Instances;
	// �폜����e�B�b�N
	std::vector<std::tuple<int, bool>> TickToDelete;
	//�h���[�R�[���̏��
	std::vector<DCType> DrawCall;
	//�e���ނ��Ƃ̓o�^��
	Interface::RectAppId InstanceCount;
	TimeBindAppearances(int instanceMaxCount) {
		InstanceMaxCount = instanceMaxCount;
		Instances = std::vector<IType>(InstanceMaxCount * InstancePerEntity);
		DrawCall = std::vector<DCType>(4 * InstancePerEntity);
		TickToDelete = std::vector<std::tuple<int,bool>>(InstanceMaxCount);
		InstanceCount = 0;
	}
	TimeBindAppearances() {
	}
	inline IType* GetInstancePointer(int index) {
		return &Instances[index * InstanceMaxCount];
	}
	inline DCType* GetDrawCallPointer(int index) {
		return &DrawCall[index * 4];
	}
	inline bool Add(int timeToDelete,bool isEternal, IType* pInstanceData[InstancePerEntity]) {
		if (InstanceCount >= InstanceMaxCount - 1) {
			pInstanceData = nullptr;
			return false;
		}
		for (int i = 0; i < InstancePerEntity; i++) {
			pInstanceData[i] = &Instances[i * InstanceMaxCount + InstanceCount];
		}
		TickToDelete[InstanceCount] = { Tick + timeToDelete ,isEternal};
		InstanceCount++;
		return true;
	}
	void EraseExpired() {
		std::vector<int> newIndex = std::vector<int>();
		int previousCount = InstanceCount;
		for (int i = 0; i < previousCount; i++) {
			if (std::get<0>(TickToDelete[i]) <= Tick && !std::get<1>(TickToDelete[i])) {
				InstanceCount--;
			}
			else {
				newIndex.push_back(i);
			}
		}
		int alivingCount = newIndex.size();
		for (int i = 0; i < alivingCount; i++) {
			//���_�f�[�^�̈ړ�
			for (int j = 0; j < InstancePerEntity; j++) {
				Instances[j * InstanceMaxCount + i] = Instances[j * InstanceMaxCount + newIndex[i]];
			}
			//EntId�̈ړ�
			TickToDelete[i] = TickToDelete[newIndex[i]];
		}
		for (int j = 0; j < InstancePerEntity; j++) {
			ZeroMemory(&Instances[j * InstanceMaxCount + alivingCount], sizeof(IType) * (previousCount - alivingCount));
		}
	}
	template<typename IAType>
	void EraseExpired(std::vector<IAType>* pInstanceAuxiliaries) {
		std::vector<int> newIndex = std::vector<int>();
		int previousCount = InstanceCount;
		for (int i = 0; i < previousCount; i++) {
			if (std::get<0>(TickToDelete[i]) <= Tick && !std::get<1>(TickToDelete[i])) {
				InstanceCount--;
			}
			else {
				newIndex.push_back(i);
			}
		}
		int alivingCount = newIndex.size();
		for (int i = 0; i < alivingCount; i++) {
			//���_�f�[�^�̈ړ�
			for (int j = 0; j < InstancePerEntity; j++) {
				Instances[j * InstanceMaxCount + i] = Instances[j * InstanceMaxCount + newIndex[i]];
			}
			//EntId�̈ړ�
			TickToDelete[i] = TickToDelete[newIndex[i]];
		}
		if (pInstanceAuxiliaries != nullptr) {
			for (int i = 0; i < alivingCount; i++) {
				(*pInstanceAuxiliaries)[i] = (*pInstanceAuxiliaries)[newIndex[i]];
			}
		}
		for (int j = 0; j < InstancePerEntity; j++) {
			ZeroMemory(&Instances[j * InstanceMaxCount + alivingCount], sizeof(IType) * (previousCount - alivingCount));
		}
	}
	void Clear() {
		InstanceCount = 0;
	}
};