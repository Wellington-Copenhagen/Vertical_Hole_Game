Texture2DArray textures : register(t0);
// 0�Ԃ̃T���v���X���b�g���g�p����
//�e�N�X�`������F�����o������
SamplerState g_sampler : register(s0);
struct GeneralVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UV : TEXCOORD0; // UV���W
};
struct PSOutput
{
    float4 Color : SV_Target0;
    float Depth : SV_Depth;
};
//�L�����N�^�[�͏㏑���̂悤�ȏd�ˍ��킹�ɂ�����
//�͗l���悹�邽��
PSOutput main(GeneralVSOutput input)
{
    PSOutput output;
    output.Color = textures.Sample(g_sampler, input.UV.xyz);
    output.Depth = input.UV.w;
    return output;
}