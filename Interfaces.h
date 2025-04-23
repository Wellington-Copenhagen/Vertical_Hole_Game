#pragma once
#include "Source/DirectX/DirectX.h"
#include "PathBindJsonValue.h"
#include "framework.h"
// Get Set�ŕς���
// �F
// �ʒu
// ���O��MotionInfo
// 
// �����ڂⓖ���蔻���h�������ԂŃf�[�^�����L�ł���I�u�W�F�N�g
#define WindowWidth 1280
#define WindowHeight 720
#define MaxUnitCount 1024
#define MaxBallCount MaxUnitCount * 7
#define MaxBulletCount 65536
#define MaxEffectCount 65536
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
	template<typename ... Args>
	inline void fDebugOutput(Args... args) {
		char* c = new char[256];
		sprintf_s(c,256, args...);
		OutputDebugStringA(c);
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

	template<class DCType>
	void InitDrawCallUV(std::vector<DCType>& drawcallVector,int index) {
		drawcallVector[index * 4 + 0].UV = { 0.0f,1.0f };
		drawcallVector[index * 4 + 1].UV = { 0.0f,0.0f };
		drawcallVector[index * 4 + 2].UV = { 1.0f,1.0f };
		drawcallVector[index * 4 + 3].UV = { 1.0f,0.0f };
	}
	template<class DCType>
	void InitDrawCallPos(std::vector<DCType>& drawcallVector, int index, float bottomXscale = 1, float topXscale = 1, float Yscale = 1, float Yoffset = 0) {
		float width = 0.5f;
		drawcallVector[index * 4 + 0].Pos = { -1 * bottomXscale * width,-1 * Yscale * width + Yoffset,0.0f,1.0f };
		drawcallVector[index * 4 + 1].Pos = { -1 * topXscale * width,Yscale * width + Yoffset,0.0f,1.0f };
		drawcallVector[index * 4 + 2].Pos = { bottomXscale * width,-1 * Yscale * width + Yoffset,0.0f,1.0f };
		drawcallVector[index * 4 + 3].Pos = { topXscale * width,Yscale * width + Yoffset,0.0f,1.0f };
	}
	//�`��o�b�t�@�ɓ����\����
	struct BlockIType {
	public:
		void Set(DirectX::XMMATRIX* world, DirectX::XMVECTOR* texIndex) {
			DirectX::XMStoreFloat2(&World, world->r[3]);
			DirectX::XMStoreFloat4(&TexIndex, *texIndex);
			Stain = DirectX::XMFLOAT4{
				0,0,0,0
			};
		}
		DirectX::XMFLOAT2 World;
		//�摜1,2,3,�}�X�N�̏�
		DirectX::XMFLOAT4 TexIndex;

		DirectX::XMFLOAT4 Stain;
		
	};
	struct BlockDCType {
	public:
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V,������
		DirectX::XMFLOAT4 Pos;
	};
	/*


	struct BallIType {
	public:
		void Set(DirectX::XMMATRIX* world, float texIndex, DirectX::XMVECTOR* color0, DirectX::XMVECTOR* color1, DirectX::XMVECTOR* color2) {
			DirectX::XMStoreFloat4x4(&World, *world);
			TexIndex = texIndex;
			DirectX::XMStoreFloat4(&Color0, *color0);
			DirectX::XMStoreFloat4(&Color1, *color1);
			DirectX::XMStoreFloat4(&Color2, *color2);
		}
		DirectX::XMFLOAT4X4 World;
		//�}�X�N�̃e�N�X�`���ԍ�
		float TexIndex;
		//�}�X�N�ɏ]���ēh��F�̏��
		DirectX::XMFLOAT4 Color0;
		DirectX::XMFLOAT4 Color1;
		DirectX::XMFLOAT4 Color2;

	};
	struct BallDCType {
	public:
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
		DirectX::XMFLOAT4 Pos;
	};


	struct BulletIType {
	public:
		void Set(DirectX::XMMATRIX* world, float texIndex) {
			DirectX::XMStoreFloat4x4(&World, *world);
			TexIndex = texIndex;
		}
		DirectX::XMFLOAT4X4 World;
		float TexIndex;

	};
	struct BulletDCType {
	public:
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
		DirectX::XMFLOAT4 Pos;
	};
	*/
	struct EffectIType {
	public:
		void Set(DirectX::XMMATRIX* world, float texIndex, DirectX::XMVECTOR* color0, DirectX::XMVECTOR* color1) {
			DirectX::XMStoreFloat4x4(&World, *world);
			TexIndex = texIndex;
			DirectX::XMStoreFloat4(&Color0, *color0);
			DirectX::XMStoreFloat4(&Color1, *color1);
		}
		DirectX::XMFLOAT4X4 World;
		//�}�X�N�̏�
		float TexIndex;
		//�}�X�N�ɏ]���ēh��F�̏��
		DirectX::XMFLOAT4 Color0;
		DirectX::XMFLOAT4 Color1;

	};
	struct EffectDCType {
	public:
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
		DirectX::XMFLOAT4 Pos;
	};



	struct CharIType {
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
	struct CharDCType {
	public:
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
		DirectX::XMFLOAT4 Pos;
	};

	struct GeneralIType {
	public:
		void Set(DirectX::XMMATRIX* world, float texIndex) {
			DirectX::XMStoreFloat4x4(&World, *world);
			TexIndex = texIndex;
		}
		DirectX::XMFLOAT4X4 World;
		float TexIndex;

	};
	struct GeneralDCType {
	public:
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
		DirectX::XMFLOAT4 Pos;
	};
	// ���� ����@�E�� �E��
	struct FreeShapeIType {
		void Set(DirectX::XMVECTOR* pos1, DirectX::XMVECTOR* pos2, DirectX::XMVECTOR* pos3, DirectX::XMVECTOR* pos4, float texIndex) {
			DirectX::XMMATRIX pos;
			pos.r[0] = *pos1;
			pos.r[1] = *pos2;
			pos.r[2] = *pos3;
			pos.r[3] = *pos4;
			DirectX::XMStoreFloat4x4(&Pos, pos);
			TexIndex = texIndex;
		}
		DirectX::XMFLOAT4X4 Pos;
		float TexIndex;
	};
	struct FreeShapeDCType {
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
	};

	struct LineIType {
		void Set(DirectX::XMVECTOR left, DirectX::XMVECTOR right, float width, float texIndex) {
			DirectX::XMStoreFloat4(&Left, left);
			DirectX::XMStoreFloat4(&Right, right);
			Width = width;
			TexIndex = texIndex;
		}
		DirectX::XMFLOAT4 Left;
		DirectX::XMFLOAT4 Right;
		float Width;
		float TexIndex;
	};
	struct LineDCType {
		DirectX::XMFLOAT2 UV;//�e���_��U,�e���_��V
		DirectX::XMFLOAT4 Pos;
	};


	// 128kB
	struct ConstantType
	{
	public:
		DirectX::XMFLOAT4X4 ViewProjection;
		DirectX::XMFLOAT4 BlackBox[2048];
	};





#define ComponentCount 32
#define TeamCount 2
	typedef int HostilityTeam;
	typedef int RectAppId;
	typedef int UnitIndex;

	// �ʒu�s��֘A
	inline DirectX::XMMATRIX GetMatrix(DirectX::XMVECTOR* parallel, float rotate, float scale) {
		DirectX::XMMATRIX output{
			scale,0.0f,0.0f,0.0f,
			0.0f,scale,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f,
		};
		output = output * DirectX::XMMatrixRotationZ(rotate);
		output.r[3] = *parallel;
		return output;
	}
	inline DirectX::XMMATRIX GetMatrix(DirectX::XMVECTOR parallel, float rotate, float scale) {
		DirectX::XMMATRIX output{
			scale,0.0f,0.0f,0.0f,
			0.0f,scale,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f,
		};
		output = output * DirectX::XMMatrixRotationZ(rotate);
		output.r[3] = parallel;
		return output;
	}
	inline float ScaleOfMatrix(DirectX::XMMATRIX* input) {
		return sqrtf(input->r[0].m128_f32[0] * input->r[1].m128_f32[1] - input->r[0].m128_f32[1] * input->r[1].m128_f32[0]);
	}
	inline float RotationOfMatrix(DirectX::XMMATRIX* input) {
		return atan2f(input->r[0].m128_f32[1], input->r[1].m128_f32[1]);
	}
	inline DirectX::XMMATRIX SetRotation(DirectX::XMMATRIX* input, float rotate) {
		return GetMatrix(&input->r[3], rotate, ScaleOfMatrix(input));
	}
	inline DirectX::XMMATRIX SetScale(DirectX::XMMATRIX* input, float scale) {
		return GetMatrix(&input->r[3], RotationOfMatrix(input), scale);
	}
	inline DirectX::XMMATRIX Rotate(DirectX::XMMATRIX* matrix, float rotate) {
		DirectX::XMMATRIX output = (*matrix) * DirectX::XMMatrixRotationZ(rotate);
		output.r[3] = matrix->r[3];
		return output;
	}
	inline DirectX::XMMATRIX Zoom(DirectX::XMMATRIX* matrix, float scale) {
		DirectX::XMMATRIX output = (*matrix) * DirectX::XMMatrixScaling(scale,scale,1);
		output.r[3] = matrix->r[3];
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
		Damage(PathBindJsonValue fromLoad) {
			physical = fromLoad.get("physical").asFloat();
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




	static std::mt19937 RandEngine;
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
		DirectX::XMMATRIX Pos;//Core�̏ꍇ��Core�̈ʒu�ACore�ł͂Ȃ��Ȃ�Core�ɑ΂��Ă̈ʒu
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
			HealthMultiply = 1;
			DamageMultiply = 1;
			SpeedMultiply = 1;
			Team = 0;
			IsLeader = false;
			Pos = DirectX::XMMatrixIdentity();
			RadianPerTick = 0.1;
			MoveTilePerTick = 0.1;
		}
		EntityInitData(PathBindJsonValue fromLoad) {
			HealthMultiply = fromLoad.tryGetAsFloat("health", 1);
			DamageMultiply = fromLoad.tryGetAsFloat("attack", 1);
			SpeedMultiply = fromLoad.tryGetAsFloat("speed", 1);
			Team = fromLoad.get("team").asInt();
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
	struct BallInfo {
		std::vector<entt::entity> Prototypes;
		std::vector<DirectX::XMMATRIX> PosFrom;
		std::vector<int> Parent;
		std::vector<int> Connected1;
		std::vector<int> Connected2;
		float Weight;
		float MovePower;
		float RotatePower;
		BallInfo(){}
		BallInfo(PathBindJsonValue jsonOfThisBall, entt::registry* pMainRegistry, std::map<std::string, entt::entity>* pEntNameHash, std::map<std::string, int>* pBallNameHash) {
			for(int i = 0; i < jsonOfThisBall.get("entities").size(); i++) {
				PathBindJsonValue jsonOfThisEntity = jsonOfThisBall.get("entities")[i];
				std::string entityName = jsonOfThisEntity.get("entityName").asString();
				if (pEntNameHash->contains(entityName)) {
					Prototypes.push_back((*pEntNameHash)[entityName]);
				}
				else {
					DebugLogOutput("Json Error:Entity name \"{}\" not found. {}", entityName, jsonOfThisEntity.get("entityName").Path);
				}
				float scale = jsonOfThisEntity.tryGetAsFloat("scale", 1);
				float rotation = jsonOfThisEntity.tryGetAsFloat("rotation", 0);
				DirectX::XMVECTOR parallel = jsonOfThisEntity.tryGetAsVector("parallel", { 0,0,0,1 });
				PosFrom.push_back(GetMatrix(parallel, rotation, scale));
				int parent = jsonOfThisEntity.tryGetAsInt("parent", -1);
				// parent==-1��core��parent�Ƃ���
				// �܂��A���g���w�肵�Ă���ꍇ�͈ړ����R�Ȃ��̂Ƃ���
				int connected1 = jsonOfThisEntity.tryGetAsInt("connected1", 0);
				int connected2 = jsonOfThisEntity.tryGetAsInt("connected2", 0);
				Parent.push_back(parent);
				Connected1.push_back(connected1);
				Connected2.push_back(connected2);
			}
		}
	};
	struct UnitInfo {
		entt::entity CorePrototype;
		DirectX::XMMATRIX World;
		std::vector<int> Ballindices;
		int ThumbnailTextureIndex;
		UnitInfo() {

		}
		UnitInfo(PathBindJsonValue jsonOfThisUnit, entt::registry* pMainRegistry, std::map<std::string, entt::entity>* pEntNameHash, std::map<std::string, int>* pBallNameHash) {
			std::string coreName = jsonOfThisUnit.get("coreName").asString();
			if (pEntNameHash->contains(coreName)) {
				CorePrototype = (*pEntNameHash)[coreName];
			}
			else {
				DebugLogOutput("Json Error:Entity name \"{}\" not found. {}", coreName, jsonOfThisUnit.get("coreName").Path);
			}
			float ratio = jsonOfThisUnit.tryGetAsFloat("ratio", 1);
			World = GetMatrix({ 0,0,0,1 }, 0, ratio);
			Ballindices = std::vector<int>();
			for (int i = 0; i < 7; i++) {
				std::string ballName = jsonOfThisUnit.get("ballNames")[i].asString();
				if (pBallNameHash->contains(ballName)) {
					Ballindices.push_back((*pBallNameHash)[ballName]);
				}
				else {
					DebugLogOutput("Json Error:Ball name \"{}\" not found. {}", ballName, jsonOfThisUnit.get("ballNames")[i].Path);
				}
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
