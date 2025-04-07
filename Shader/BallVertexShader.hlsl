//高さを分けるためにこれを複数回
struct BallVSInput
{
    float2 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    column_major float4x4 World : MWorld;
    float texIndex : TEXCOORD1;
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
    float4 Color2 : COLOR2;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct BallVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UVM : TEXCOORD0; // UV座標
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
    float4 Color2 : COLOR2;
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //ここの並び順はCPU側と同じように書かなくてはならない
};
BallVSOutput main(BallVSInput input)
{
    BallVSOutput output;
    output.Pos = mul(input.World, input.Pos);
    output.Pos = mul(ViewProjection, output.Pos);
    output.UVM.xy = input.UV.xy;
    output.UVM.z = input.texIndex;
    
    output.UVM.w = output.Pos.z;
    output.Color0 = input.Color0;
    output.Color1 = input.Color1;
    output.Color2 = input.Color2;
    return output;
}