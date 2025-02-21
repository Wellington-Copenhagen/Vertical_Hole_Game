Texture2DArray textures : register(t0);
// 0番のサンプラスロットを使用する
//テクスチャから色を取り出す道具
SamplerState g_sampler : register(s0);
struct CharacterVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UVM : TEXCOORD0; // UV座標
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
    float4 Color2 : COLOR2;
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
//キャラクターは上書きのような重ね合わせにしたい
//模様を乗せるため
PSOutput main(CharacterVSOutput input)
{
    PSOutput output;
    float4 mask = textures.Sample(g_sampler, input.UVM.xyz);
    output.Color = input.Color0 * mask.r;
    output.Color = output.Color * (1 - mask.g * input.Color1.a) + input.Color1 * mask.g * input.Color1.a;
    output.Color = output.Color * (1 - mask.b * input.Color2.a) + input.Color2 * mask.b * input.Color2.a;
    
    output.Depth = input.UVM.w;
    return output;
}