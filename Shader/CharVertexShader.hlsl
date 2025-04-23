//�����𕪂��邽�߂ɂ���𕡐���
struct CharVSInput
{
    float2 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    column_major float4x4 World : MWorld;
    float texIndex : TEXCOORD1;
    float4 Color : COLOR0;
    float2 BlackBox : BlackBox0;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct CharVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float3 UV : TEXCOORD0; // UV���W
    float4 Color : COLOR0;
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    float4 BlackBox[2048] : BLACKBOX;
    //�����̕��я���CPU���Ɠ����悤�ɏ����Ȃ��Ă͂Ȃ�Ȃ�
};
CharVSOutput main(CharVSInput input)
{
    float2 uv = float2(input.UV.x * BlackBox[input.texIndex].x, input.UV.y * BlackBox[input.texIndex].y) + BlackBox[input.texIndex].zw;
    float4 pos = float4(uv.x - 0.5, -1 * uv.y - 0.5, 0, 1);
    CharVSOutput output;
    output.Pos = mul(input.World, input.Pos);
    output.Pos = mul(ViewProjection, output.Pos);
    output.Color = input.Color;
    output.UV = float3(input.UV.x * input.BlackBox.x, input.UV.y * input.BlackBox.y, input.texIndex);
    return output;
}