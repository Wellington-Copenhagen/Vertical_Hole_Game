#pragma once
#include "framework.h"
#include "Component.h"
#include "Interfaces.h"
class AllEntities {
	//bitset�̈�����Component�̐�����
	std::vector<std::bitset<32>> Archetypes;
	std::vector<SameArchetype> EntityArraies;
	void Load(std::string filePath);
	EntityPointer INtoIndex[UINT16_MAX * 4];
	inline void PushToDeleteFromEIN(EntityIdentifyN instanceEIN);
	inline void AddFromEIN(EntityIdentifyN prototypeEIN);
};
//����Archetype������Entity�̏W��
class SameArchetype {
public:
	//�V�����G���e�B�e�B�����Ƃ��̕t�����͂��̊֐��̈����Ƀ|�C���^���w�肷�邭�炢�����Ă͂Ȃ�(�Ȃ��Ȃ�nullptr)
	inline int Add(int prototypeIndex);
	//�폜����C�ɂ��Ȃ��Ɛ������̖��ɂȂ肻��
	inline void PushToDelete(int entityIndex);
	inline void Delete();

	SameArchetype() {
		GlobalIdentityNumber = std::vector<int>();
		CompTextureData = std::vector<Component::RectAppearance>();
	}
	//����͓ǂݍ��ݎ��p(Prototypes�̒��ł����Ă΂Ȃ�)
	void AddFromFile();

	SameArchetype Prototypes;
	//�S�I�u�W�F�N�g�ɌŗL�Ŋ��蓖�Ă�
	//�I�u�W�F�N�g�Ԃ̎��ʗp
	std::vector<EntityIdentifyN> GlobalIdentityNumber;
	//�����艺���eComponent�̃f�[�^
	std::vector<Component::RectAppearance> CompTextureData;
};