Texture2DArray textures : register(t0);
// 0�Ԃ̃T���v���X���b�g���g�p����
//�e�N�X�`������F�����o������
SamplerState g_sampler : register(s0);

struct BlockVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UV1 : TEXCOORD0; // UV���W
    float3 UV2 : TEXCOORD1; // UV���W
    float3 UV3 : TEXCOORD2; // UV���W
    float3 UVM : TEXCOORD3; // UV���W
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
    float4 mask = textures.Sample(g_sampler, input.UVM.xyz);
    float sum = 0.00001;
    float4 color = textures.Sample(g_sampler, input.UV1.xyz);
    output.Color = color * color.a * mask.r;
    sum = sum + color.a * mask.r;
    
    color = textures.Sample(g_sampler, input.UV2.xyz);
    output.Color = output.Color + color * color.a * mask.g;
    sum = sum + color.a * mask.g;
    
    color = textures.Sample(g_sampler, input.UV3.xyz);
    output.Color = output.Color + color * color.a * mask.b;
    sum = sum + color.a * mask.b;
    
    output.Color = output.Color + input.Stain * input.Stain.a;
    sum = sum + input.Stain.a;
    
    output.Color = output.Color / sum;
    output.Depth = input.UV1.w;
    return output;
}