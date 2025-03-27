#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
// Get Setで変える
// 色
// 位置
// 直前のMotionInfo
// 
// 見た目や当たり判定や派生処理間でデータを共有できるオブジェクト
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
	// 文字を表示するやつ

	//プロトタイプにないデータで指定したいものはすべてこれ経由
	/*
	//プロトタイプにないデータで指定したいものはすべてこれ経由
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



	//描画バッファに入れる構造体
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
		//同じテクスチャ画像内での位置のインデックス(左上→右上→左下→右下順),TexArray内でのインデックス
		//画像1,2,3,マスクの順
		DirectX::XMFLOAT4 TexCoord12;
		//同じテクスチャ画像内での位置のインデックス(左上→右上→左下→右下順),TexArray内でのインデックス
		//画像1,2,3,マスクの順
		DirectX::XMFLOAT4 TexCoord3M;

		DirectX::XMFLOAT4 Stain;
		
	};
	struct BlockDrawCallType {
	public:
		DirectX::XMFLOAT3 UV;//各頂点のU,各頂点のV,分割数
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
		//同じテクスチャ画像内での位置のインデックス(左上→右上→左下→右下順),TexArray内でのインデックス
		//マスクの順
		DirectX::XMFLOAT2 TexCoordMask;
		//マスクに従って塗る色の情報
		DirectX::XMFLOAT4 Color0;
		DirectX::XMFLOAT4 Color1;
		DirectX::XMFLOAT4 Color2;

	};
	struct BallDrawCallType {
	public:
		DirectX::XMFLOAT3 UV;//各頂点のU,各頂点のV,分割数
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
		//同じテクスチャ画像内での位置のインデックス(左上→右上→左下→右下順),TexArray内でのインデックス
		//マスクの順
		DirectX::XMFLOAT2 TexCoordMask;
		//マスクに従って塗る色の情報
		DirectX::XMFLOAT4 Color0;
		DirectX::XMFLOAT4 Color1;

	};
	struct BulletDrawCallType {
	public:
		DirectX::XMFLOAT3 UV;//各頂点のU,各頂点のV,分割数
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
		// 文字インデックス
		float TexIndex;
		//文字の色
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT2 BlackBoxShape;

	};
	struct CharDrawCallType {
	public:
		DirectX::XMFLOAT2 UV;//各頂点のU,各頂点のV
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
		DirectX::XMFLOAT2 UV;//各頂点のU,各頂点のV
	};

	struct LineInstanceType {
		DirectX::XMFLOAT4 Vertex1;
		DirectX::XMFLOAT4 Vertex2;
		float width;
		float texIndex;
	};
	struct LineDrawCallType {
		DirectX::XMFLOAT2 UV;//各頂点のU,各頂点のV
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
	static std::bitset<TeamCount*TeamCount> HostilityTable;//8team*8teamのテーブルでtrueの場所は敵対する
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
		// 常に使う
		entt::entity thisEntities;
		entt::entity Prototype;
		bool IsCore;
		// EffectとBallに使う
		float HealthMultiply;
		float DamageMultiply;
		float SpeedMultiply;
		Interface::HostilityTeam Team;
		// Ballに使う
		entt::entity CoreId;
		bool IsLeader;
		entt::entity LeaderId;
		RelationOfCoord Pos;//Coreの場合はCoreの位置、CoreではないならCoreに対しての位置
		DirectX::XMVECTOR BaseColor0;
		DirectX::XMVECTOR BaseColor1;
		float SplintTilePerTick[6];
		float RadianPerTick;
		float MoveTilePerTick;
		float Ratio;
		float Weight;
		// Blockに使う
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
				//コアに対しての初期位置
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
			Content = "ＭＳ 明朝";
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
		Line,// 横隊
		Circle,// 円陣
	};
};
