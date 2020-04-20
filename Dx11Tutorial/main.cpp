//Include and link appropriate libraries and headers, 解决LNK2019 无法解析的外部符号
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "DxErr.lib")

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include<DxErr.h>//用于错误抛出

/* 全局变量 */

LPCTSTR WndClassName = "firstwindow";//窗体名称
HWND hwnd = NULL;//窗体进程
//窗口长宽
const int WIDTH = 300;
const int HEIGHT = 300;

//交换链以及d3d设备、上下文、渲染目标视图
IDXGISwapChain* SwapChain;
ID3D11Device* D3d11Device;
ID3D11DeviceContext* D3d11DeviceContent;
ID3D11RenderTargetView* RenderTargetView;
//颜色,用于背景颜色更改
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
HRESULT hr;	//用于错误监测

//ID3D11Buffer* triangleVertBuffer;//三角形顶点缓存, tutorial5: 被正方形顶点缓存替代
ID3D11VertexShader* VS;//顶点着色器
ID3D11PixelShader* PS;//像素着色器
ID3D10Blob* VS_Buffer;//顶点着色器缓存
ID3D10Blob* PS_Buffer;//像素着色器缓存
ID3D11InputLayout* vertexLayout;//顶点输入布局
struct Vertex//顶点结构
{
	Vertex() {}
	//tutorial4: 增加RGBA颜色元素
	Vertex(float x, float y, float z,
		float cr, float cg, float cb, float ca) : position(x, y, z), color(cr, cg, cb, ca) {}

	XMFLOAT3 position;
	XMFLOAT4 color;
};
//输入布局
D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},//偏移量
};
UINT numElements = ARRAYSIZE(layout);//保存布局数组的大小

//tutorial5: 加入索引缓存, 为了展示索引的功能, 加入矩形演示
//矩形的顶点缓存与索引缓存
ID3D11Buffer* squareIndexBuffer;
ID3D11Buffer* squareVertexBuffer;

//tutorial6: 加入深度/模板视图与深度/模板缓存
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

//tutorial7: 加入常量缓存, 存储在WVP矩阵, 逐对象刷新
ID3D11Buffer* cbPerObjectBuffer;
//tutorial7: 加入各个空间转换矩阵的定义
XMMATRIX WVP;
XMMATRIX worldSpace;
XMMATRIX cameraView;
XMMATRIX cameraProjection;
//tutorial7: 加入向量定义
XMVECTOR cameraPosition;
XMVECTOR cameraTarget;
XMVECTOR cameraUp;//摄像机的向上方向, 可见Note中的 视图空间(view space)
//tutorial7: 增加常量缓存结构
struct cbPerObject
{
	XMMATRIX WVP;
};
cbPerObject cbPerObj;

//tutorial8: 创建新的世界矩阵, 用于两个正方体
XMMATRIX cube1World;
XMMATRIX cube2World;
//tutorial8: 用于旋转 缩放 平移
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;//旋转角度

//tutorial9: 设置管道的RS阶段的呈现状态
ID3D11RasterizerState* WireFrame;

/* ** 全局变量 ** */

/* 函数声明 */

//窗口初始化,涉及类、窗口创建以及窗口显示与更新
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int Width, int Height, bool windowed);
//运行主体
int MessageLoop();
//按键响应,涉及esc热键退出以及窗口销毁
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//d3d初始化
bool InitializeDirect3dApp(HINSTANCE hInstance);
//释放对象,防止内存泄漏
void RealeaseObjects();
//初始化场景
bool InitializeScene();
//场景的每帧更新
void UpdateScene();
//场景映射到屏幕
void DrawScene();

/* ** 函数声明 ** */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//监测窗体是否初始化
	if (!InitializeWindow(hInstance, nShowCmd, WIDTH, HEIGHT, true))
	{
		MessageBox(0, "Window Initialization Failed!", "ERROR", MB_OK);
		return 0;
	}
	if (!InitializeDirect3dApp(hInstance))
	{
		MessageBox(0, "D3d Initialization Failed!", "ERROR", MB_OK);
		return 0;
	}
	if (!InitializeScene())
	{
		MessageBox(0, "Scene Initialization Failed!", "ERROR", MB_OK);
		return 0;
	}

	//主体
	MessageLoop();

	//释放内存
	RealeaseObjects();

	return 0;
}

/* 函数实现 */

bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int Width, int Height, bool windowed)
{

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 20);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Error registering class", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}
	hwnd = CreateWindowEx(NULL, WndClassName, "Window Title",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		Width, Height, NULL, NULL, hInstance, NULL);
	if (!hwnd)
	{
		MessageBox(NULL, "Error creating window", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);
	return true;
}

int MessageLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// run the game part
			UpdateScene();
			DrawScene();
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(0, "Are you sure you want to exit?",
				"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool InitializeDirect3dApp(HINSTANCE hInstance)
{
	DXGI_MODE_DESC bufferDesc;// 后置缓冲
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));//确保对象清空, tutorial5 :使用变量名做为初始化的参数, 而非类型名
	bufferDesc.Width = WIDTH;
	bufferDesc.Height = HEIGHT;
	//60Hz: 60/1
	bufferDesc.RefreshRate.Numerator = 60;//分子
	bufferDesc.RefreshRate.Denominator = 1;//分母
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//rgba 32bit, Alpha代表透明度
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;//交换链,
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));//确保对象清空, tutorial5: 使用变量名做为初始化的参数, 而非类型名
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//创建d3d设备和交换链
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &D3d11Device, NULL, &D3d11DeviceContent);
	//添加()错误抛出
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("D3D11CreateDeviceAndSwapChain"), MB_OK);
		return 0;
	}

	//创建后置缓存
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	//添加错误抛出
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("SwapChain->GetBuffer()"), MB_OK);
		return 0;
	}

	//创建渲染目标
	hr = D3d11Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
	BackBuffer->Release();//后置缓存使用完毕,直接回收
	//添加错误抛出
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("D3d11Device->CreateRenderTargetView()"), MB_OK);
		return 0;
	}

	//tutorial6: 添加深度模板缓存描述及初始化
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = WIDTH;
	depthStencilDesc.Height = HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	//tutorial6: 创建深度模板缓存及视图
	hr = D3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	//tutorial6: 错误抛出
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("创建深度模板缓存"), MB_OK);
		return 0;
	}
	hr = D3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("创建深度模板视图"), MB_OK);
		return 0;
	}

	//渲染目标绑定, tutorial6: 增加深度模板缓存与OM阶段的绑定
	D3d11DeviceContent->OMSetRenderTargets(1, &RenderTargetView, depthStencilView);
}

void RealeaseObjects()
{
	//三个COM组件的释放
	SwapChain->Release();
	D3d11Device->Release();
	D3d11DeviceContent->Release();

	//渲染视图释放
	RenderTargetView->Release();

	//tutorial5: 释放新增的正方形顶点缓存与索引缓存
	squareIndexBuffer->Release();
	squareVertexBuffer->Release();

	//tutorial6: 释放深度模板视图与缓存
	depthStencilView->Release();
	depthStencilBuffer->Release();

	//着色器释放
	VS->Release();
	PS->Release();
	//着色器缓存释放
	VS_Buffer->Release();
	PS_Buffer->Release();
	//输入布局释放
	vertexLayout->Release();

	//tutorial7: 常量缓存释放
	cbPerObjectBuffer->Release();

	//tutorial9: 光栅状态释放
	WireFrame->Release();

}

//在这里放置物体,贴图,加载模型,音乐
bool InitializeScene()
{
	//顶点着色器与像素着色器通过文件编译
	hr = D3DX11CompileFromFile("Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile("Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
	//着色器创建
	hr = D3d11Device->CreateVertexShader(
		VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = D3d11Device->CreatePixelShader(
		PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	//着色器设置
	D3d11DeviceContent->VSSetShader(VS, 0, 0);
	D3d11DeviceContent->PSSetShader(PS, 0, 0);
	//顶点集合创建, tutorial4: 增加RGBA颜色元素
	/*
	tutorial5:
		顶点集合设为四个, 绘制正方形
	tutorial8:
		顶点集合设为八个, 绘制正方体
	*/
	Vertex v[] =
	{
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
		Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, -1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(-1.0f, +1.0f, +1.0f, 1.0f, 1.0f, 1.0f, 1.0f),
		Vertex(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
		Vertex(+1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	};
	/*
	tutorial5:
		增加索引,点012构成一个三角形,023构成另一个, 因为布局通过三角形进行传输
	tutorial8:
		增加索引, 构成正方体的六个面
	*/
	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	//tutorial5: 初始化索引缓存, 使用变量名做为初始化的参数, 而非类型名
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	/*  
	tutorial5: 
		索引缓存设置,每个矩形由两个片面构成
		ByteWidth: 每个片面由三个顶点构成, 每个顶点占用indices数组一个DWORD长度
	tutorial8:
		更新索引缓存长度, 正方体, 6个矩形, 12个片面
	*/
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	//tutorial5: 索引缓存创建与IA绑定
	D3D11_SUBRESOURCE_DATA indexInitialData;
	indexInitialData.pSysMem = indices;
	D3d11Device->CreateBuffer(&indexBufferDesc, &indexInitialData, &squareIndexBuffer);
	D3d11DeviceContent->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	/*
		顶点缓存描述及初始化
	tutorial5:
		初始化正方形顶点缓存, 将三角形顶点缓存换成正方形顶点缓存
		使用变量名做为初始化的参数, 而非类型名
		创建了新的顶点集后, 需要更新顶点缓存设置
	tutorial8:
		更新顶点缓存长度, 正方体, 8个顶点
	*/
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	//顶点子资源初始化, tutorial5:使用变量名做为初始化的参数, 而非类型名
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	hr = D3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertexBuffer);
	//顶点缓存设置
	UINT stride = sizeof(Vertex);//顶点大小
	UINT offset = 0;//偏移量
	D3d11DeviceContent->IASetVertexBuffers(0, 1,
		&squareVertexBuffer, &stride, &offset);//顶点缓存与IA绑定
	//顶点输入布局
	hr = D3d11Device->CreateInputLayout(layout, numElements,
		VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &vertexLayout);
	//顶点输入布局与IA绑定
	D3d11DeviceContent->IASetInputLayout(vertexLayout);
	//图元拓扑设置, 三角形带传输
	D3d11DeviceContent->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//创建视图并初始化, tutorial5:使用变量名做为初始化的参数, 而非类型名
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(viewport));
	//起始位置为(0,0),即窗口的最左上角,并使得长宽与窗口相同,从而使得视图覆盖整个窗口
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = WIDTH;
	viewport.Height = HEIGHT;
	//tutorial6: 添加深度值的区间(0,1)
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//视图设置, 绑定至光栅着色器
	D3d11DeviceContent->RSSetViewports(1, &viewport);

	//tutorial7: 创建常量缓存
	D3D11_BUFFER_DESC cbBufferDesc;
	ZeroMemory(&cbBufferDesc, sizeof(cbBufferDesc));
	cbBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cbBufferDesc.ByteWidth = sizeof(cbPerObject);
	cbBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbBufferDesc.CPUAccessFlags = 0;
	cbBufferDesc.MiscFlags = 0;
	hr = D3d11Device->CreateBuffer(&cbBufferDesc, NULL, &cbPerObjectBuffer);

	/* tutorial7: cbPerObjectBuffer创建测试单元开始 */
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("创建常量缓存"), MB_OK);
		return 0;
	}
	/* cbPerObjectBuffer创建测试单元结束 */

	/*
	tutorial7:
		定义摄像头位置, 这个投影坐标还清楚
	tutorial8:
		更新摄像头位置, 以获得更好的视觉角度和效果
	*/
	cameraPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
	cameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//tutorial7: 创建视图空间
	cameraView = XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp);
	//tutorial7: 创建投影空间, 0.4*3.14没搞懂(?)
	cameraProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)WIDTH / HEIGHT, 1.0f, 1000.0f);

	//tutorial9: 创建光栅状态以及与RS绑定
	D3D11_RASTERIZER_DESC wireFrameDesc;
	ZeroMemory(&wireFrameDesc, sizeof(wireFrameDesc));
	wireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireFrameDesc.CullMode = D3D11_CULL_NONE;
	hr = D3d11Device->CreateRasterizerState(&wireFrameDesc, &WireFrame);
	/* tutorial9: WireFrame创建测试单元开始 */
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("创建光栅状态"), MB_OK);
		return 0;
	}
	/* WireFrame创建测试单元结束 */
	D3d11DeviceContent->RSSetState(WireFrame);

	return true;
}

//tutorial8: 更新正方体围绕另一个自旋的正方体旋转
void UpdateScene()
{
	//保证正方体旋转
	rot += .0005f;
	if (rot > 6.28)//大于2pi, 角度归零
		rot = 0.0f;
	//重置正方体1
	cube1World = XMMatrixIdentity();
	//设定正方体1的世界空间矩阵
	XMVECTOR rotationAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//旋转方向(向量), y轴
	Rotation = XMMatrixRotationAxis(rotationAxis, rot);//旋转矩阵
	Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);//平移矩阵
	cube1World = Translation * Rotation;
	//重置正方体2
	cube2World = XMMatrixIdentity();
	//设定正方体1的世界空间矩阵
	Rotation = XMMatrixRotationAxis(rotationAxis, -rot);
	Scale = XMMatrixScaling(1.3f, 1.3f, 1.3f);//缩放
	cube2World = Rotation * Scale;

	/*
	旧内容
	//改变背景色,反正搞不懂rgb色彩调整
	red += colormodr * 0.00005f;
	green += colormodg * 0.00002f;
	blue += colormodb * 0.00001f;

	if (red >= 1.0f || red <= 0.0f)
		colormodr *= -1;
	if (green >= 1.0f || green <= 0.0f)
		colormodg *= -1;
	if (blue >= 1.0f || blue <= 0.0f)
		colormodb *= -1;
	*/

}

/*
tutorial8:
	更新为正方体围绕另一个自旋的正方体旋转
*/
void DrawScene()
{
	//背景颜色清空
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };//背景颜色初始化为黑
	D3d11DeviceContent->ClearRenderTargetView(RenderTargetView, bgColor);//背景颜色清空

	//tutorial6: 刷新深度模板视图
	D3d11DeviceContent->ClearDepthStencilView(depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);


	//tutorial7: 定义世界空间转换矩阵与WVP矩阵
	//tutorial8: 绘制cube1
	//worldSpace = XMMatrixIdentity();//返回一个空矩阵, tutorial8: cube1World与cube2World代替
	WVP = cube1World * cameraView * cameraProjection;//一个空间转换公式
	//tutorial7: 更新常量缓存
	cbPerObj.WVP = XMMatrixTranspose(WVP);//矩阵转置
	D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	/*
	tutorial5:
		使用DrawIndexed()绘制正方形
	tutorial8:
		更新索引数量为12*3, 绘制cube1
	*/
	D3d11DeviceContent->DrawIndexed(36, 0, 0);

	//tutorial8: 绘制cube2
	WVP = cube2World * cameraView * cameraProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);//矩阵转置
	D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	D3d11DeviceContent->DrawIndexed(36, 0, 0);

	//交换链将前置缓存映射到显示器, 即图像呈现
	SwapChain->Present(0, 0);
}

/* ** 函数实现 ** */
