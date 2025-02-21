#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
// Get Set�ŕς���
// �F
// �ʒu
// ���O��MotionInfo
// 
// �����ڂⓖ���蔻���h�������ԂŃf�[�^�����L�ł���I�u�W�F�N�g
// 

namespace Interface {
	//�v���g�^�C�v�ɂȂ��f�[�^�Ŏw�肵�������̂͂��ׂĂ���o�R
	/*
	//�v���g�^�C�v�ɂȂ��f�[�^�Ŏw�肵�������̂͂��ׂĂ���o�R
	struct BulletGenerateCoef {
		int PrototypeIndex;
		Damage DamageCoef;
		BulletGenerateCoef(int index) {
			PrototypeIndex = index;
			DamageCoef = Damage();
		}
	};
	struct TimeCounter {
		int CurrentTick;
		TimeCounter() {
			CurrentTick = 0;
		}
		void Reset() {
			CurrentTick = 0;
		}
		void CountUp() {
			CurrentTick++;
		}
	};
	*/



	//�`��o�b�t�@�ɓ����\����
	struct BlockInstanceType {
	public:
		void Set(DirectX::XMMATRIX* world, DirectX::XMVECTOR* texCoord12, DirectX::XMVECTOR* texCoord3M) {
			DirectX::XMStoreFloat2(&World, world->r[3]);
			DirectX::XMStoreFloat4(&TexCoord12, *texCoord12);
			DirectX::XMStoreFloat4(&TexCoord3M, *texCoord3M);
			Stain = DirectX::XMFLOAT4{
				0,0,0,0
			};
		}
		DirectX::XMFLOAT2 World;
		//�����e�N�X�`���摜���ł̈ʒu�̃C���f�b�N�X(���と�E�と�������E����),TexArray���ł̃C���f�b�N�X
		//�摜1,2,3,�}�X�N�̏�
		DirectX::XMFLOAT4 TexCoord12;
		//�����e�N�X�`���摜���ł̈ʒu�̃C���f�b�N�X(���と�E�と�������E����),TexArray���ł̃C���f�b�N�X
		//�摜1,2,3,�}�X�N�̏�
		DirectX::XMFLOAT4 TexCoord3M;

		DirectX::XMFLOAT4 Stain;
		
	};
	struct BlockDrawCallType {
	public:
		DirectX::XMFLOAT3 UV;//�e���_��U,�e���_��V,������
		DirectX::XMFLOAT4 Pos;
	};


	struct CharacterInstanceType {
	public:
		void Set(DirectX::XMMATRIX* world, DirectX::XMVECTOR* texCoordMask, DirectX::XMVECTOR* color0, DirectX::XMVECTOR* color1, DirectX::XMVECTOR* color2) {
			DirectX::XMStoreFloat4x4(&World, *world);
			DirectX::XMStoreFloat2(&TexCoordMask, *texCoordMask);
			DirectX::XMStoreFloat4(&Color0, *color0);
			DirectX::XMStoreFloat4(&Color1, *color1);
			DirectX::XMStoreFloat4(&Color2, *color2);
		}
		DirectX::XMFLOAT4X4 World;
		//�����e�N�X�`���摜���ł̈ʒu�̃C���f�b�N�X(���と�E�と�������E����),TexArray���ł̃C���f�b�N�X
		//�}�X�N�̏�
		DirectX::XMFLOAT2 TexCoordMask;
		//�}�X�N�ɏ]���ēh��F�̏��
		DirectX::XMFLOAT4 Color0;
		DirectX::XMFLOAT4 Color1;
		DirectX::XMFLOAT4 Color2;

	};
	struct CharacterDrawCallType {
	public:
		DirectX::XMFLOAT3 UV;//�e���_��U,�e���_��V,������
		DirectX::XMFLOAT4 Pos;
	};


	struct BulletInstanceType {
	public:
		void Set(DirectX::XMMATRIX* world, DirectX::XMVECTOR* texCoordMask, DirectX::XMVECTOR* color0, DirectX::XMVECTOR* color1) {
			DirectX::XMStoreFloat4x4(&World, *world);
			DirectX::XMStoreFloat2(&TexCoordMask, *texCoordMask);
			DirectX::XMStoreFloat4(&Color0, *color0);
			DirectX::XMStoreFloat4(&Color1, *color1);
		}
		DirectX::XMFLOAT4X4 World;
		//�����e�N�X�`���摜���ł̈ʒu�̃C���f�b�N�X(���と�E�と�������E����),TexArray���ł̃C���f�b�N�X
		//�}�X�N�̏�
		DirectX::XMFLOAT2 TexCoordMask;
		//�}�X�N�ɏ]���ēh��F�̏��
		DirectX::XMFLOAT4 Color0;
		DirectX::XMFLOAT4 Color1;

	};
	struct BulletDrawCallType {
	public:
		DirectX::XMFLOAT3 UV;//�e���_��U,�e���_��V,������
		DirectX::XMFLOAT4 Pos;
	};



	struct ConstantType
	{
	public:
		DirectX::XMFLOAT4X4 ViewProjection;
	};





	typedef int EntId;
#define ComponentCount 32
#define TeamCount 2
	typedef std::bitset<ComponentCount> RawArchetype;
	typedef int HostilityTeam;
	typedef int ArchetypeIndex;
	typedef int SameArchIndex;
	typedef int RectAppId;
	struct RelationOfCoord {
		float Rotate;
		float Ratio;
		DirectX::XMVECTOR Parallel;
		RelationOfCoord operator+(RelationOfCoord& other) {
			RelationOfCoord output;
			output.Rotate = Rotate + other.Rotate;
			output.Ratio = Ratio * other.Ratio;
			output.Parallel = DirectX::XMVectorAdd(Parallel,other.Parallel);
			output.Parallel.m128_f32[3] = 1;
			return output;
		}
		RelationOfCoord operator*(RelationOfCoord& other) {
			RelationOfCoord output;
			output.Rotate = Rotate + other.Rotate;
			output.Ratio = Ratio * other.Ratio;
			output.Parallel = DirectX::XMVectorAdd(
				DirectX::XMVector4Transform(
				DirectX::XMVectorScale(Parallel,other.Ratio),
					DirectX::XMMatrixRotationZ(other.Rotate))
				, other.Parallel);
			output.Parallel.m128_f32[3] = 1;
			return output;
		}
		DirectX::XMMATRIX GetMatrix() {
			DirectX::XMMATRIX output{
				Ratio,0.0f,0.0f,0.0f,
				0.0f,Ratio,0.0f,0.0f,
				0.0f,0.0f,1.0f,0.0f,
				0.0f,0.0f,0.0f,1.0f,
			};
			output = output * DirectX::XMMatrixRotationZ(Rotate);
			output.r[3] = Parallel;
			return output;
		}
		void Identity() {
			Parallel = { 0.0f,0.0f, 0.0f, 0.0f };
			Rotate = 0;
			Ratio = 1;
		}
	};
	static DirectX::XMVECTOR operator*(DirectX::XMVECTOR& prescding, Interface::RelationOfCoord subsequent) {
		DirectX::XMVECTOR output;
		output = DirectX::XMVectorAdd(
			DirectX::XMVector4Transform(
				DirectX::XMVectorScale(prescding, subsequent.Ratio),
				DirectX::XMMatrixRotationZ(subsequent.Rotate))
			, subsequent.Parallel);
		return output;
	}
#define CompCount 32
	struct EntityPointer {
		EntityPointer() {
			Archetype = -1;
			Index = -1;
		}
		EntityPointer(Interface::ArchetypeIndex archetype, Interface::SameArchIndex index) {
			Archetype = archetype;
			Index = index;
		}
		Interface::ArchetypeIndex Archetype;
		Interface::SameArchIndex Index;
		bool operator==(Interface::EntityPointer& other) {
			return (Archetype == other.Archetype) && (Index = other.Index);
		}
	};
	struct Damage {
	public:
		float physical;
		HostilityTeam Team;
		Damage() {
			physical = 0;
		}
		Damage operator+(Damage& other) {
			Damage output;
			output.physical = other.physical + physical;
			return output;
		}
		void operator+=(Damage& other) {
			physical = other.physical + physical;
		}
		Damage(Json::Value fromLoad) {
			physical = fromLoad.get("physical", "").asFloat();
		}
	};
	struct IntXY {
		UINT8 X;
		UINT8 Y;
	};
	struct CircleCollidbox {
		DirectX::XMVECTOR RelativeCenter;
		float radius;
	};

	static bool SameString(std::string A, std::string B) {
		if (A.length() != B.length()) {
			return false;
		}
		int size = A.length();
		for (int i = 0; i < size; i++) {
			if (A[i] != B[i]) {
				return false;
			}
		}
		return true;
	}

	struct CombatUnitInitData {
		float HealthMultiply;
		float DamageMultiply;
		float SpeedMultiply;
		Interface::HostilityTeam Team;
		Interface::EntId CoreId;
		bool isCore;
		CombatUnitInitData() {
			HealthMultiply = 1;
			DamageMultiply = 1;
			SpeedMultiply = 1;
		}
	};
	static std::map<std::string, Interface::EntityPointer> EntNameHash;
	static std::map<std::string, int> AppearanceNameHash;
	static Interface::EntId PlayingCharacter;
	static std::bitset<256 * 256> OccupiedMap;
	static std::bitset<64> HostilityTable;//8team*8team�̃e�[�u����true�̏ꏊ�͓G�΂���
	inline DirectX::XMVECTOR GetVectorFromJson(Json::Value input) {
		DirectX::XMVECTOR output = {
			input[0].asFloat(),
			input[1].asFloat(),
			input[2].asFloat(),
			input[3].asFloat(),
		};
		return output;
	}
	struct UnitInfo {
		std::vector<Interface::EntityPointer> PrototypePointer;
		std::vector<DirectX::XMVECTOR> RelativePosFromCore;
		std::vector<DirectX::XMVECTOR> BaseColor0;
		std::vector<DirectX::XMVECTOR> BaseColor1;
		UnitInfo() {
			PrototypePointer = std::vector<Interface::EntityPointer>();
			RelativePosFromCore = std::vector<DirectX::XMVECTOR>();
			BaseColor0 = std::vector<DirectX::XMVECTOR>();
			BaseColor1 = std::vector<DirectX::XMVECTOR>();
		}
		UnitInfo(Json::Value jsonOfThisUnit) {
			PrototypePointer = std::vector<Interface::EntityPointer>();
			RelativePosFromCore = std::vector<DirectX::XMVECTOR>();
			BaseColor0 = std::vector<DirectX::XMVECTOR>();
			BaseColor1 = std::vector<DirectX::XMVECTOR>();
			for (int i = 0; i < jsonOfThisUnit.size(); i++) {
				PrototypePointer.push_back(EntNameHash[jsonOfThisUnit[i].get("characterName", "").asString()]);
				RelativePosFromCore.push_back({
					jsonOfThisUnit[i].get("relativePos","")[0].asFloat(),
					jsonOfThisUnit[i].get("relativePos","")[1].asFloat(),
					0,1
				});
				BaseColor0.push_back(Interface::GetVectorFromJson(jsonOfThisUnit[i].get("baseColor0", "")));
				BaseColor1.push_back(Interface::GetVectorFromJson(jsonOfThisUnit[i].get("baseColor1", "")));
			}
		}
	};
	static std::map<std::string, Interface::UnitInfo> UnitInfoTable;
};
