//Include and link appropriate libraries and headers, 解决LNK2019 无法解析的外部符号
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "DxErr.lib")

#include <windows.h>
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

ID3D11Buffer* triangleVertBuffer;//顶点缓存
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
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));//确保对象清空
	bufferDesc.Width = WIDTH;
	bufferDesc.Height = HEIGHT;
	//60Hz: 60/1
	bufferDesc.RefreshRate.Numerator = 60;//分子
	bufferDesc.RefreshRate.Denominator = 1;//分母
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//rgba 32bit, Alpha代表透明度
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;//交换链
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));//确保对象清空
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
	//添加()错误抛出
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("D3d11Device->CreateRenderTargetView()"), MB_OK);
		return 0;
	}

	//渲染目标绑定
	D3d11DeviceContent->OMSetRenderTargets(1, &RenderTargetView, NULL);
}

void RealeaseObjects()
{
	//三个COM组件的释放
	SwapChain->Release();
	D3d11Device->Release();
	D3d11DeviceContent->Release();

	//渲染视图释放
	RenderTargetView->Release();

	//着色器释放
	VS->Release();
	PS->Release();
	//着色器缓存释放
	VS_Buffer->Release();
	PS_Buffer->Release();
	//输入布局释放
	vertexLayout->Release();

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
	//顶点缓存创建, tutorial4: 增加RGBA颜色元素
	Vertex v[] =
	{
		Vertex(0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f),//红
		Vertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),//绿
		Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),//蓝 最基本的rgb还是看得懂
	};
	//顶点缓存描述及初始化
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	//顶点子资源初始化
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = v;
	hr = D3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &triangleVertBuffer);
	//顶点缓存设置
	UINT stride = sizeof(Vertex);//顶点大小
	UINT offset = 0;//偏移量
	D3d11DeviceContent->IASetVertexBuffers(0, 1,
		&triangleVertBuffer, &stride, &offset);//顶点缓存与IA绑定
	//顶点输入布局
	hr = D3d11Device->CreateInputLayout(layout, numElements,
		VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &vertexLayout);
	//顶点输入布局与IA绑定
	D3d11DeviceContent->IASetInputLayout(vertexLayout);
	//图元拓扑设置, 三角形带传输
	D3d11DeviceContent->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//创建视图并初始化
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	//起始位置为(0,0),即窗口的最左上角,并使得长宽与窗口相同,从而使得视图覆盖整个窗口
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = WIDTH;
	viewport.Height = HEIGHT;
	//视图设置, 绑定至光栅着色器
	D3d11DeviceContent->RSSetViewports(1, &viewport);
	return true;
}

void UpdateScene()
{
	////改变背景色,反正搞不懂rgb色彩调整
	//red += colormodr * 0.00005f;
	//green += colormodg * 0.00002f;
	//blue += colormodb * 0.00001f;

	//if (red >= 1.0f || red <= 0.0f)
	//	colormodr *= -1;
	//if (green >= 1.0f || green <= 0.0f)
	//	colormodg *= -1;
	//if (blue >= 1.0f || blue <= 0.0f)
	//	colormodb *= -1;
}

void DrawScene()
{
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };//背景颜色初始化为黑
	D3d11DeviceContent->ClearRenderTargetView(RenderTargetView, bgColor);//背景颜色清空
	D3d11DeviceContent->Draw(3, 0);
	SwapChain->Present(0, 0);//交换链将前置缓存映射到显示器
}

/* ** 函数实现 ** */
