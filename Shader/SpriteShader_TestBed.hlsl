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
    float2 UV : TEXCOORD; // UV���W
};
 
//========================================
// ���_�V�F�[�_�[
//========================================
VSOutput VS(float4 pos : POSITION,float2 uv : TEXCOORD)
{
    VSOutput Out;
    // ���_���W���A�������H�������̂܂܏o��
    Out.Pos = pos / float4(pos[2],pos[2],1,1);
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
float3 PS(VSOutput In) : SV_Target0
{
    // �e�N�X�`������F���擾
    //��2������UV���W�Ȃ̂Ńe�N�X�`�����̂̒��̍��W�̂��Ƃł���
    float3 texColor;
    texColor = g_texture.Sample(g_sampler, In.UV);
    
 
    // �e�N�X�`���̐F���o��
    return texColor;
}