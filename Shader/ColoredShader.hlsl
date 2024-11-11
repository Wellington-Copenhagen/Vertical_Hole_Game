//Vertex Shader とは頂点座標の変換に使っている物
//つまりは空間内での座標を画面での座標に変換するものと思えばよいだろう

//頂点シェーダーが返した値をそのままピクセルシェーダーに返す感じだろうか

// 頂点シェーダーから出力するデータ




struct VSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 Color : COLOR; // UV座標
};
cbuffer ConstantBuffer
{
    float4x4 ViewProjection;
};
 
//========================================
// 頂点シェーダー
//========================================
VSOutput VS(float4 pos : POSITION, float4 color : COLOR)
{
    VSOutput Out;
    // 頂点座標を、何も加工せずそのまま出力
    Out.Pos = pos;
    Out.Pos = mul(Out.Pos, ViewProjection);
    // 頂点のUV座標を、何も加工せずそのまま出力
    Out.Color = color;
    return Out;
}
float4 main(float4 pos : POSITION) : SV_Position
{
    return pos;
}
 
//========================================
// ピクセルシェーダー
//========================================
float4 PS(VSOutput In) : SV_Target0
{
    // テクスチャから色を取得
    //第2引数はUV座標なのでテクスチャ自体の中の座標のことである
    float4 texColor = In.Color;
    
 
    // テクスチャの色を出力
    return texColor;
}