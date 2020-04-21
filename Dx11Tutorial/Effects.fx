//tutorial7: 创建常量缓存结构
cbuffer cbPerObject
{
	float4x4 WVP;
};

//tutorial10: 新增2D纹理与采样器状态
Texture2D ObjTexture;
SamplerState ObjSamplerState;

//tutorial4: 增加VS_OUTPUT结构, 给VS输出 PS输入
//tutorial10: 纹理替代颜色
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

//tutorial4: 增加颜色信息, 与坐标信息一起输入
//tutorial7: 新增通过WVP矩阵, 转换得到顶点
//tutorial10: 纹理替代颜色
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
