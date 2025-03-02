//�����𕪂��邽�߂ɂ���𕡐���
struct BallVSInput
{
    float3 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    column_major float4x4 World : MWorld;
    float2 texCoordMask : TEXCOORD1;
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
    float4 Color2 : COLOR2;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct BallVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UVM : TEXCOORD0; // UV���W
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
    float4 Color2 : COLOR2;
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //�����̕��я���CPU���Ɠ����悤�ɏ����Ȃ��Ă͂Ȃ�Ȃ�
};
BallVSOutput main(BallVSInput input)
{
    BallVSOutput output;
    output.Pos = mul(input.World, input.Pos);
    output.Pos = mul(ViewProjection, output.Pos);
    output.UVM.x = input.UV.x + floor(input.texCoordMask.x % input.UV.z) / input.UV.z;
    output.UVM.y = input.UV.y + floor(input.texCoordMask.x / input.UV.z) / input.UV.z;
    output.UVM.z = input.texCoordMask.y;
    
    output.UVM.w = output.Pos.z;
    output.Color0 = input.Color0;
    output.Color1 = input.Color1;
    output.Color2 = input.Color2;
    return output;
}