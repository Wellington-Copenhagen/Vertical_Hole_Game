//�����𕪂��邽�߂ɂ���𕡐���
struct LineVSInput
{
    float2 UV : TEXCOORD0;
    row_major float4x4 Pos : POSITION0;
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
    GeneralVSOutput output;
    float2 uv = float2(input.UV.x * BlackBox[input.texIndex].x, input.UV.y * BlackBox[input.texIndex].y) + BlackBox[input.texIndex].zw;
    float4 pos = (1 - uv.x) * (uv.y * input.Pos[0] + (1 - uv.y) * input.Pos[1]);
    pos = pos + uv.x * (uv.y * input.Pos[2] + (1 - uv.y) * input.Pos[3]);
    output.Pos = mul(ViewProjection, pos);
    
    output.UV.xy = uv;
    output.UV.z = input.texIndex;
    
    output.UV.w = output.Pos.z;
    return output;
}