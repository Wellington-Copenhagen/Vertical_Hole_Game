
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
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UVM : TEXCOORD0; // UV座標
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    float4 BlackBox[2048] : BLACKBOX;
    //ここの並び順はCPU側と同じように書かなくてはならない
};

BulletVSOutput main(BulletVSInput input)
{
    
    float2 uv = float2(input.UV.x * BlackBox[input.texIndex].x, input.UV.y * BlackBox[input.texIndex].y) + BlackBox[input.texIndex].zw;
    float4 pos = float4(uv.x-0.5, -1 * uv.y+0.5, 0, 1);
    BulletVSOutput output;
    output.Pos = mul(input.World, pos);
    output.Pos = mul(ViewProjection, output.Pos);
    output.UVM.xy = uv;
    output.UVM.z = input.texIndex;
    
    output.UVM.w = output.Pos.z;
    output.Color0 = input.Color0;
    output.Color1 = input.Color1;
    return output;
}