//Vertex Shader �Ƃ͒��_���W�̕ϊ��Ɏg���Ă��镨
//�܂�͋�ԓ��ł̍��W����ʂł̍��W�ɕϊ�������̂Ǝv���΂悢���낤

//���_�V�F�[�_�[���Ԃ����l�����̂܂܃s�N�Z���V�F�[�_�[�ɕԂ��������낤��

// ���_�V�F�[�_�[����o�͂���f�[�^

// 0�Ԃ̃e�N�X�`���X���b�g���g�p����
//���ꂪ�e�N�X�`������
//Texture2D g_texture : register(t0);
Texture2DArray textures : register(t0);
// 0�Ԃ̃T���v���X���b�g���g�p����
//�e�N�X�`������F�����o������
SamplerState g_sampler : register(s0);
struct LightInfo
{
    float4 pos : POSITION;
    float4 color : COLOR;
};
StructuredBuffer<LightInfo> Lights : register(t0);

struct VSInputForRect
{
    float2 UV : TEXCOORD;
    float4 Pos : POSITIONT;
    column_major float4x4 World : MATRIX;
    uint instanceID : SV_InstanceID;
};
struct VSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UV : TEXCOORD; // UV���W
};
struct PSOutput
{
    float4 Color : SV_Target0;
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //�����̕��я���CPU���Ɠ����悤�ɏ����Ȃ��Ă͂Ȃ�Ȃ�
};
 
//========================================
// ���_�V�F�[�_�[
//========================================
VSOutput VSRect(VSInputForRect input)
{
    VSOutput output;
    output.Pos = mul(input.World, input.Pos);
    output.Pos = mul(ViewProjection, output.Pos);
    output.UV = input.UV;
    return output;
}

    float4 main(float4 pos : POSITION) : SV_Position
{
    return pos;
}
 
//========================================
// �s�N�Z���V�F�[�_�[
//========================================
PSOutput PS(VSOutput In)
{
    PSOutput output;
    output.Color = textures.Sample(g_sampler,In.UV.xyz);
    return output;
}