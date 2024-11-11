//Vertex Shader とは頂点座標の変換に使っている物
//つまりは空間内での座標を画面での座標に変換するものと思えばよいだろう

//頂点シェーダーが返した値をそのままピクセルシェーダーに返す感じだろうか

// 頂点シェーダーから出力するデータ

// 0番のテクスチャスロットを使用する
//これがテクスチャ自体
Texture2D g_texture : register(t0);
// 0番のサンプラスロットを使用する
//テクスチャから色を取り出す道具
SamplerState g_sampler : register(s0);



struct VSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float2 UV : TEXCOORD; // UV座標
};
 
//========================================
// 頂点シェーダー
//========================================
VSOutput VS(float4 pos : POSITION,float2 uv : TEXCOORD)
{
    VSOutput Out;
    // 頂点座標を、何も加工せずそのまま出力
    Out.Pos = pos / float4(pos[2],pos[2],1,1);
    // 頂点のUV座標を、何も加工せずそのまま出力
    Out.UV = uv;
    return Out;
}
float4 main(float4 pos : POSITION) : SV_Position
{
    return pos;
}
 
//========================================
// ピクセルシェーダー
//========================================
float3 PS(VSOutput In) : SV_Target0
{
    // テクスチャから色を取得
    //第2引数はUV座標なのでテクスチャ自体の中の座標のことである
    float3 texColor;
    texColor = g_texture.Sample(g_sampler, In.UV);
    
 
    // テクスチャの色を出力
    return texColor;
}