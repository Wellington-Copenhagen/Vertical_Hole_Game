//高さを分けるためにこれを複数回
struct LineVSInput
{
    float2 UV : TEXCOORD0;
    row_major float4x4 Pos : POSITION0;
    float texIndex : TEXCOORD1;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct GeneralVSOutput
{
    float4 Pos : SV_Position; // 頂点の座標(射影座標系)
    float4 UV : TEXCOORD0; // UV座標
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //ここの並び順はCPU側と同じように書かなくてはならない
};
GeneralVSOutput main(LineVSInput input)
{
    GeneralVSOutput output;
    //output.Pos = input.Pos[input.vertexID];
    switch (input.vertexID)
    {
        case 0:
            output.Pos = input.Pos[0];
            break;
        case 1:
            output.Pos = input.Pos[1];
            break;
        case 2:
            output.Pos = input.Pos[2];
            break;
        case 3:
            output.Pos = input.Pos[3];
            break;   
    }
    output.Pos = mul(ViewProjection, output.Pos);
    
    output.UV.xy = input.UV.xy;
    output.UV.z = input.texIndex;
    
    output.UV.w = output.Pos.z;
    return output;
}