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
    float3 Normal : NORMAL;
    column_major float4x4 World : MATRIX;
    uint instanceID : SV_InstanceID;
};
struct VSInputForStrip
{
    float3 pos : POSITION; //Z=0.5前後を常に維持すること
    float2 UV : TEXCOORD;
    //uint instanceID : SV_InstanceID;
};
struct VSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UV : TEXCOORD; // UV座標
    float4 RefColor : COLOR; // 反射光
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
cbuffer ConstantBuffer : register(b0)
{
    float2 ViewProjection;
    //ここの並び順はCPU側と同じように書かなくてはならない
};
 
//========================================
// 頂点シェーダー
//========================================
VSOutput VSStrip(VSInputForStrip input)
{
    //シェーダーで分岐とかさせて描画で表示するのが比較的やりやすいデバッグ方な気がする
    VSOutput Out;
    Out.Pos = float4(input.pos.xy,0.5,1);
    //一般的な線形代数での表記とは引数の前後が逆になっていることに極めて注意しなければならない
    Out.Pos = Out.Pos + float4(input.World,0,0);
    float2 WPos = Out.Pos.xy;
    Out.Pos = Out.Pos + float4(ViewProjection,0,0);
    
    Out.RefColor = float4(0, 0, 0, 0);
    for (int i = 0; i < 100; i++)
    {
        if (Lights[i].color.a == 0)
        {
            break;
        }
        float3 normal = mul(input.World, float4(input.Normal, 0));
        float3 Wlight = normalize(Lights[i].pos.xy - WPos);
        float3 Wview = float3(0,0,-1);
        float dist = distance(Lights[i].pos.xy, WPos);
        float3 Ref = normalize(2 * normal * dot(normal, Wlight) - Wlight);
        float RefStr = 0;
        RefStr = RefStr + max(pow(dot(Ref, Wview), 5) * 40, 0);
        RefStr = RefStr + max(pow(dot(Wlight, normal), 1) * 20, 0);
        RefStr = RefStr + 15;
        Out.RefColor = Out.RefColor + RefStr * Lights[i].color / (dist * dist * dist);

    }
    Out.UV = float4(input.UV,input.pos.z/-10.0 + 1.0);
    return Out;
}
VSOutput VSRect(VSInputForRect input)
{
    //シェーダーで分岐とかさせて描画で表示するのが比較的やりやすいデバッグ方な気がする
    VSOutput Out;
    Out.Pos = float4(input.pos.xy, 0.5, 1);
    //一般的な線形代数での表記とは引数の前後が逆になっていることに極めて注意しなければならない
    Out.Pos = Out.Pos + float4(input.World, 0, 0);
    float2 WPos = Out.Pos.xy;
    Out.Pos = Out.Pos + float4(ViewProjection, 0, 0);
    
    Out.RefColor = float4(0, 0, 0, 0);
    for (int i = 0; i < 100; i++)
    {
        if (Lights[i].color.a == 0)
        {
            break;
        }
        float3 normal = mul(input.World, float4(input.Normal, 0));
        float3 Wlight = normalize(Lights[i].pos.xy - WPos);
        float3 Wview = float3(0, 0, -1);
        float dist = distance(Lights[i].pos.xy, WPos);
        float3 Ref = normalize(2 * normal * dot(normal, Wlight) - Wlight);
        float RefStr = 0;
        RefStr = RefStr + max(pow(dot(Ref, Wview), 5) * 40, 0);
        RefStr = RefStr + max(pow(dot(Wlight, normal), 1) * 20, 0);
        RefStr = RefStr + 15;
        Out.RefColor = Out.RefColor + RefStr * Lights[i].color / (dist * dist * dist);

    }
    Out.UV = float4(input.UV, input.pos.z / -10.0 + 1.0);
    return Out;
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
    // テクスチャから色を取得
    //第2引数はUV座標なのでテクスチャ自体の中の座標のことである
    PSOutput output;
    output.Color = textures.Sample(g_sampler, In.UV.xyz) * In.RefColor;
    output.Depth = In.UV.w;
    return output;
}