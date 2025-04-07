//�����𕪂��邽�߂ɂ���𕡐���


// �ėp�I�ȃV�F�[�_�[��������
// �e�N�X�`���ƈʒu�����w��ł���z
struct GeneralVSInput
{
    float2 UV : TEXCOORD0;
    float4 Pos : POSITION0;
    column_major float4x4 World : MWorld;
    float texIndex : TEXCOORD1;
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};
struct GeneralVSOutput
{
    float4 Pos : SV_Position; // ���_�̍��W(�ˉe���W�n)
    float4 UV : TEXCOORD0; // UV���W
};
cbuffer ConstantBuffer : register(b0)
{
    column_major float4x4 ViewProjection : MATRIX;
    //�����̕��я���CPU���Ɠ����悤�ɏ����Ȃ��Ă͂Ȃ�Ȃ�
};
GeneralVSOutput main(GeneralVSInput input)
{
    GeneralVSOutput output;
    output.Pos = mul(input.World, input.Pos);
    output.Pos = mul(ViewProjection, output.Pos);
    output.UV.xy = input.UV.xy;
    output.UV.z = input.texIndex;
    output.UV.w = output.Pos.z;
    return output;
}