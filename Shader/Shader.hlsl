//Vertex Shader とは頂点座標の変換に使っている物
//つまりは空間内での座標を画面での座標に変換するものと思えばよいだろう

//頂点シェーダーが返した値をそのままピクセルシェーダーに返す感じだろうか

// 頂点シェーダーから出力するデータ

// 0番のテクスチャスロットを使用する
//これがテクスチャ自体
//Texture2D g_texture : register(t0);
Texture2DArray textures : register(t0);
// 0番のサンプラスロットを使用する
//テクスチャから色を取り出す道具
SamplerState g_sampler : register(s0);
struct LightInfo
{
    float4 pos : POSITION;
    float4 color : COLOR;
};
StructuredBuffer<LightInfo> Lights : register(t0);

struct VSInputForRect
{
    float3 UV : TEXCOORD0;
    float4 Pos : POSITIONT;
    column_major float4x4 World : MWorld;
    float4 texCoord12 : TEXCOORD1;
    float4 texCoord3M : TEXCOORD2;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct VSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UV1 : TEXCOORD0; // UV座標
    float3 UV2 : TEXCOORD1; // UV座標
    float3 UV3 : TEXCOORD2; // UV座標
    float3 UVM : TEXCOORD3; // UV座標
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //ここの並び順はCPU側と同じように書かなくてはならない
};
 
//========================================
// 頂点シェーダー
//========================================
VSOutput VSRect(VSInputForRect input)
{
    VSOutput output;
    output.Pos = mul(input.World, input.Pos);
    if (output.Pos.z < 0.01)
    {
        //output.ColorT = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    }
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
    return output;
}

float4 main(float4 pos : POSITION) : SV_Position
{
    return pos;
}
 
//========================================
// ピクセルシェーダー
//========================================
PSOutput PS(VSOutput In)
{
    PSOutput output;
    float4 mask = textures.Sample(g_sampler, In.UVM.xyz);
    float sum = 0.00001;
    float4 color = textures.Sample(g_sampler, In.UV1.xyz);
    output.Color = color * color.a*mask.r;
    sum = sum + color.a * mask.r;
    
    color = textures.Sample(g_sampler, In.UV2.xyz);
    output.Color = output.Color + color * color.a * mask.g;
    sum = sum + color.a * mask.g;
    
    color = textures.Sample(g_sampler, In.UV3.xyz);
    output.Color = output.Color + color * color.a * mask.b;
    sum = sum + color.a * mask.b;
    
    output.Color = output.Color / sum;
    output.Depth = In.UV1.w;
    return output;
}