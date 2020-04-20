//Include and link appropriate libraries and headers, ���LNK2019 �޷��������ⲿ����
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "DxErr.lib")

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

//ID3D11Buffer* triangleVertBuffer;//�����ζ��㻺��, tutorial5: �������ζ��㻺�����
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

//tutorial5: ������������, Ϊ��չʾ�����Ĺ���, ���������ʾ
//���εĶ��㻺������������
ID3D11Buffer* squareIndexBuffer;
ID3D11Buffer* squareVertexBuffer;

//tutorial6: �������/ģ����ͼ�����/ģ�建��
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

//tutorial7: ���볣������, �洢��WVP����, �����ˢ��
ID3D11Buffer* cbPerObjectBuffer;
//tutorial7: ��������ռ�ת������Ķ���
XMMATRIX WVP;
XMMATRIX worldSpace;
XMMATRIX cameraView;
XMMATRIX cameraProjection;
//tutorial7: ������������
XMVECTOR cameraPosition;
XMVECTOR cameraTarget;
XMVECTOR cameraUp;//����������Ϸ���, �ɼ�Note�е� ��ͼ�ռ�(view space)
//tutorial7: ���ӳ�������ṹ
struct cbPerObject
{
	XMMATRIX WVP;
};
cbPerObject cbPerObj;

//tutorial8: �����µ��������, ��������������
XMMATRIX cube1World;
XMMATRIX cube2World;
//tutorial8: ������ת ���� ƽ��
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;//��ת�Ƕ�

//tutorial9: ���ùܵ���RS�׶εĳ���״̬
ID3D11RasterizerState* WireFrame;

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
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));//ȷ���������, tutorial5 :ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
	bufferDesc.Width = WIDTH;
	bufferDesc.Height = HEIGHT;
	//60Hz: 60/1
	bufferDesc.RefreshRate.Numerator = 60;//����
	bufferDesc.RefreshRate.Denominator = 1;//��ĸ
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//rgba 32bit, Alpha����͸����
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;//������,
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));//ȷ���������, tutorial5: ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
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
	//��Ӵ����׳�
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("D3d11Device->CreateRenderTargetView()"), MB_OK);
		return 0;
	}

	//tutorial6: ������ģ�建����������ʼ��
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
	//tutorial6: �������ģ�建�漰��ͼ
	hr = D3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	//tutorial6: �����׳�
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("�������ģ�建��"), MB_OK);
		return 0;
	}
	hr = D3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("�������ģ����ͼ"), MB_OK);
		return 0;
	}

	//��ȾĿ���, tutorial6: �������ģ�建����OM�׶εİ�
	D3d11DeviceContent->OMSetRenderTargets(1, &RenderTargetView, depthStencilView);
}

void RealeaseObjects()
{
	//����COM������ͷ�
	SwapChain->Release();
	D3d11Device->Release();
	D3d11DeviceContent->Release();

	//��Ⱦ��ͼ�ͷ�
	RenderTargetView->Release();

	//tutorial5: �ͷ������������ζ��㻺������������
	squareIndexBuffer->Release();
	squareVertexBuffer->Release();

	//tutorial6: �ͷ����ģ����ͼ�뻺��
	depthStencilView->Release();
	depthStencilBuffer->Release();

	//��ɫ���ͷ�
	VS->Release();
	PS->Release();
	//��ɫ�������ͷ�
	VS_Buffer->Release();
	PS_Buffer->Release();
	//���벼���ͷ�
	vertexLayout->Release();

	//tutorial7: ���������ͷ�
	cbPerObjectBuffer->Release();

	//tutorial9: ��դ״̬�ͷ�
	WireFrame->Release();

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
	//���㼯�ϴ���, tutorial4: ����RGBA��ɫԪ��
	/*
	tutorial5:
		���㼯����Ϊ�ĸ�, ����������
	tutorial8:
		���㼯����Ϊ�˸�, ����������
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
		��������,��012����һ��������,023������һ��, ��Ϊ����ͨ�������ν��д���
	tutorial8:
		��������, �����������������
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

	//tutorial5: ��ʼ����������, ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	/*  
	tutorial5: 
		������������,ÿ������������Ƭ�湹��
		ByteWidth: ÿ��Ƭ�����������㹹��, ÿ������ռ��indices����һ��DWORD����
	tutorial8:
		�����������泤��, ������, 6������, 12��Ƭ��
	*/
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	//tutorial5: �������洴����IA��
	D3D11_SUBRESOURCE_DATA indexInitialData;
	indexInitialData.pSysMem = indices;
	D3d11Device->CreateBuffer(&indexBufferDesc, &indexInitialData, &squareIndexBuffer);
	D3d11DeviceContent->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	/*
		���㻺����������ʼ��
	tutorial5:
		��ʼ�������ζ��㻺��, �������ζ��㻺�滻�������ζ��㻺��
		ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
		�������µĶ��㼯��, ��Ҫ���¶��㻺������
	tutorial8:
		���¶��㻺�泤��, ������, 8������
	*/
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	//��������Դ��ʼ��, tutorial5:ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	hr = D3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertexBuffer);
	//���㻺������
	UINT stride = sizeof(Vertex);//�����С
	UINT offset = 0;//ƫ����
	D3d11DeviceContent->IASetVertexBuffers(0, 1,
		&squareVertexBuffer, &stride, &offset);//���㻺����IA��
	//�������벼��
	hr = D3d11Device->CreateInputLayout(layout, numElements,
		VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &vertexLayout);
	//�������벼����IA��
	D3d11DeviceContent->IASetInputLayout(vertexLayout);
	//ͼԪ��������, �����δ�����
	D3d11DeviceContent->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//������ͼ����ʼ��, tutorial5:ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(viewport));
	//��ʼλ��Ϊ(0,0),�����ڵ������Ͻ�,��ʹ�ó����봰����ͬ,�Ӷ�ʹ����ͼ������������
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = WIDTH;
	viewport.Height = HEIGHT;
	//tutorial6: ������ֵ������(0,1)
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//��ͼ����, ������դ��ɫ��
	D3d11DeviceContent->RSSetViewports(1, &viewport);

	//tutorial7: ������������
	D3D11_BUFFER_DESC cbBufferDesc;
	ZeroMemory(&cbBufferDesc, sizeof(cbBufferDesc));
	cbBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cbBufferDesc.ByteWidth = sizeof(cbPerObject);
	cbBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbBufferDesc.CPUAccessFlags = 0;
	cbBufferDesc.MiscFlags = 0;
	hr = D3d11Device->CreateBuffer(&cbBufferDesc, NULL, &cbPerObjectBuffer);

	/* tutorial7: cbPerObjectBuffer�������Ե�Ԫ��ʼ */
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("������������"), MB_OK);
		return 0;
	}
	/* cbPerObjectBuffer�������Ե�Ԫ���� */

	/*
	tutorial7:
		��������ͷλ��, ���ͶӰ���껹���
	tutorial8:
		��������ͷλ��, �Ի�ø��õ��Ӿ��ǶȺ�Ч��
	*/
	cameraPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
	cameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//tutorial7: ������ͼ�ռ�
	cameraView = XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp);
	//tutorial7: ����ͶӰ�ռ�, 0.4*3.14û�㶮(?)
	cameraProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)WIDTH / HEIGHT, 1.0f, 1000.0f);

	//tutorial9: ������դ״̬�Լ���RS��
	D3D11_RASTERIZER_DESC wireFrameDesc;
	ZeroMemory(&wireFrameDesc, sizeof(wireFrameDesc));
	wireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireFrameDesc.CullMode = D3D11_CULL_NONE;
	hr = D3d11Device->CreateRasterizerState(&wireFrameDesc, &WireFrame);
	/* tutorial9: WireFrame�������Ե�Ԫ��ʼ */
	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr),
			TEXT("������դ״̬"), MB_OK);
		return 0;
	}
	/* WireFrame�������Ե�Ԫ���� */
	D3d11DeviceContent->RSSetState(WireFrame);

	return true;
}

//tutorial8: ����������Χ����һ����������������ת
void UpdateScene()
{
	//��֤��������ת
	rot += .0005f;
	if (rot > 6.28)//����2pi, �Ƕȹ���
		rot = 0.0f;
	//����������1
	cube1World = XMMatrixIdentity();
	//�趨������1������ռ����
	XMVECTOR rotationAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//��ת����(����), y��
	Rotation = XMMatrixRotationAxis(rotationAxis, rot);//��ת����
	Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);//ƽ�ƾ���
	cube1World = Translation * Rotation;
	//����������2
	cube2World = XMMatrixIdentity();
	//�趨������1������ռ����
	Rotation = XMMatrixRotationAxis(rotationAxis, -rot);
	Scale = XMMatrixScaling(1.3f, 1.3f, 1.3f);//����
	cube2World = Rotation * Scale;

	/*
	������
	//�ı䱳��ɫ,�����㲻��rgbɫ�ʵ���
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
	����Ϊ������Χ����һ����������������ת
*/
void DrawScene()
{
	//������ɫ���
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };//������ɫ��ʼ��Ϊ��
	D3d11DeviceContent->ClearRenderTargetView(RenderTargetView, bgColor);//������ɫ���

	//tutorial6: ˢ�����ģ����ͼ
	D3d11DeviceContent->ClearDepthStencilView(depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);


	//tutorial7: ��������ռ�ת��������WVP����
	//tutorial8: ����cube1
	//worldSpace = XMMatrixIdentity();//����һ���վ���, tutorial8: cube1World��cube2World����
	WVP = cube1World * cameraView * cameraProjection;//һ���ռ�ת����ʽ
	//tutorial7: ���³�������
	cbPerObj.WVP = XMMatrixTranspose(WVP);//����ת��
	D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	/*
	tutorial5:
		ʹ��DrawIndexed()����������
	tutorial8:
		������������Ϊ12*3, ����cube1
	*/
	D3d11DeviceContent->DrawIndexed(36, 0, 0);

	//tutorial8: ����cube2
	WVP = cube2World * cameraView * cameraProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);//����ת��
	D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	D3d11DeviceContent->DrawIndexed(36, 0, 0);

	//��������ǰ�û���ӳ�䵽��ʾ��, ��ͼ�����
	SwapChain->Present(0, 0);
}

/* ** ����ʵ�� ** */
