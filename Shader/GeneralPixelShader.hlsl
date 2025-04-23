Texture2DArray textures : register(t0);
// 0番のサンプラスロットを使用する
//テクスチャから色を取り出す道具
SamplerState g_sampler : register(s0);
struct GeneralVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UV : TEXCOORD0; // UV座標
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
//キャラクターは上書きのような重ね合わせにしたい
//模様を乗せるため
PSOutput main(GeneralVSOutput input)
{
    PSOutput output;
    output.Color = textures.Sample(g_sampler, input.UV.xyz);
    output.Depth = input.UV.w;
    return output;
}