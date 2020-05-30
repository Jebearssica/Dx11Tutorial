//tutorial7: ������������ṹ
//tutorial15: �޸� �����������
cbuffer cbPerObject
{
	float4x4 WVP;
	float4x4 World;
};

//tutorial15: ���� ���߽ṹ
struct Light
{
	float3 direction;
	float4 ambient;
	float4 diffuse;
};

//tutorial10: ����2D�����������״̬
Texture2D ObjTexture;
SamplerState ObjSamplerState;

//tutorial15: ���� ÿ֡����һ�ι�����Ϣ
cbuffer cbPerFrame
{
	Light light;
};

//tutorial4: ����VS_OUTPUT�ṹ, ��VS��� PS����
//tutorial10: ���������ɫ
//tutorial15: ����������Ϣ
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

//tutorial4: ������ɫ��Ϣ, ��������Ϣһ������
//tutorial7: ����ͨ��WVP����, ת���õ�����
//tutorial10: ���������ɫ
//tutorial15: ��������
VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);
	output.normal = mul(normal, World);//���������������˻����ȷ�ķ���
	output.TexCoord = inTexCoord;

	return output;
}

//tutorial14: �Ļ�
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    input.normal = normalize(input.normal);//���߹�һ��

    float4 diffuse = ObjTexture.Sample(ObjSamplerState,input.TexCoord);

    float3 finalColor;
    finalColor = diffuse * light.ambient;//ʹ�û����������ɫ, ʹ�ò�����ֱ�������Ҳ�ɼ�
    //�Դ˽��б���, ��ȷ��û��ֵ����1.0f(��ֹ������)
    finalColor += saturate(dot(light.direction, input.normal)*light.diffuse*diffuse);

    return float4(finalColor,diffuse.a);
}
