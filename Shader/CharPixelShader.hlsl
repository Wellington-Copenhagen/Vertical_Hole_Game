Texture2DArray textures : register(t0);
// 0�Ԃ̃T���v���X���b�g���g�p����
//�e�N�X�`������F�����o������
SamplerState g_sampler : register(s0);
struct CharVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float3 UV : TEXCOORD0; // UV���W
    float4 Color : COLOR0;
};
struct PSOutput
{
    float4 Color : SV_Target0;
};
//�L�����N�^�[�͏㏑���̂悤�ȏd�ˍ��킹�ɂ�����
//�͗l���悹�邽��
PSOutput main(CharVSOutput input)
{
    PSOutput output;
    float4 mask = textures.Sample(g_sampler, input.UV);
    output.Color = mask.r * 4 * input.Color + (1 - mask.r * 4) * float4(0, 0, 0, 0);
    
    return output;
}