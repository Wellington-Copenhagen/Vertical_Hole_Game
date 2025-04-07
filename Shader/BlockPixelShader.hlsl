Texture2DArray textures : register(t0);
// 0�Ԃ̃T���v���X���b�g���g�p����
//�e�N�X�`������F�����o������
SamplerState g_sampler : register(s0);

struct BlockVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float3 UV : TEXCOORD0; // UV���W
    float4 texIndex : TEXCOORD1; // �e�N�X�`���C���f�b�N�X���W
    float4 Stain : COLOR;
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
//========================================
// �s�N�Z���V�F�[�_�[
//========================================
PSOutput main(BlockVSOutput input)
{
    PSOutput output;
    float4 mask = textures.Sample(g_sampler, float3(input.UV.xy, input.texIndex.w));
    float sum = 0.00001;
    float sumA = 0.00001;
    float4 color = textures.Sample(g_sampler, float3(input.UV.xy, input.texIndex.x));
    output.Color = color * color.a * mask.r;
    sum = sum + color.a * mask.r;
    sumA = sumA + color.a;
    
    color = textures.Sample(g_sampler, float3(input.UV.xy, input.texIndex.y));
    output.Color = output.Color + color * color.a * mask.g;
    sum = sum + color.a * mask.g;
    sumA = sumA + color.a;
    
    color = textures.Sample(g_sampler, float3(input.UV.xy, input.texIndex.z));
    output.Color = output.Color + color * color.a * mask.b;
    sum = sum + color.a * mask.b;
    sumA = sumA + color.a;
    
    output.Color = output.Color + input.Stain * input.Stain.a;
    sum = sum + input.Stain.a;
    sumA = sumA + input.Stain.a;
    
    output.Color.rgb = output.Color.rgb / sum;
    output.Color.a = output.Color.a / sumA;
    output.Depth = input.UV.z;
    return output;
}