//�����𕪂��邽�߂ɂ���𕡐���
struct LineVSInput
{
    float2 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    float4 Left : POSITION1;
    float4 Right : POSITION2;
    float width : POSITION3;
    float texIndex : TEXCOORD1;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct GeneralVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UV : TEXCOORD0; // UV���W
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    float4 BlackBox[2048] : BLACKBOX;
    //�����̕��я���CPU���Ɠ����悤�ɏ����Ȃ��Ă͂Ȃ�Ȃ�
};
GeneralVSOutput main(LineVSInput input)
{
    float2 uv = float2(input.UV.x * BlackBox[input.texIndex].x, input.UV.y * BlackBox[input.texIndex].y) + BlackBox[input.texIndex].zw;
    float4 pos = float4(uv.x - 0.5, -1 * uv.y + 0.5, 0, 1);
    GeneralVSOutput output;
    float3 xVec = input.Right.xyz - input.Left.xyz;
    float3 center = (input.Right.xyz + input.Left.xyz) * 0.5;
    float3 yVec = float3(xVec.y * -1, xVec.x, 0);
    yVec = normalize(yVec);
    yVec *= input.width;
    output.Pos = float4(pos.x * xVec + pos.y * yVec + center, pos.w);
    output.Pos = mul(ViewProjection, output.Pos);
    output.UV.xy = uv;
    output.UV.z = input.texIndex;
    
    output.UV.w = output.Pos.z;
    return output;
}