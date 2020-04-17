
//tutorial4: 增加VS_OUTPUT结构, 给VS输出 PS输入
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

//tutorial7: 创建常量缓存结构
cbuffer cbPerObject
{
	float4x4 WVP;
};

//tutorial4: 增加颜色信息, 与坐标信息一起输入
//tutorial7: 新增通过WVP矩阵, 转换得到顶点
VS_OUTPUT VS(float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;
	output.Pos = mul(inPos, WVP);//使用mul()函数将顶点的位置相乘
	output.Color = inColor;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return input.Color;
}
