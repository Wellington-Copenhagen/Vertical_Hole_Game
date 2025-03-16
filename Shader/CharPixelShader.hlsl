Texture2DArray textures : register(t0);
// 0番のサンプラスロットを使用する
//テクスチャから色を取り出す道具
SamplerState g_sampler : register(s0);
struct CharVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float3 UV : TEXCOORD0; // UV座標
    float4 Color : COLOR0;
};
struct PSOutput
{
    float4 Color : SV_Target0;
};
//キャラクターは上書きのような重ね合わせにしたい
//模様を乗せるため
PSOutput main(CharVSOutput input)
{
    PSOutput output;
    float4 mask = textures.Sample(g_sampler, input.UV);
    output.Color = mask.r * 4 * input.Color + (1 - mask.r * 4) * float4(0, 0, 0, 0);
    
    return output;
}