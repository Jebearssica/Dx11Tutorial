//tutorial7: ������������ṹ
cbuffer cbPerObject
{
	float4x4 WVP;
};

//tutorial10: ����2D�����������״̬
Texture2D ObjTexture;
SamplerState ObjSamplerState;

//tutorial4: ����VS_OUTPUT�ṹ, ��VS��� PS����
//tutorial10: ���������ɫ
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

//tutorial4: ������ɫ��Ϣ, ��������Ϣһ������
//tutorial7: ����ͨ��WVP����, ת���õ�����
//tutorial10: ���������ɫ
VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);
	output.TexCoord = inTexCoord;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return ObjTexture.Sample(ObjSamplerState, input.TexCoord);
}
