#include "Entity.h"
#define AddCompMacro(CompName) CompName.Add(Prototypes->CompName, pInitData)
#define DeleteCompMacro(CompName) CompName.Delete(toDelete)
#define InitCompWrapper(CompName) CompName = ComponentWrapper<Component::CompName>(thisArchetype, CompNames::CompName)
#define InitPrototypeCompWrapper(CompName) Prototypes->CompName.Add(oneObject, CompNameArray[CompNames::CompName])

// coreWorld��core�ł���ꍇ��core�̈ʒu�Acore�łȂ��ꍇ��core�Ƃ̈ʒu�֌W
Interface::SameArchIndex SameArchetype::Add(Interface::EntId nextId, Interface::EntityInitData* pInitData) {
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
	AddCompMacro(BallHurtbox);
	AddCompMacro(UnitOccupationbox);
	AddCompMacro(WallHurtbox);
	AddCompMacro(GiveDamage);
	AddCompMacro(DamagePool);
	//�����ɂ�����鐫��
	AddCompMacro(BallData);
	AddCompMacro(UnitData);
	AddCompMacro(BulletData);
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	AddCompMacro(HitEffect);
	AddCompMacro(Trajectory);
	//�����ڂɂ�����鐫��
	AddCompMacro(BlockAppearance);
	AddCompMacro(BallAppearance);
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
	DeleteCompMacro(BallHurtbox);
	DeleteCompMacro(UnitOccupationbox);
	DeleteCompMacro(WallHurtbox);
	DeleteCompMacro(GiveDamage);
	DeleteCompMacro(DamagePool);
	//�����ɂ�����鐫��
	DeleteCompMacro(BallData);
	DeleteCompMacro(UnitData);
	DeleteCompMacro(BulletData);
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	DeleteCompMacro(HitEffect);
	DeleteCompMacro(Trajectory);
	//�����ڂɂ�����鐫��
	DeleteCompMacro(BlockAppearance);
	DeleteCompMacro(BallAppearance);
	DeleteCompMacro(BulletAppearance);
	//�^���ɂ�����鐫��
	DeleteCompMacro(Motion);
	DeleteCompMacro(LinearAcceralation);
	DeleteCompMacro(WorldPosition);
	//�~�b�V�����i�s�ɂ�����鐫��
	DeleteCompMacro(InvationObservance);
	DeleteCompMacro(UnitCountObservance);
	DeleteCompMacro(Spawn);
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
	RealEntityCount = 0;

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
	InitCompWrapper(BallHurtbox);
	InitCompWrapper(UnitOccupationbox);
	InitCompWrapper(WallHurtbox);
	InitCompWrapper(GiveDamage);
	InitCompWrapper(DamagePool);
	//�����ɂ�����鐫��
	InitCompWrapper(BallData);
	InitCompWrapper(UnitData);
	InitCompWrapper(BulletData);
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	InitCompWrapper(HitEffect);
	InitCompWrapper(Trajectory);
	//�����ڂɂ�����鐫��
	InitCompWrapper(BlockAppearance);
	InitCompWrapper(BallAppearance);
	InitCompWrapper(BulletAppearance);
	//�^���ɂ�����鐫��
	InitCompWrapper(Motion);
	InitCompWrapper(LinearAcceralation);
	InitCompWrapper(WorldPosition);
	//�~�b�V�����i�s�ɂ�����鐫��
	InitCompWrapper(InvationObservance);
	InitCompWrapper(UnitCountObservance);
	InitCompWrapper(Spawn);
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
	InitPrototypeCompWrapper(BallHurtbox);
	InitPrototypeCompWrapper(UnitOccupationbox);
	InitPrototypeCompWrapper(WallHurtbox);

	InitPrototypeCompWrapper(GiveDamage);
	InitPrototypeCompWrapper(DamagePool);
	//�����ɂ�����鐫��
	InitPrototypeCompWrapper(BallData);
	InitPrototypeCompWrapper(UnitData);
	InitPrototypeCompWrapper(BulletData);
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	InitPrototypeCompWrapper(HitEffect);
	InitPrototypeCompWrapper(Trajectory);
	//�����ڂɂ�����鐫��
	InitPrototypeCompWrapper(BlockAppearance);
	InitPrototypeCompWrapper(BallAppearance);
	InitPrototypeCompWrapper(BulletAppearance);
	//�^���ɂ�����鐫��
	InitPrototypeCompWrapper(Motion);
	InitPrototypeCompWrapper(LinearAcceralation);
	InitPrototypeCompWrapper(WorldPosition);
	//�~�b�V�����i�s�ɂ�����鐫��
	InitPrototypeCompWrapper(InvationObservance);
	InitPrototypeCompWrapper(UnitCountObservance);
	InitPrototypeCompWrapper(Spawn);
	//�e�X�g�p
	InitPrototypeCompWrapper(Test1);
	InitPrototypeCompWrapper(Test2);
	InitPrototypeCompWrapper(TestResult);
	InitPrototypeCompWrapper(Test3);
}
void AllEntities::LoadEntities(std::vector<std::string> fileNames) {
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
}
void AllEntities::LoadUnits(std::vector<std::string> filePathes) {
	for (int i = 0; i < filePathes.size(); i++) {
		Json::Value root;
		std::ifstream file = std::ifstream(filePathes[i]);
		file >> root;
		file.close();
		auto unitNames = root.getMemberNames();
		for (int i = 0; i < unitNames.size(); i++) {
			UnitInfos.push_back(UnitInfo(root.get(unitNames[i], ""),this));
			Interface::UnitNameHash.emplace(std::pair<std::string, int>(unitNames[i], i));
		}
	}
}
Interface::EntId AllEntities::AddUnitWithMagnification(int unitIndex, Interface::EntityInitData* pInitData, Interface::RelationOfCoord* CorePos) {
	Interface::EntId core;
	pInitData->Pos = *CorePos;
	pInitData->Prototype = UnitInfos[unitIndex].CorePrototype;

	pInitData->IsCore = true;
	// �����̃T�C�Y�̎w��
	pInitData->Ratio = UnitInfos[unitIndex].Ratio;

	core = AddFromEntPointer(pInitData);
	pInitData->CoreId = core;
	pInitData->IsCore = false;
	//1�̃��j�b�g�ɂ�7���̃{�[����u��
	for (int i = 0; i < 7; i++) {
		pInitData->BaseColor0 = UnitInfos[unitIndex].BaseColor0[i];
		pInitData->BaseColor1 = UnitInfos[unitIndex].BaseColor1[i];
		pInitData->Pos = UnitInfos[unitIndex].RelativePosFromCore[i];
		pInitData->Prototype = UnitInfos[unitIndex].PrototypePointer[i];
		AddFromEntPointer(pInitData);
	}
	return core;
}
void AllEntities::LoadMission(std::string fileName) {
	Json::Value root;
	std::ifstream file = std::ifstream(fileName);
	file >> root;
	file.close();
	Json::Value EnemySpawnCondition = root.get("enemySpawnCondition", "");
	// �G���A�N���ɂ���ăX�|�[��
	{
		Interface::RawArchetype rawArchetype;
		rawArchetype.reset();
		rawArchetype.set(CompNames::Spawn);
		rawArchetype.set(CompNames::InvationObservance);
		rawArchetype.set(CompNames::GenerateFlag);
		rawArchetype.set(CompNames::KillFlag);
		Archetypes.push_back(rawArchetype);
		EntityArraies.push_back(SameArchetype(rawArchetype, true));
		for (int i = 0; i < EnemySpawnCondition.size(); i++) {
			if (Interface::SameString(EnemySpawnCondition[i].get("conditionType", "").asString(), "areaInvation")) {
				EntityArraies.back().Spawn.Add(root.get("enemySpawnCondition","")[i], "");
				EntityArraies.back().InvationObservance.Add(root.get("enemySpawnCondition", "")[i], "");
			}
		}
	}
	// ���j�b�g�c���ɂ���ăX�|�[��
	{
		Interface::RawArchetype rawArchetype;
		rawArchetype.reset();
		rawArchetype.set(CompNames::Spawn);
		rawArchetype.set(CompNames::UnitCountObservance);
		rawArchetype.set(CompNames::GenerateFlag);
		rawArchetype.set(CompNames::KillFlag);
		Archetypes.push_back(rawArchetype);
		EntityArraies.push_back(SameArchetype(rawArchetype, true));
		for (int i = 0; i < EnemySpawnCondition.size(); i++) {
			if (Interface::SameString(EnemySpawnCondition[i].get("conditionType", "").asString(), "unitCount")) {
				EntityArraies.back().Spawn.Add(root.get("enemySpawnCondition", "")[i], "");
				EntityArraies.back().UnitCountObservance.Add(root.get("enemySpawnCondition", "")[i], "");
				EntityArraies.back().GenerateFlag.Add(root.get("enemySpawnCondition", "")[i], "");
				EntityArraies.back().KillFlag.Add(root.get("enemySpawnCondition", "")[i], "");
				EntityArraies.back().ValidEntityCount++;
			}
		}
	}
}