Texture2DArray textures : register(t0);
// 0�Ԃ̃T���v���X���b�g���g�p����
//�e�N�X�`������F�����o������
SamplerState g_sampler : register(s0);
struct CharacterVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UVM : TEXCOORD0; // UV���W
    float4 Color0 : COLOR0;
    float4 Color1 : COLOR1;
    float4 Color2 : COLOR2;
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
//�L�����N�^�[�͏㏑���̂悤�ȏd�ˍ��킹�ɂ�����
//�͗l���悹�邽��
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