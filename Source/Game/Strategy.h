#pragma once
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"
#define CorpsPerTeam 8
// ������ΏۂƂ�����p�̌���
// �~�b�V�����t�@�C���Ŏw�肳�ꂽ�悤�ɓ���
// �����p���������̑I�����Ƃ��č���Ă����A���������߂Ă���
// ���̂����œ�Փx�ɂ���đI�Ԋm�������߂Ă���������
// ��헪
// �~�b�V�����t�@�C���ŔC�Ӎs���Ǝw�肳��Ă���ꍇ�A���p�Ƃ��Ă�艜�n��
// �K�v�ȏ��Ƃ���
// �G�̎c����
class StrategicDecisionMaker {

};
class Strategy {
	std::vector<entt::entity> AllCorps[TeamCount * CorpsPerTeam];
	std::vector<entt::entity> highDamageUnits;
	// �˒����ɍ��ἨG���e�B�e�B�̃��X�g�ɓ����Ă���Ώۂ��������Ȃ炻����o��
	// �Ȃ��ꍇ��nullptr
	entt::entity* GetNearHighDamageUnit(DirectX::XMVECTOR position, float maximumDistance);
};