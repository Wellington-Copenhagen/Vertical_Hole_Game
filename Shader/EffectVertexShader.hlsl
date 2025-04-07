
struct BulletVSInput
{
    float2 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    column_major float4x4 World : MWorld;
    float texIndex : TEXCOORD1;
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct BulletVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UVM : TEXCOORD0; // UV���W
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //�����̕��я���CPU���Ɠ����悤�ɏ����Ȃ��Ă͂Ȃ�Ȃ�
};

BulletVSOutput main(BulletVSInput input)
{
    BulletVSOutput output;
    output.Pos = mul(input.World, input.Pos);
    output.Pos = mul(ViewProjection, output.Pos);
    output.UVM.xy = input.UV.xy;
    output.UVM.z = input.texIndex;
    
    output.UVM.w = output.Pos.z;
    output.Color0 = input.Color0;
    output.Color1 = input.Color1;
    return output;
}