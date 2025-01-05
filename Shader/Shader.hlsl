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
    float2 UV : TEXCOORD;
    float4 Pos : POSITIONT;
    column_major float4x4 World : MATRIX;
    uint instanceID : SV_InstanceID;
};
struct VSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UV : TEXCOORD; // UV座標
};
struct PSOutput
{
    float4 Color : SV_Target0;
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
    output.Pos = mul(ViewProjection, output.Pos);
    output.UV = input.UV;
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
    output.Color = textures.Sample(g_sampler,In.UV.xyz);
    return output;
}