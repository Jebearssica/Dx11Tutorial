//Include and link appropriate libraries and headers, ���LNK2019 �޷��������ⲿ����
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "DxErr.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include<DxErr.h>//���ڴ����׳�

/* ȫ�ֱ��� */

LPCTSTR WndClassName = "firstwindow";//��������
HWND hwnd = NULL;//�������
//���ڳ���
const int WIDTH = 300;
const int HEIGHT = 300;

//�������Լ�d3d�豸�������ġ���ȾĿ����ͼ
IDXGISwapChain* SwapChain;
ID3D11Device* D3d11Device;
ID3D11DeviceContext* D3d11DeviceContent;
ID3D11RenderTargetView* RenderTargetView;
//��ɫ,���ڱ�����ɫ����
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
HRESULT hr;	//���ڴ�����

ID3D11Buffer* triangleVertBuffer;//���㻺��
ID3D11VertexShader* VS;//������ɫ��
ID3D11PixelShader* PS;//������ɫ��
ID3D10Blob* VS_Buffer;//������ɫ������
ID3D10Blob* PS_Buffer;//������ɫ������
ID3D11InputLayout* vertexLayout;//�������벼��
struct Vertex//����ṹ
{
	Vertex() {}
	//tutorial4: ����RGBA��ɫԪ��
	Vertex(float x, float y, float z,
		float cr, float cg, float cb, float ca) : position(x, y, z), color(cr, cg, cb, ca) {}

	XMFLOAT3 position;
	XMFLOAT4 color;
};
//���벼��
D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},//ƫ����
};
UINT numElements = ARRAYSIZE(layout);//���沼������Ĵ�С

/* ** ȫ�ֱ��� ** */

/* �������� */

//���ڳ�ʼ��,�漰�ࡢ���ڴ����Լ�������ʾ�����
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int Width, int Height, bool windowed);
//��������
int MessageLoop();
//������Ӧ,�漰esc�ȼ��˳��Լ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//d3d��ʼ��
bool InitializeDirect3dApp(HINSTANCE hInstance);
//�ͷŶ���,��ֹ�ڴ�й©
void RealeaseObjects();
//��ʼ������
bool InitializeScene();
//������ÿ֡����
void UpdateScene();
//����ӳ�䵽��Ļ
void DrawScene();

/* ** �������� ** */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//��ⴰ���Ƿ��ʼ��
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

	//����
	MessageLoop();

	//�ͷ��ڴ�
	RealeaseObjects();

	return 0;
}

/* ����ʵ�� */

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
	DXGI_MODE_DESC bufferDesc;// ���û���
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));//ȷ���������
	bufferDesc.Width = WIDTH;
	bufferDesc.Height = HEIGHT;
	//60Hz: 60/1
	bufferDesc.RefreshRate.Numerator = 60;//����
	bufferDesc.RefreshRate.Denominator = 1;//��ĸ
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//rgba 32bit, Alpha����͸����
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;//������
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));//ȷ���������
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//����d3d�豸�ͽ�����
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &D3d11Device, NULL, &D3d11DeviceContent);
	//���()�����׳�
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("D3D11CreateDeviceAndSwapChain"), MB_OK);
		return 0;
	}

	//�������û���
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	//��Ӵ����׳�
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("SwapChain->GetBuffer()"), MB_OK);
		return 0;
	}

	//������ȾĿ��
	hr = D3d11Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
	BackBuffer->Release();//���û���ʹ�����,ֱ�ӻ���
	//���()�����׳�
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("D3d11Device->CreateRenderTargetView()"), MB_OK);
		return 0;
	}

	//��ȾĿ���
	D3d11DeviceContent->OMSetRenderTargets(1, &RenderTargetView, NULL);
}

void RealeaseObjects()
{
	//����COM������ͷ�
	SwapChain->Release();
	D3d11Device->Release();
	D3d11DeviceContent->Release();

	//��Ⱦ��ͼ�ͷ�
	RenderTargetView->Release();

	//��ɫ���ͷ�
	VS->Release();
	PS->Release();
	//��ɫ�������ͷ�
	VS_Buffer->Release();
	PS_Buffer->Release();
	//���벼���ͷ�
	vertexLayout->Release();

}

//�������������,��ͼ,����ģ��,����
bool InitializeScene()
{
	//������ɫ����������ɫ��ͨ���ļ�����
	hr = D3DX11CompileFromFile("Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile("Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
	//��ɫ������
	hr = D3d11Device->CreateVertexShader(
		VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = D3d11Device->CreatePixelShader(
		PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	//��ɫ������
	D3d11DeviceContent->VSSetShader(VS, 0, 0);
	D3d11DeviceContent->PSSetShader(PS, 0, 0);
	//���㻺�洴��, tutorial4: ����RGBA��ɫԪ��
	Vertex v[] =
	{
		Vertex(0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f),//��
		Vertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),//��
		Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),//�� �������rgb���ǿ��ö�
	};
	//���㻺����������ʼ��
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	//��������Դ��ʼ��
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = v;
	hr = D3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &triangleVertBuffer);
	//���㻺������
	UINT stride = sizeof(Vertex);//�����С
	UINT offset = 0;//ƫ����
	D3d11DeviceContent->IASetVertexBuffers(0, 1,
		&triangleVertBuffer, &stride, &offset);//���㻺����IA��
	//�������벼��
	hr = D3d11Device->CreateInputLayout(layout, numElements,
		VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &vertexLayout);
	//�������벼����IA��
	D3d11DeviceContent->IASetInputLayout(vertexLayout);
	//ͼԪ��������, �����δ�����
	D3d11DeviceContent->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//������ͼ����ʼ��
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	//��ʼλ��Ϊ(0,0),�����ڵ������Ͻ�,��ʹ�ó����봰����ͬ,�Ӷ�ʹ����ͼ������������
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = WIDTH;
	viewport.Height = HEIGHT;
	//��ͼ����, ������դ��ɫ��
	D3d11DeviceContent->RSSetViewports(1, &viewport);
	return true;
}

void UpdateScene()
{
	////�ı䱳��ɫ,�����㲻��rgbɫ�ʵ���
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
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };//������ɫ��ʼ��Ϊ��
	D3d11DeviceContent->ClearRenderTargetView(RenderTargetView, bgColor);//������ɫ���
	D3d11DeviceContent->Draw(3, 0);
	SwapChain->Present(0, 0);//��������ǰ�û���ӳ�䵽��ʾ��
}

/* ** ����ʵ�� ** */
