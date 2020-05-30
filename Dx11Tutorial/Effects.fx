//tutorial7: 创建常量缓存结构
//tutorial15: 修改 新增世界矩阵
cbuffer cbPerObject
{
	float4x4 WVP;
	float4x4 World;
};

//tutorial15: 新增 光线结构
struct Light
{
	float3 direction;
	float4 ambient;
	float4 diffuse;
};

//tutorial10: 新增2D纹理与采样器状态
Texture2D ObjTexture;
SamplerState ObjSamplerState;

//tutorial15: 新增 每帧更新一次光线信息
cbuffer cbPerFrame
{
	Light light;
};

//tutorial4: 增加VS_OUTPUT结构, 给VS输出 PS输入
//tutorial10: 纹理替代颜色
//tutorial15: 新增法线信息
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

//tutorial4: 增加颜色信息, 与坐标信息一起输入
//tutorial7: 新增通过WVP矩阵, 转换得到顶点
//tutorial10: 纹理替代颜色
//tutorial15: 新增法线
VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);
	output.normal = mul(normal, World);//法线与世界矩阵相乘获得正确的法线
	output.TexCoord = inTexCoord;

	return output;
}

//tutorial14: 改回
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    input.normal = normalize(input.normal);//法线归一化

    float4 diffuse = ObjTexture.Sample(ObjSamplerState,input.TexCoord);

    float3 finalColor;
    finalColor = diffuse * light.ambient;//使用环境光计算颜色, 使得不被光直射的物体也可见
    //对此进行饱和, 以确保没有值大于1.0f(防止过饱和)
    finalColor += saturate(dot(light.direction, input.normal)*light.diffuse*diffuse);

    return float4(finalColor,diffuse.a);
}
