//Vertex Shader �Ƃ͒��_���W�̕ϊ��Ɏg���Ă��镨
//�܂�͋�ԓ��ł̍��W����ʂł̍��W�ɕϊ�������̂Ǝv���΂悢���낤

//���_�V�F�[�_�[���Ԃ����l�����̂܂܃s�N�Z���V�F�[�_�[�ɕԂ��������낤��

// ���_�V�F�[�_�[����o�͂���f�[�^

// 0�Ԃ̃e�N�X�`���X���b�g���g�p����
//���ꂪ�e�N�X�`������
Texture2D g_texture : register(t0);
// 0�Ԃ̃T���v���X���b�g���g�p����
//�e�N�X�`������F�����o������
SamplerState g_sampler : register(s0);


struct VSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float2 UV : TEXCOORD0; // UV���W
};
 
//========================================
// ���_�V�F�[�_�[
//========================================
VSOutput VS(float4 pos : POSITION,float2 uv : TEXUV)
{
    VSOutput Out;
    // ���_���W���A�������H�������̂܂܏o��
    Out.Pos = pos;
    // ���_��UV���W���A�������H�������̂܂܏o��
    Out.UV = uv;
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
    float4 texColor = g_texture.Sample(g_sampler, In.UV);
 
    // �e�N�X�`���̐F���o��
    return texColor;
}