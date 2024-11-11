//Vertex Shader �Ƃ͒��_���W�̕ϊ��Ɏg���Ă��镨
//�܂�͋�ԓ��ł̍��W����ʂł̍��W�ɕϊ�������̂Ǝv���΂悢���낤

//���_�V�F�[�_�[���Ԃ����l�����̂܂܃s�N�Z���V�F�[�_�[�ɕԂ��������낤��

// ���_�V�F�[�_�[����o�͂���f�[�^




struct VSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 Color : COLOR; // UV���W
};
cbuffer ConstantBuffer
{
    float4x4 ViewProjection;
};
 
//========================================
// ���_�V�F�[�_�[
//========================================
VSOutput VS(float4 pos : POSITION, float4 color : COLOR)
{
    VSOutput Out;
    // ���_���W���A�������H�������̂܂܏o��
    Out.Pos = pos;
    Out.Pos = mul(Out.Pos, ViewProjection);
    // ���_��UV���W���A�������H�������̂܂܏o��
    Out.Color = color;
    return Out;
}
float4 main(float4 pos : POSITION) : SV_Position
{
    return pos;
}
 
//========================================
// �s�N�Z���V�F�[�_�[
//========================================
float4 PS(VSOutput In) : SV_Target0
{
    // �e�N�X�`������F���擾
    //��2������UV���W�Ȃ̂Ńe�N�X�`�����̂̒��̍��W�̂��Ƃł���
    float4 texColor = In.Color;
    
 
    // �e�N�X�`���̐F���o��
    return texColor;
}