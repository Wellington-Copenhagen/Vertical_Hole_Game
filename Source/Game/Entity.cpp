#include "Entity.h"
#define AddCompMacro(CompName) CompName.Add(Prototypes->CompName, prototypeIndex)
#define DeleteCompMacro(CompName) CompName.Delete(toDelete)
#define InitCompWrapper(CompName) CompName = ComponentWrapper<Component::CompName>(thisArchetype, CompNames::CompName)
#define InitPrototypeCompWrapper(CompName) Prototypes->CompName.Add(oneObject, CompNameArray[CompNames::CompName])

// coreWorld��core�ł���ꍇ��core�̈ʒu�Acore�łȂ��ꍇ��core�Ƃ̈ʒu�֌W
Interface::SameArchIndex SameArchetype::Add(Interface::SameArchIndex prototypeIndex, Interface::EntId nextId,
	Interface::CombatUnitInitData* pCombatUnitInit,Interface::RelationOfCoord* coreWorld,int initialMaskCoord,
	bool isCore,Interface::EntId positionReference) {
	IdArray.push_back(nextId);

	// Component�̎�ނ��������炱���𑝂₷�K�v������
	AddCompMacro(HitFlag);
	AddCompMacro(KillFlag);
	AddCompMacro(GenerateFlag);
	AddCompMacro(MoveFlag);
	AddCompMacro(AppearanceChanged);
	AddCompMacro(PositionReference);
	//AI�Ɋւ��鐫��
	AddCompMacro(AI);
	//�����蔻��ɂ�����鐫��
	AddCompMacro(CircleHitbox);
	AddCompMacro(CharacterHurtbox);
	AddCompMacro(WallHurtbox);
	AddCompMacro(GiveDamage);
	AddCompMacro(DamagePool);
	//�����ɂ�����鐫��
	AddCompMacro(CharacterData);
	AddCompMacro(BulletData);
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	AddCompMacro(HitEffect);
	AddCompMacro(Trajectory);
	//�����ڂɂ�����鐫��
	AddCompMacro(BlockAppearance);
	AddCompMacro(CharacterAppearance);
	AddCompMacro(BulletAppearance);
	//�^���ɂ�����鐫��
	AddCompMacro(Motion);
	AddCompMacro(LinearAcceralation);
	AddCompMacro(WorldPosition);
	//�e�X�g�p
	AddCompMacro(Test1);
	AddCompMacro(Test2);
	AddCompMacro(TestResult);
	AddCompMacro(Test3);
	if (GenerateFlag.UsedFlag) {
		GenerateFlag.Components.back().Init();
	}
	if (KillFlag.UsedFlag) {
		KillFlag.Components.back().Init();
	}
	if (MoveFlag.UsedFlag) {
		MoveFlag.Components.back().Init();
	}
	if (AppearanceChanged.UsedFlag) {
		AppearanceChanged.Components.back().Init();
	}
	if (HitFlag.UsedFlag) {
		HitFlag.Components.back().Init();
	}
	if (CharacterData.UsedFlag&& pCombatUnitInit != nullptr) {
		CharacterData.Components.back().Init(pCombatUnitInit);
	}
	if (WorldPosition.UsedFlag && coreWorld != nullptr) {
		WorldPosition.Components.back().Init(coreWorld,isCore);
	}
	if (PositionReference.UsedFlag) {
		if (isCore) {
			PositionReference.Components.back().Init(isCore, IdArray.back());
		}
		else {
			PositionReference.Components.back().Init(isCore, positionReference);
		}
	}
	if (BlockAppearance.UsedFlag) {
		BlockAppearance.Components.back().Init(initialMaskCoord);
	}
	//Motion.Components.back().Init();
	//Rotate.Components.back().Init();
	return IdArray.size() - 1;
}
//toDelete�ɂ���f�[�^���폜���ĊԂ𖄂߂邽�߂Ɉړ�������Entity��EntId��Ԃ�
//�������A�Ō��Entity���폜�����ꍇ��-1��Ԃ�
void SameArchetype::Delete(AllEntities* pAllEntities,Interface::SameArchIndex toDelete){
	//AllEntities��INtoIndex���X�V���鏈��
	pAllEntities->INtoIndex[IdArray.back()].Index = toDelete;

	//IdArray�̃f�[�^����������
	IdArray[toDelete] = IdArray.back();
	IdArray.pop_back();

	// Component�̎�ނ��������炱���𑝂₷�K�v������
	DeleteCompMacro(HitFlag);
	DeleteCompMacro(KillFlag);
	DeleteCompMacro(GenerateFlag);
	DeleteCompMacro(MoveFlag);
	DeleteCompMacro(AppearanceChanged);
	DeleteCompMacro(PositionReference);
	//AI�Ɋւ��鐫��
	DeleteCompMacro(AI);
	//�����蔻��ɂ�����鐫��
	DeleteCompMacro(CircleHitbox);
	DeleteCompMacro(CharacterHurtbox);
	DeleteCompMacro(WallHurtbox);
	DeleteCompMacro(GiveDamage);
	DeleteCompMacro(DamagePool);
	//�����ɂ�����鐫��
	DeleteCompMacro(CharacterData);
	DeleteCompMacro(BulletData);
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	DeleteCompMacro(HitEffect);
	DeleteCompMacro(Trajectory);
	//�����ڂɂ�����鐫��
	DeleteCompMacro(BlockAppearance);
	DeleteCompMacro(CharacterAppearance);
	DeleteCompMacro(BulletAppearance);
	//�^���ɂ�����鐫��
	DeleteCompMacro(Motion);
	DeleteCompMacro(LinearAcceralation);
	DeleteCompMacro(WorldPosition);
	//�e�X�g�p
	DeleteCompMacro(Test1);
	DeleteCompMacro(Test2);
	DeleteCompMacro(TestResult);
	DeleteCompMacro(Test3);
}
SameArchetype::SameArchetype(Interface::RawArchetype thisArchetype, bool IsPrototype) {
	PrototypeCount = 0;
	ThisArchetype = thisArchetype;
	if (!IsPrototype) {
		Prototypes = new SameArchetype(thisArchetype, true);
	}
	IdArray = std::vector<Interface::EntId>();
	ValidEntityCount = 0;

	//���ʂ���f�[�^
	InitCompWrapper(HitFlag);
	InitCompWrapper(KillFlag);
	InitCompWrapper(GenerateFlag);
	InitCompWrapper(MoveFlag);
	InitCompWrapper(AppearanceChanged);
	InitCompWrapper(PositionReference);
	//AI�Ɋւ��鐫��
	InitCompWrapper(AI);
	//�����蔻��ɂ�����鐫��
	InitCompWrapper(CircleHitbox);
	InitCompWrapper(CharacterHurtbox);
	InitCompWrapper(WallHurtbox);
	InitCompWrapper(GiveDamage);
	InitCompWrapper(DamagePool);
	//�����ɂ�����鐫��
	InitCompWrapper(CharacterData);
	InitCompWrapper(BulletData);
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	InitCompWrapper(HitEffect);
	InitCompWrapper(Trajectory);
	//�����ڂɂ�����鐫��
	InitCompWrapper(BlockAppearance);
	InitCompWrapper(CharacterAppearance);
	InitCompWrapper(BulletAppearance);
	//�^���ɂ�����鐫��
	InitCompWrapper(Motion);
	InitCompWrapper(LinearAcceralation);
	InitCompWrapper(WorldPosition);
	//�e�X�g�p
	InitCompWrapper(Test1);
	InitCompWrapper(Test2);
	InitCompWrapper(TestResult);
	InitCompWrapper(Test3);
}
void SameArchetype::LoadToPrototype(Json::Value oneObject) {
	//���ʂ���f�[�^
	InitPrototypeCompWrapper(HitFlag);
	InitPrototypeCompWrapper(KillFlag);
	InitPrototypeCompWrapper(GenerateFlag);
	InitPrototypeCompWrapper(MoveFlag);
	InitPrototypeCompWrapper(AppearanceChanged);
	InitPrototypeCompWrapper(PositionReference);
	//AI�Ɋւ��鐫��
	InitPrototypeCompWrapper(AI);
	//�����蔻��ɂ�����鐫��
	InitPrototypeCompWrapper(CircleHitbox);
	InitPrototypeCompWrapper(CharacterHurtbox);
	InitPrototypeCompWrapper(WallHurtbox);
	InitPrototypeCompWrapper(GiveDamage);
	InitPrototypeCompWrapper(DamagePool);
	//�����ɂ�����鐫��
	InitPrototypeCompWrapper(CharacterData);
	InitPrototypeCompWrapper(BulletData);
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	InitPrototypeCompWrapper(HitEffect);
	InitPrototypeCompWrapper(Trajectory);
	//�����ڂɂ�����鐫��
	InitPrototypeCompWrapper(BlockAppearance);
	InitPrototypeCompWrapper(CharacterAppearance);
	InitPrototypeCompWrapper(BulletAppearance);
	//�^���ɂ�����鐫��
	InitPrototypeCompWrapper(Motion);
	InitPrototypeCompWrapper(LinearAcceralation);
	InitPrototypeCompWrapper(WorldPosition);
	//�e�X�g�p
	InitPrototypeCompWrapper(Test1);
	InitPrototypeCompWrapper(Test2);
	InitPrototypeCompWrapper(TestResult);
	InitPrototypeCompWrapper(Test3);
}
std::map<std::string, Interface::EntityPointer> AllEntities::LoadFromFile(std::vector<std::string> fileNames) {
	//�܂��S�G���e�B�e�B�̖��O�ƃv���g�^�C�v��EntPointer��R�Â���
	for (int i = 0; i < fileNames.size(); i++) {
		Json::Value root;
		std::ifstream file = std::ifstream(fileNames[i]);
		file >> root;
		file.close();
		//�G���e�B�e�B���̈ꗗ���擾
		Json::Value::Members entNames = root.getMemberNames();
		for (int j = 0; j < entNames.size(); j++) {
			Interface::EntityPointer prototypePointer;
			//�R���|�[�l���g���̈ꗗ�𓾂ăA�[�L�^�C�v�ɕϊ����A���ɓo�^�ς݂̃A�[�L�^�C�v�����ׂ�
			Json::Value::Members compNames = root.get(entNames[j], "").getMemberNames();
			Interface::RawArchetype rawArchetype = GetArchetypeFromCompNames(compNames);
			auto itr = std::find(Archetypes.begin(), Archetypes.end(), rawArchetype);

			if (itr == Archetypes.end()) {
				//���o�^
				Archetypes.push_back(rawArchetype);
				EntityArraies.push_back(SameArchetype(rawArchetype, false));
				prototypePointer.Archetype = Archetypes.size() - 1;
				prototypePointer.Index = 0;
				EntityArraies.back().PrototypeCount++;
			}
			else {
				//�o�^�ς�
				prototypePointer.Archetype = std::distance(Archetypes.begin(), itr);
				prototypePointer.Index = EntityArraies[prototypePointer.Archetype].PrototypeCount;
				EntityArraies[prototypePointer.Archetype].PrototypeCount++;
			}
			std::string name = entNames[j];
			bool inserted = Interface::EntNameHash.emplace(std::pair<std::string, Interface::EntityPointer>(name, prototypePointer)).second;
			int a = Interface::EntNameHash.size();
		}
	}
	//���Ɏ��ۂ̃f�[�^��ǂݍ���ł���
	for (int i = 0; i < fileNames.size(); i++) {
		Json::Value root;
		std::ifstream file = std::ifstream(fileNames[i]);
		file >> root;
		file.close();
		//�G���e�B�e�B���̈ꗗ���擾
		Json::Value::Members entNames = root.getMemberNames();
		for (int j = 0; j < entNames.size(); j++) {
			Interface::EntityPointer prototypePointer = Interface::EntNameHash[entNames[j]];
			EntityArraies[prototypePointer.Archetype].LoadToPrototype(root.get(entNames[j], ""));
		}
	}
	return Interface::EntNameHash;
}