struct BlockVSInput
{
    float3 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    float2 World : POSITION1;
    float4 texCoord12 : TEXCOORD1;
    float4 texCoord3M : TEXCOORD2;
    float4 Stain : COLOR;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct BlockVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UV1 : TEXCOORD0; // UV座標
    float3 UV2 : TEXCOORD1; // UV座標
    float3 UV3 : TEXCOORD2; // UV座標
    float3 UVM : TEXCOORD3; // UV座標
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
    output.Stain = input.Stain;
    return output;
}