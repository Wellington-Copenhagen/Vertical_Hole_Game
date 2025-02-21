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
    float3 UV : TEXCOORD0;
    float4 Pos : POSITIONT;
    column_major float4x4 World : MWorld;
    float4 texCoord12 : TEXCOORD1;
    float4 texCoord3M : TEXCOORD2;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct VSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UV1 : TEXCOORD0; // UV���W
    float3 UV2 : TEXCOORD1; // UV���W
    float3 UV3 : TEXCOORD2; // UV���W
    float3 UVM : TEXCOORD3; // UV���W
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
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
    if (output.Pos.z < 0.01)
    {
        //output.ColorT = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    }
    output.Pos = mul(ViewProjection, output.Pos);
    output.UV1.x = input.UV.x + floor(input.texCoord12.x % input.UV.z) / input.UV.z;
    output.UV1.y = input.UV.y + floor(input.texCoord12.x / input.UV.z) / input.UV.z;
    output.UV1.z = input.texCoord12.y;
    
    output.UV2.x = input.UV.x + floor(input.texCoord12.z % input.UV.z) / input.UV.z;
    output.UV2.y = input.UV.y + floor(input.texCoord12.z / input.UV.z) / input.UV.z;
    output.UV2.z = input.texCoord12.w;
    
    output.UV3.x = input.UV.x + floor(input.texCoord3M.x % input.UV.z) / input.UV.z;
    output.UV3.y = input.UV.y + floor(input.texCoord3M.x / input.UV.z) / input.UV.z;
    output.UV3.z = input.texCoord3M.y;
    
    output.UVM.x = input.UV.x + floor(input.texCoord3M.z % input.UV.z) / input.UV.z;
    output.UVM.y = input.UV.y + floor(input.texCoord3M.z / input.UV.z) / input.UV.z;
    output.UVM.z = input.texCoord3M.w;
    
    output.UV1.w = output.Pos.z;
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
    float4 mask = textures.Sample(g_sampler, In.UVM.xyz);
    float sum = 0.00001;
    float4 color = textures.Sample(g_sampler, In.UV1.xyz);
    output.Color = color * color.a*mask.r;
    sum = sum + color.a * mask.r;
    
    color = textures.Sample(g_sampler, In.UV2.xyz);
    output.Color = output.Color + color * color.a * mask.g;
    sum = sum + color.a * mask.g;
    
    color = textures.Sample(g_sampler, In.UV3.xyz);
    output.Color = output.Color + color * color.a * mask.b;
    sum = sum + color.a * mask.b;
    
    output.Color = output.Color / sum;
    output.Depth = In.UV1.w;
    return output;
}