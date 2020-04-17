
//tutorial4: ����VS_OUTPUT�ṹ, ��VS��� PS����
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

//tutorial7: ������������ṹ
cbuffer cbPerObject
{
	float4x4 WVP;
};

//tutorial4: ������ɫ��Ϣ, ��������Ϣһ������
//tutorial7: ����ͨ��WVP����, ת���õ�����
VS_OUTPUT VS(float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;
	output.Pos = mul(inPos, WVP);//ʹ��mul()�����������λ�����
	output.Color = inColor;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return input.Color;
}
