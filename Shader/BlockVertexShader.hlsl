struct BlockVSInput
{
    float2 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    float2 World : POSITION1;
    float4 texIndex : TEXCOORD1;
    float4 Stain : COLOR;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct BlockVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float3 UV : TEXCOORD0; // UV座標
    float4 texIndex : TEXCOORD1; // テクスチャインデックス座標
    float4 Stain : COLOR;
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //ここの並び順はCPU側と同じように書かなくてはならない
};
 
//========================================
// 頂点シェーダー
//========================================
BlockVSOutput main(BlockVSInput input)
{
    BlockVSOutput output;
    output.Pos.xy = input.World + input.Pos.xy;
    output.Pos.zw = input.Pos.zw;
    output.Pos = mul(ViewProjection, output.Pos);
    output.UV.xy = input.UV.xy;
    output.texIndex = input.texIndex;
    output.UV.z = output.Pos.z;
    output.Stain = input.Stain;
    return output;
}