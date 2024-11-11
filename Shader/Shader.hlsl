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
    float3 Normal : NORMAL;
    column_major float4x4 World : MATRIX;
    uint instanceID : SV_InstanceID;
};
struct VSInputForStrip
{
    float3 pos : POSITION; //Z=0.5�O�����Ɉێ����邱��
    float2 UV : TEXCOORD;
    //uint instanceID : SV_InstanceID;
};
struct VSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UV : TEXCOORD; // UV���W
    float4 RefColor : COLOR; // ���ˌ�
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
cbuffer ConstantBuffer : register(b0)
{
    float2 ViewProjection;
    //�����̕��я���CPU���Ɠ����悤�ɏ����Ȃ��Ă͂Ȃ�Ȃ�
};
 
//========================================
// ���_�V�F�[�_�[
//========================================
VSOutput VSStrip(VSInputForStrip input)
{
    //�V�F�[�_�[�ŕ���Ƃ������ĕ`��ŕ\������̂���r�I���₷���f�o�b�O���ȋC������
    VSOutput Out;
    Out.Pos = float4(input.pos.xy,0.5,1);
    //��ʓI�Ȑ��`�㐔�ł̕\�L�Ƃ͈����̑O�オ�t�ɂȂ��Ă��邱�Ƃɋɂ߂Ē��ӂ��Ȃ���΂Ȃ�Ȃ�
    Out.Pos = Out.Pos + float4(input.World,0,0);
    float2 WPos = Out.Pos.xy;
    Out.Pos = Out.Pos + float4(ViewProjection,0,0);
    
    Out.RefColor = float4(0, 0, 0, 0);
    for (int i = 0; i < 100; i++)
    {
        if (Lights[i].color.a == 0)
        {
            break;
        }
        float3 normal = mul(input.World, float4(input.Normal, 0));
        float3 Wlight = normalize(Lights[i].pos.xy - WPos);
        float3 Wview = float3(0,0,-1);
        float dist = distance(Lights[i].pos.xy, WPos);
        float3 Ref = normalize(2 * normal * dot(normal, Wlight) - Wlight);
        float RefStr = 0;
        RefStr = RefStr + max(pow(dot(Ref, Wview), 5) * 40, 0);
        RefStr = RefStr + max(pow(dot(Wlight, normal), 1) * 20, 0);
        RefStr = RefStr + 15;
        Out.RefColor = Out.RefColor + RefStr * Lights[i].color / (dist * dist * dist);

    }
    Out.UV = float4(input.UV,input.pos.z/-10.0 + 1.0);
    return Out;
}
VSOutput VSRect(VSInputForRect input)
{
    //�V�F�[�_�[�ŕ���Ƃ������ĕ`��ŕ\������̂���r�I���₷���f�o�b�O���ȋC������
    VSOutput Out;
    Out.Pos = float4(input.pos.xy, 0.5, 1);
    //��ʓI�Ȑ��`�㐔�ł̕\�L�Ƃ͈����̑O�オ�t�ɂȂ��Ă��邱�Ƃɋɂ߂Ē��ӂ��Ȃ���΂Ȃ�Ȃ�
    Out.Pos = Out.Pos + float4(input.World, 0, 0);
    float2 WPos = Out.Pos.xy;
    Out.Pos = Out.Pos + float4(ViewProjection, 0, 0);
    
    Out.RefColor = float4(0, 0, 0, 0);
    for (int i = 0; i < 100; i++)
    {
        if (Lights[i].color.a == 0)
        {
            break;
        }
        float3 normal = mul(input.World, float4(input.Normal, 0));
        float3 Wlight = normalize(Lights[i].pos.xy - WPos);
        float3 Wview = float3(0, 0, -1);
        float dist = distance(Lights[i].pos.xy, WPos);
        float3 Ref = normalize(2 * normal * dot(normal, Wlight) - Wlight);
        float RefStr = 0;
        RefStr = RefStr + max(pow(dot(Ref, Wview), 5) * 40, 0);
        RefStr = RefStr + max(pow(dot(Wlight, normal), 1) * 20, 0);
        RefStr = RefStr + 15;
        Out.RefColor = Out.RefColor + RefStr * Lights[i].color / (dist * dist * dist);

    }
    Out.UV = float4(input.UV, input.pos.z / -10.0 + 1.0);
    return Out;
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
    // �e�N�X�`������F���擾
    //��2������UV���W�Ȃ̂Ńe�N�X�`�����̂̒��̍��W�̂��Ƃł���
    PSOutput output;
    output.Color = textures.Sample(g_sampler, In.UV.xyz) * In.RefColor;
    output.Depth = In.UV.w;
    return output;
}