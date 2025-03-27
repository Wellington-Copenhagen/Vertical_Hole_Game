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
#define MaxUnitCount 1024
#define MaxBallCount MaxUnitCount * 7
#define MaxBulletCount 65536
#define MaxLineCount 65536
#define WorldWidth 256
#define WorldHeight 256

namespace Interface {
	static void OutputMatrix(DirectX::XMMATRIX matrix,std::string suffix, std::string prefix) {
		OutputDebugStringA(suffix.c_str());
		for (int i = 0; i < 4; i++) {
			OutputDebugStringA("[");
			for (int j = 0; j < 4; j++) {
				std::string output = std::to_string(matrix.r[i].m128_f32[j]);
				OutputDebugStringA(output.c_str());
				OutputDebugStringA(" ");
			}
			OutputDebugStringA("]\n");
		}
		OutputDebugStringA(prefix.c_str());
	}
	static void OutputVector(DirectX::XMVECTOR vector, std::string suffix, std::string prefix) {
		OutputDebugStringA(suffix.c_str());
		OutputDebugStringA("[");
		for (int j = 0; j < 4; j++) {
			std::string output = std::to_string(vector.m128_f32[j]);
			OutputDebugStringA(output.c_str());
			OutputDebugStringA(" ");
		}
		OutputDebugStringA("]\n");
		OutputDebugStringA(prefix.c_str());
	}
	// ������\��������

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


	struct BallInstanceType {
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
	struct BallDrawCallType {
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


	struct CharInstanceType {
	public:
		void Set(DirectX::XMMATRIX* world, int texIndex, DirectX::XMVECTOR* color) {
			DirectX::XMStoreFloat4x4(&World, *world);
			TexIndex = (float)texIndex;
			DirectX::XMStoreFloat4(&Color, *color);
		}
		DirectX::XMFLOAT4X4 World;
		// �����C���f�b�N�X
		float TexIndex;
		//�����̐F
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT2 BlackBoxShape;

	};
	struct CharDrawCallType {
	public:
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
		DirectX::XMFLOAT4 Pos;
	};


	struct IconInstanceType {
		DirectX::XMFLOAT4 Vertex1;
		DirectX::XMFLOAT4 Vertex2;
		DirectX::XMFLOAT4 Vertex3;
		DirectX::XMFLOAT4 Vertex4;
		float texIndex;
	};
	struct IconDrawCallType {
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
	};

	struct LineInstanceType {
		DirectX::XMFLOAT4 Vertex1;
		DirectX::XMFLOAT4 Vertex2;
		float width;
		float texIndex;
	};
	struct LineDrawCallType {
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
	};

	struct OCGInstanceType {
	public:
		void Set(DirectX::XMVECTOR* vertex1, DirectX::XMVECTOR* vertex2, DirectX::XMVECTOR* vertex3, DirectX::XMVECTOR* vertex4, 
			DirectX::XMVECTOR* color1, DirectX::XMVECTOR* color2,float colorChange) {
			DirectX::XMStoreFloat4(&Vertex1, *vertex1);
			DirectX::XMStoreFloat4(&Vertex2, *vertex2);
			DirectX::XMStoreFloat4(&Vertex3, *vertex3);
			DirectX::XMStoreFloat4(&Vertex4, *vertex4);
			DirectX::XMStoreFloat4(&Color1, *color1);
			DirectX::XMStoreFloat4(&Color2, *color2);
			ColorChange = colorChange;
		}
		DirectX::XMFLOAT4 Vertex1;
		DirectX::XMFLOAT4 Vertex2;
		DirectX::XMFLOAT4 Vertex3;
		DirectX::XMFLOAT4 Vertex4;
		DirectX::XMFLOAT4 Color1;
		DirectX::XMFLOAT4 Color2;
		float ColorChange;

	};
	struct OCGDrawCallType {
	public:
		float Triangle;
	};



	struct ConstantType
	{
	public:
		DirectX::XMFLOAT4X4 ViewProjection;
	};





#define ComponentCount 32
#define TeamCount 2
	typedef int HostilityTeam;
	typedef int RectAppId;
	typedef int UnitIndex;
	struct RelationOfCoord {
		float Rotate;
		float Ratio;
		DirectX::XMVECTOR Parallel;
		RelationOfCoord() {
			Rotate = 0;
			Ratio = 1;
			Parallel = { 0,0,0,1 };
		}
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
	static std::map<std::string, int> AppearanceNameHash;
	inline entt::entity PlayingUnit;
	static std::bitset<TeamCount*TeamCount> HostilityTable;//8team*8team�̃e�[�u����true�̏ꏊ�͓G�΂���
	static std::mt19937 RandEngine;
	inline DirectX::XMVECTOR GetVectorFromJson(Json::Value input) {
		DirectX::XMVECTOR output = {
			input[0].asFloat(),
			input[1].asFloat(),
			input[2].asFloat(),
			input[3].asFloat(),
		};
		return output;
	}
	inline std::map<std::string, Interface::UnitIndex> UnitNameHash;
	inline std::map<std::string, entt::entity> EntNameHash;
	struct EntityInitData {
		// ��Ɏg��
		entt::entity thisEntities;
		entt::entity Prototype;
		bool IsCore;
		// Effect��Ball�Ɏg��
		float HealthMultiply;
		float DamageMultiply;
		float SpeedMultiply;
		Interface::HostilityTeam Team;
		// Ball�Ɏg��
		entt::entity CoreId;
		bool IsLeader;
		entt::entity LeaderId;
		RelationOfCoord Pos;//Core�̏ꍇ��Core�̈ʒu�ACore�ł͂Ȃ��Ȃ�Core�ɑ΂��Ă̈ʒu
		DirectX::XMVECTOR BaseColor0;
		DirectX::XMVECTOR BaseColor1;
		float SplintTilePerTick[6];
		float RadianPerTick;
		float MoveTilePerTick;
		float Ratio;
		float Weight;
		// Block�Ɏg��
		bool OnTop;
		bool Breakable;
		entt::entity DamageObserveEntity;
		EntityInitData() {

		}
		EntityInitData(Json::Value fromLoad) {
			HealthMultiply = fromLoad.get("health", "").asFloat();
			DamageMultiply = fromLoad.get("attack", "").asFloat();
			SpeedMultiply = fromLoad.get("speed", "").asFloat();
			Team = fromLoad.get("team", "").asInt();
		}
	};
	enum WhatDoing {
		Moving,
		Chasing,
	};
	struct Order {
		WhatDoing Doing;
		entt::entity Target;
		DirectX::XMVECTOR MoveFor;

	}; 
	struct UnitInfo {
		entt::entity CorePrototype;
		entt::entity Prototypes[7];
		Interface::RelationOfCoord RelativePosFromCore[7];
		DirectX::XMVECTOR BaseColor0[7];
		DirectX::XMVECTOR BaseColor1[7];
		float Ratio;
		UnitInfo() {

		}
		UnitInfo(Json::Value jsonOfThisUnit, entt::registry* pMainRegistry) {
			CorePrototype = Interface::EntNameHash[jsonOfThisUnit.get("coreName", "").asString()];
			Ratio = jsonOfThisUnit.get("ratio", "").asFloat();
			for (int i = 0; i < 7; i++) {
				Prototypes[i] = Interface::EntNameHash[jsonOfThisUnit.get("balls", "")[i].get("ballName", "").asString()];
				Interface::RelationOfCoord toPush;
				//�R�A�ɑ΂��Ă̏����ʒu
				toPush.Parallel = DirectX::XMVECTOR{
					std::cosf(jsonOfThisUnit.get("balls","")[i].get("pos","").asInt() * PI / 3) / 3,
					std::sinf(jsonOfThisUnit.get("balls","")[i].get("pos","").asInt() * PI / 3) / 3,
					0,1
				};
				if (i == 0) {
					toPush.Parallel = { 0,0,0,1 };
				}
				toPush.Ratio = 1;
				toPush.Rotate = 0;
				RelativePosFromCore[i] = toPush;
				BaseColor0[i] = Interface::GetVectorFromJson(jsonOfThisUnit.get("balls", "")[i].get("baseColor0", ""));
				BaseColor1[i] = Interface::GetVectorFromJson(jsonOfThisUnit.get("balls", "")[i].get("baseColor1", ""));
			}
		}
	};
	struct VisibleStringInfo {
		VisibleStringInfo() {
			Content = "�l�r ����";
			Color = RGB(0, 0, 0);
			Pos = { 0,0,0,1 };
			DeleteTick = 60;
		}
		std::string Content;
		COLORREF Color;
		DirectX::XMVECTOR Pos;
		int DeleteTick;
	};
	inline float Uniform01RandFloat() {
		std::uniform_real_distribution<float> get(0.0f, 1.0f);
		return get(RandEngine);
	}
	inline float UniformRandFloat(float min,float max) {
		std::uniform_real_distribution<float> get(min, max);
		return get(RandEngine);
	}
	inline int UniformRandInt(int min, int max) {
		std::uniform_int_distribution<int> get(min, max);
		return get(RandEngine);
	}
	inline DirectX::XMVECTOR NormalRandHeadingVector(float min,float max,float w = 1) {
		std::uniform_real_distribution<float> get(min, max);
		float azimath = get(RandEngine);
		return { cosf(azimath),sinf(azimath) ,0,w };
	}
	struct WayPoint {
		DirectX::XMVECTOR Pos;
		DirectX::XMVECTOR Heading;
	};
	enum MovementOrder {
		HoldPosition,
		Chase,
		Charge
	};
	enum AttackOrder {
		CeaseFire,
		Nearest,
		Strongest,
		ByDesignation,
	};
	enum Formation {
		Line,// ����
		Circle,// �~�w
	};
};
