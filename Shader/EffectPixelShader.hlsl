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
struct BulletVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UVM : TEXCOORD0; // UV座標
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
PSOutput main(BulletVSOutput input)
{
    PSOutput output;
    float4 mask = textures.Sample(g_sampler, input.UVM.xyz);
    output.Color = input.Color0 * mask.r;
    output.Color = output.Color * (1 - mask.g) + input.Color1 * mask.g;
    output.Depth = input.UVM.w;
    return output;
}