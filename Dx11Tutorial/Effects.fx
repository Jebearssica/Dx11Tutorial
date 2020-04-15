
//tutorial4: 增加VS_OUTPUT结构, 给VS输出 PS输入, 并增加颜色信息, 与坐标信息一起输入
struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT VS(float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;
	output.Pos = inPos;
	output.Color = inColor;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return input.Color;
}
