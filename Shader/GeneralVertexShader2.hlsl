//高さを分けるためにこれを複数回


// 汎用的なシェーダー作ったるわ
// テクスチャと位置だけ指定できる奴
struct GeneralVSInput
{
    float2 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    column_major float4x4 World : MWorld;
    float texIndex : TEXCOORD1;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct GeneralVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UV : TEXCOORD0; // UV座標
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //ここの並び順はCPU側と同じように書かなくてはならない
};
GeneralVSOutput main(GeneralVSInput input)
{
    GeneralVSOutput output;
    output.Pos = mul(input.World, input.Pos);
    output.Pos = mul(ViewProjection, output.Pos);
    output.UV.xy = input.UV.xy;
    output.UV.z = input.texIndex;
    output.UV.w = output.Pos.z;
    return output;
}