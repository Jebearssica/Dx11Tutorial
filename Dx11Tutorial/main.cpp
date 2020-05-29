//Include and link appropriate libraries and headers, ���LNK2019 �޷��������ⲿ����
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "DxErr.lib")
//tutorial13: ������������DX10�н���D2D������Ⱦ
#pragma comment(lib, "D3D10_1.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <DxErr.h> //���ڴ����׳�
//tutorial13: ������������DX10�н���D2D������Ⱦ
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>

/* ȫ�ֱ��� */

LPCTSTR WndClassName = L"firstwindow"; //��������
HWND hwnd = NULL;                      //�������
//���ڳ���
const int WIDTH = 300;
const int HEIGHT = 300;

//�������Լ�d3d�豸�������ġ���ȾĿ����ͼ
IDXGISwapChain *SwapChain;
ID3D11Device *D3d11Device;
ID3D11DeviceContext *D3d11DeviceContent;
ID3D11RenderTargetView *RenderTargetView;
//��ɫ,���ڱ�����ɫ����
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
HRESULT hr; //���ڴ�����

//ID3D11Buffer* triangleVertBuffer;//�����ζ��㻺��, tutorial5: �������ζ��㻺�����
ID3D11VertexShader *VS;          //������ɫ��
ID3D11PixelShader *PS;           //������ɫ��
ID3D10Blob *VS_Buffer;           //������ɫ������
ID3D10Blob *PS_Buffer;           //������ɫ������
ID3D11InputLayout *vertexLayout; //�������벼��

/*
tutorial10:
    ɾ����ɫ����, �滻Ϊ��������
*/
struct Vertex //����ṹ
{
    Vertex() {}
    //tutorial4: ����RGBA��ɫԪ��, tutorial10: �滻Ϊ��������
    Vertex(float x, float y, float z,
        float u, float v) : position(x, y, z), textureCoord(u, v) {}

    XMFLOAT3 position;
    XMFLOAT2 textureCoord;
};
//���벼��, tutorial10: ���ݶ���ṹ�޸�
D3D11_INPUT_ELEMENT_DESC layout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, //ƫ����
};
UINT numElements = ARRAYSIZE(layout); //���沼������Ĵ�С

//tutorial5: ������������, Ϊ��չʾ�����Ĺ���, ���������ʾ
//���εĶ��㻺������������
ID3D11Buffer *squareIndexBuffer;
ID3D11Buffer *squareVertexBuffer;

//tutorial6: �������/ģ����ͼ�����/ģ�建��
ID3D11DepthStencilView *depthStencilView;
ID3D11Texture2D *depthStencilBuffer;

//tutorial7: ���볣������, �洢��WVP����, �����ˢ��
ID3D11Buffer *cbPerObjectBuffer;
//tutorial7: ��������ռ�ת������Ķ���
XMMATRIX WVP;
XMMATRIX worldSpace;
XMMATRIX cameraView;
XMMATRIX cameraProjection;
//tutorial7: ������������
XMVECTOR cameraPosition;
XMVECTOR cameraTarget;
XMVECTOR cameraUp; //����������Ϸ���, �ɼ�Note�е� ��ͼ�ռ�(view space)
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
float rot = 0.01f; //��ת�Ƕ�

//tutorial9: ���ùܵ���RS�׶εĳ���״̬
ID3D11RasterizerState *WireFrame;

//tutorial10: ����������ļ��м��ص������Լ����������״̬��Ϣ�Ľӿ�
ID3D11ShaderResourceView *CubeTexture;
ID3D11SamplerState *CubeTextureSampleState;

//tutorial11: ����һ�����״̬, ������դ״̬, ����һ��˳ʱ����һ����ʱ��
ID3D11BlendState *Transparency;
ID3D11RasterizerState *CCWcullMode; //counter clockwise ��ʱ��
ID3D11RasterizerState *CWcullMode;  //clockwise ˳ʱ��

//turorial12: ����һ���µ���Ⱦ״̬, ���ڿ���������ĺ��沿��
ID3D11RasterizerState *noCull;

/*
tutorial13: ����
    D3D10�豸 D3d10Device
    ����D3D10��D3D11�豸���ź��� KeyedMutexD3d10 KeyedMutexD3d11
    D2D����ȾĿ�� D2dRenderTarget
    D2D�Ĺ�����ɫ��ˢ ColorBrush: ����д��
    �洢2D��������ĺ��û��� BackBuffer11
    ������������ SharedTexture11
    D2D���㻺�� D2dVertexBuffer
    D2D�������� D2dIndexBuffer
    D2D��ɫ����Դ��ͼ D2dTexture
    DWriteFactory: ��Ⱦ���ֵĽӿ�, ֪ͨD2D��λ���
    TextFormat: ���ָ�ʽ
    �������ִ���Ŀ��ַ��� printText
*/
ID3D10Device1 *D3d10Device;
IDXGIKeyedMutex *KeyedMutexD3d10;
IDXGIKeyedMutex *KeyedMutexD3d11;
ID2D1RenderTarget *D2dRenderTarget;
ID2D1SolidColorBrush *ColorBrush;
ID3D11Texture2D *BackBuffer11;
ID3D11Texture2D *SharedTexture11;
ID3D11Buffer *D2dVertexBuffer;
ID3D11Buffer *D2dIndexBuffer;
ID3D11ShaderResourceView *D2dTexture;
IDWriteFactory *DWriteFactory;
IDWriteTextFormat *TextFormat;
std::wstring printText;

//tutorial14: ���� ���ڴ���FPSֵ
double CountsPerSecond = 0.0;
__int64 CounterStart = 0;

int FrameCount = 0;
int fps = 0;

__int64 FrameTimeOld = 0;
double FrameTime;

/* ** ȫ�ֱ��� ** */

/* �������� */

//���ڳ�ʼ��,�漰�ࡢ���ڴ����Լ�������ʾ�����
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int Width, int Height, bool windowed);
//��������
int MessageLoop();
//������Ӧ,�漰esc�ȼ��˳��Լ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//d3d11��ʼ��
bool InitializeDirect3dApp(HINSTANCE hInstance);
//�ͷŶ���,��ֹ�ڴ�й©
void ReleaseObjects();
//��ʼ������
bool InitializeScene();
//������ÿ֡����
//tutorial14: �޸� ����time�β�, ����ʱ����и���, ��ֹ��ͬFPSֵ�������˶��ٶȲ�ͬ
void UpdateScene(double time);
//����ӳ�䵽��Ļ
void DrawScene();

/*
tutorrial13: ����
    InitialD2D_D3D10_DWrite: ��ʼ��D2D D3D10 DWrite
    InitialD2DScreenTexture: ��ʼ��D2D����
    RenderText: ��Ⱦ����
*/
bool InitialD2D_D3D10_DWrite(IDXGIAdapter1 *Adapter);
void InitialD2DScreenTexture();
//tutorial14: �޸� ����inInt�β�, ����FPSֵ, ��ȻFPS��ȫ�ֱ���, ��Ϊ�˽ṹ��������Ȼʹ�ò�������
void RenderText(std::wstring text, int inInt);

/*
tutorial14: ����
    StartTimer: ������ʱ��
    GetTime: ��ȡʱ��
    GetFrameTime: ��ȡÿ֡ʱ��
*/
void StartTimer();
double GetTime();
double GetFrameTime();

/* ** �������� ** */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    //��ⴰ���Ƿ��ʼ��
    if (!InitializeWindow(hInstance, nShowCmd, WIDTH, HEIGHT, true))
    {
        MessageBox(0, L"Window Initialization Failed!", L"ERROR", MB_OK);
        return 0;
    }
    if (!InitializeDirect3dApp(hInstance))
    {
        MessageBox(0, L"D3d Initialization Failed!", L"ERROR", MB_OK);
        return 0;
    }
    if (!InitializeScene())
    {
        MessageBox(0, L"Scene Initialization Failed!", L"ERROR", MB_OK);
        return 0;
    }

    //����
    MessageLoop();

    //�ͷ��ڴ�
    ReleaseObjects();

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
        MessageBox(NULL, L"Error registering class", L"ERROR", MB_OK | MB_ICONERROR);
        return false;
    }
    hwnd = CreateWindowEx(NULL, WndClassName, L"Tutorial10-Texture",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        Width, Height, NULL, NULL, hInstance, NULL);
    if (!hwnd)
    {
        MessageBox(NULL, L"Error creating window", L"ERROR", MB_OK | MB_ICONERROR);
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
            //tutorial14: ���� ����ÿ��֡��
            FrameCount++;         //ÿ��ѭ������һ֡
            if (GetTime() > 1.0f) //һ���, ͳ��һ���ڵ�֡��
            {
                fps = FrameCount;
                FrameCount = 0;
                StartTimer(); //���¼�ʱ
            }
            FrameTime = GetFrameTime(); //ÿһ֡��ʱ��

            UpdateScene(FrameTime); //�������ʱ����г�������
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
            if (MessageBox(0, L"Are you sure you want to exit?",
                L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
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
    /*
    tutorial13: ����
        DXGIFactory, �Ӷ���ö�ٳ�
        Adapter, ͨ��������ʹ��D3D11�豸����D3D10ͬ��
    */
    IDXGIFactory1 *DXGIFactory;
    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&DXGIFactory);
    IDXGIAdapter1 *Adapter;
    hr = DXGIFactory->EnumAdapters1(0, &Adapter);
    DXGIFactory->Release(); //�ú�����

    /*
    tutorial13: �޸�
        BGRA��ʽ
    */
    DXGI_MODE_DESC bufferDesc;                   // ���û���
    ZeroMemory(&bufferDesc, sizeof(bufferDesc)); //ȷ���������, tutorial5 :ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
    bufferDesc.Width = WIDTH;
    bufferDesc.Height = HEIGHT;
    //144Hz: 144/1
    bufferDesc.RefreshRate.Numerator = 144;         //����
    bufferDesc.RefreshRate.Denominator = 1;         //��ĸ
    bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //tutorial13: �޸�ΪBGRA��ʽ
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;                //������,
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc)); //ȷ���������, tutorial5: ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    /* ����d3d�豸�ͽ�����
    tutorial13: �޸�
        ������������, ȡ��GPUӲ������(Ӳ��������D3D10��ʹ��: ��������Ⱦ), ����DEBUGģʽ��BGRA֧��ģʽ
    */
    hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
        D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, NULL,
        D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &D3d11Device, NULL, &D3d11DeviceContent);
    //���()�����׳�
    if (FAILED(hr))
    {
        MessageBox(NULL, DXGetErrorDescription(hr),
            TEXT("D3D11CreateDeviceAndSwapChain"), MB_OK);
        return 0;
    }

    /*
    tutorial13: ����
        ͨ����������ʼ��D3D10�豸 D2D�豸 DWrite�豸
    */
    InitialD2D_D3D10_DWrite(Adapter);
    Adapter->Release(); //�ú�����

    //tutori13: ȥ������Ҫ�Ĵ���
    //�������û���
    //ID3D11Texture2D* BackBuffer;
    hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&BackBuffer11);
    hr = D3d11Device->CreateRenderTargetView(BackBuffer11, NULL, &RenderTargetView);
    ////��Ӵ����׳�
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, DXGetErrorDescription(hr),
    //        TEXT("SwapChain->GetBuffer()"), MB_OK);
    //    return 0;
    //}

    ////������ȾĿ��
    //hr = D3d11Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
    //BackBuffer->Release();//���û���ʹ�����,ֱ�ӻ���
    ////��Ӵ����׳�
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, DXGetErrorDescription(hr),
    //        TEXT("D3d11Device->CreateRenderTargetView()"), MB_OK);
    //    return 0;
    //}

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

void ReleaseObjects()
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

    //tutorial11: ͸�����״̬, ˳ʱ����ʱ���դ״̬�ͷ�
    Transparency->Release();
    CCWcullMode->Release();
    CWcullMode->Release();

    //turorial12: ��Ⱦ״̬�ͷ�
    noCull->Release();

    /*
    tutorial13:
        ���� ������ȫ�ֱ������ͷ�
    */
    D3d10Device->Release();
    KeyedMutexD3d10->Release();
    KeyedMutexD3d11->Release();
    D2dRenderTarget->Release();
    ColorBrush->Release();
    BackBuffer11->Release();
    SharedTexture11->Release();
    D2dVertexBuffer->Release();
    D2dIndexBuffer->Release();
    D2dTexture->Release();
    DWriteFactory->Release();
    TextFormat->Release();
}

//�������������,��ͼ,����ģ��,����
bool InitializeScene()
{
    //tutorial13: ���� D2D���������ʼ��
    InitialD2DScreenTexture();

    //������ɫ����������ɫ��ͨ���ļ�����
    hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
    hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
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
    tutorial10:
        ���ڶ���ṹ���Ա任(�����滻��ɫ), �˴��Ķ��㼯�ϴ���ҲҪ�仯
        ����ȷ��ÿ������ͼ��Ҫ��Ⱦ��ȷ, ���һ��6*4�����㶼Ҫ����(��ʹ���ظ�)
    */
    Vertex v[] =
    {
        // Front Face
        Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
        Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f),
        Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f),
        Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

        // Back Face
        Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
        Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
        Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f),
        Vertex(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f),

        // Top Face
        Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
        Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f),
        Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f),
        Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

        // Bottom Face
        Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
        Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
        Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f),
        Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f),

        // Left Face
        Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
        Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f),
        Vertex(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f),
        Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

        // Right Face
        Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
        Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f),
        Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f),
        Vertex(1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
    };
    /*
    tutorial5:
        ��������,��012����һ��������,023������һ��, ��Ϊ����ͨ�������ν��д���
    tutorial8:
        ��������, �����������������
    tutorial10:
        ���ڶ��㼯�ϱ�Ϊ24��, ����ҲҪ�任
    */
    DWORD indices[] = {
        // Front Face
        0, 1, 2,
        0, 2, 3,

        // Back Face
        4, 5, 6,
        4, 6, 7,

        // Top Face
        8, 9, 10,
        8, 10, 11,

        // Bottom Face
        12, 13, 14,
        12, 14, 15,

        // Left Face
        16, 17, 18,
        16, 18, 19,

        // Right Face
        20, 21, 22,
        20, 22, 23 };

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
        ���¶��㻺�泤��, ������, 8������, ��ǰ��Ķ��㼯��������ͬ
    tutorial10:
        ���¶��㻺�泤��, ������, 24������, ��ǰ��Ķ��㼯��������ͬ
    */
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    //��������Դ��ʼ��, tutorial5:ʹ�ñ�������Ϊ��ʼ���Ĳ���, ����������
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = v;
    hr = D3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertexBuffer);
    //���㻺������
    UINT stride = sizeof(Vertex); //�����С
    UINT offset = 0;              //ƫ����
    D3d11DeviceContent->IASetVertexBuffers(0, 1,
        &squareVertexBuffer, &stride, &offset); //���㻺����IA��
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
    cameraProjection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)WIDTH / HEIGHT, 1.0f, 1000.0f);

    //tutorial9: ������դ״̬�Լ���RS��
    //tutorial10: Ϊ�˼�����ͼ���߿���Ⱦ��Ϊʵ����Ⱦ
    D3D11_RASTERIZER_DESC wireFrameDesc;
    ZeroMemory(&wireFrameDesc, sizeof(wireFrameDesc));
    wireFrameDesc.FillMode = D3D11_FILL_SOLID; //tutorial10: Ϊ�˼�����ͼ���߿���Ⱦ��Ϊʵ����Ⱦ
    wireFrameDesc.CullMode = D3D11_CULL_NONE;
    hr = D3d11Device->CreateRasterizerState(&wireFrameDesc, &WireFrame);
    /* tutorial9: WireFrame�������Ե�Ԫ��ʼ */
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, DXGetErrorDescription(hr), TEXT("������դ״̬"), MB_OK);
    //    return 0;
    //}
    /* WireFrame�������Ե�Ԫ���� */
    D3d11DeviceContent->RSSetState(WireFrame);

    //tutorial10: ���ļ��м�������
    //tutorial12: ����һ�����ʺϿ�������Ч����pngͼ, ʧ����
    hr = D3DX11CreateShaderResourceViewFromFile(D3d11Device, L"braynzar.png", NULL, NULL, &CubeTexture, NULL);
    /* tutorial9: CubeTexture�������Ե�Ԫ��ʼ */
    if (FAILED(hr))
    {
        MessageBox(NULL, DXGetErrorDescription(hr), TEXT("���ļ��м�������"), MB_OK);
        return 0;
    }
    /* CubeTexture�������Ե�Ԫ���� */
    //tutorial10: �����������������ʼ��
    D3D11_SAMPLER_DESC sampleDesc;
    ZeroMemory(&sampleDesc, sizeof(sampleDesc));
    sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //ʹ�����Բ�ֵ��С, �Ŵ�, ��mip������
    sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    //��ÿ��(u,v)�������ƽ������, ����u����(0,3), ��ƽ������
    sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampleDesc.MinLOD = 0;
    sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = D3d11Device->CreateSamplerState(&sampleDesc, &CubeTextureSampleState);
    /* tutorial10: CubeTextureSampleState�������Ե�Ԫ��ʼ */
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, DXGetErrorDescription(hr), TEXT("�������״̬"), MB_OK);
    //    return 0;
    //}
    /* CubeTextureSampleState�������Ե�Ԫ���� */

    //tutorial11: ������Ϸ�������
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));
    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR; //ԭ��������������ɫ��(RGB)
    /*
        ������Դ��ID3D11Device::OMSetBlendState���������õ�belnd factor: D3D11_BLEND_BLEND_FACTOR
    tutorial13: �޸�DestBlend
        ʹ��ֻ���ı���Ⱦ, ���������ǳ���
    */
    rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;                         //�����
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;                      //ԭ����(1,1,1,1)��ɫ
    rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;                    //ԭ����(0,0,0,0)��ɫ
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;                    //�����
    rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL; //RGBA�ĸ�������
    blendDesc.AlphaToCoverageEnable = false;                   //ȡ��A2C
    blendDesc.RenderTarget[0] = rtbd;
    D3d11Device->CreateBlendState(&blendDesc, &Transparency);
    /*
    tutorial11:
        ����˳ʱ����ʱ���޳�������դ��״̬, ����һ����դ������
        ÿ�λ�������������, ��������������תʱ, ���ܴ�����͸��������
    */
    D3D11_RASTERIZER_DESC cmDesc;
    ZeroMemory(&cmDesc, sizeof(cmDesc));
    cmDesc.FillMode = D3D11_FILL_SOLID;
    cmDesc.CullMode = D3D11_CULL_BACK;
    cmDesc.FrontCounterClockwise = true; //��ʱ��
    hr = D3d11Device->CreateRasterizerState(&cmDesc, &CCWcullMode);
    /* tutorial11: CCWcullMode�������Ե�Ԫ��ʼ */
    if (FAILED(hr))
    {
        MessageBox(NULL, DXGetErrorDescription(hr), TEXT("��ʱ���դ״̬"), MB_OK);
        return 0;
    }
    /* CCWcullMode�������Ե�Ԫ���� */
    cmDesc.FrontCounterClockwise = false; //˳ʱ��
    hr = D3d11Device->CreateRasterizerState(&cmDesc, &CWcullMode);
    /* tutorial11: CWcullMode�������Ե�Ԫ��ʼ */
    if (FAILED(hr))
    {
        MessageBox(NULL, DXGetErrorDescription(hr), TEXT("˳ʱ���դ״̬"), MB_OK);
        return 0;
    }
    /* CWcullMode�������Ե�Ԫ���� */

    //tutorial12: ȡ�������޳�
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    D3d11Device->CreateRasterizerState(&rasterDesc, &noCull);

    return true;
}

//tutorial8: ����������Χ����һ����������������ת
void UpdateScene(double time)
{
    //��֤��������ת
    //tutorial14: ����������ת��ʱ�����, ��ֹ��ͬFPSֵ������ת�ٲ�ͬ
    rot += 1.0f * time;
    if (rot > 6.28) //����2pi, �Ƕȹ���
        rot = 0.0f;
    //����������1
    cube1World = XMMatrixIdentity();
    //�趨������1������ռ����
    XMVECTOR rotationAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //��ת����(����), y��
    Rotation = XMMatrixRotationAxis(rotationAxis, rot);          //��ת����
    Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);         //ƽ�ƾ���
    cube1World = Translation * Rotation;
    //����������2
    cube2World = XMMatrixIdentity();
    //�趨������1������ռ����
    Rotation = XMMatrixRotationAxis(rotationAxis, -rot);
    Scale = XMMatrixScaling(1.3f, 1.3f, 1.3f); //����
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
    float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };                        //������ɫ��ʼ��Ϊ��
    D3d11DeviceContent->ClearRenderTargetView(RenderTargetView, bgColor); //������ɫ���

    //tutorial6: ˢ�����ģ����ͼ
    D3d11DeviceContent->ClearDepthStencilView(depthStencilView,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f, 0);

    //tutorial13: ���� ��Ⱦ������֮ǰ, ����ȷ�Ķ�������������, ����tutorial13�������µ�
    D3d11DeviceContent->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    UINT stride = sizeof(Vertex); //�����С
    UINT offset = 0;              //ƫ����
    D3d11DeviceContent->IAGetVertexBuffers(0, 1, &squareVertexBuffer, &stride, &offset);

    //tutorial12: ��Ĭ����Ⱦ, Ȼ���ޱ����޳���Ⱦ
    //tutorial13: ȥ����Ⱦ����, ��renderText���Ѿ�ʵ��
    //D3d11DeviceContent->RSSetState(NULL);//nullΪĬ��
    //D3d11DeviceContent->RSSetState(noCull);

    //tutorial12: ��������Ч����˽��û��Ч��
    ////tutorial11: �����������, �������״̬��OM��
    //float blendFactor[] = { 0.75f,0.75f,0.75f,1.0f };//�������RGB��25%͸��
    //D3d11DeviceContent->OMSetBlendState(0, 0, 0xffffffff);//���û��״̬���ڻ��Ʋ�͸������
    ///*    ��Ⱦ��͸���������   */
    //D3d11DeviceContent->OMSetBlendState(Transparency, blendFactor, 0xffffffff);//���û��״̬���ڻ���͸������
    ///*    ��Ⱦ͸���������   */

    ///*
    //tutorial11:
    //    �����ж������������λ�ù�ϵ, �Ӷ�ʹ����Ⱦ��ϵ�������, �Ӷ������ںϳ�����(����ȾԶ���������)
    //*/
    //XMVECTOR cubePosition = XMVectorZero();//��������
    ////��ȡ������1λ���������λ�õľ���
    //cubePosition = XMVector3TransformCoord(cubePosition, cube1World);//ͨ��������1�����������λ��
    //float distX = XMVectorGetX(cubePosition) - XMVectorGetX(cameraPosition);
    //float distY = XMVectorGetY(cubePosition) - XMVectorGetY(cameraPosition);
    //float distZ = XMVectorGetZ(cubePosition) - XMVectorGetZ(cameraPosition);
    //float cube1Distance = distX * distX + distY * distY + distZ * distZ;//ֻ�ǱȽ�, ����sqrt�õ���ȷ��ֵ, ����������ʧ
    //cubePosition = XMVectorZero();//��������
    ////��ȡ������2λ���������λ�õľ���
    //cubePosition = XMVector3TransformCoord(cubePosition, cube2World);//ͨ��������2�����������λ��
    //distX = XMVectorGetX(cubePosition) - XMVectorGetX(cameraPosition);
    //distY = XMVectorGetY(cubePosition) - XMVectorGetY(cameraPosition);
    //distZ = XMVectorGetZ(cubePosition) - XMVectorGetZ(cameraPosition);
    //float cube2Distance = distX * distX + distY * distY + distZ * distZ;
    ////����������������ȫ��ͬ, �����Ҫ�����ø�Զ���������, �ȽϺ�, ֱ�����߻�������
    //if (cube1Distance < cube2Distance)
    //{
    //    XMMATRIX tempMatrix = cube1World;
    //    cube1World = cube2World;
    //    cube2World = tempMatrix;//����cube1World������ø�Զ������Ⱦ��������
    //}
    /*    tutorial12: ���û��Ч������   */

    //tutorial7: ��������ռ�ת��������WVP����
    //tutorial8: ����cube1
    //worldSpace = XMMatrixIdentity();//����һ���վ���, tutorial8: cube1World��cube2World����
    WVP = cube1World * cameraView * cameraProjection; //һ���ռ�ת����ʽ
    //tutorial7: ���³�������
    cbPerObj.WVP = XMMatrixTranspose(WVP); //����ת��
    D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

    //tutorial10: ��������ɫ������������Ϣ���������״̬��Ϣ
    D3d11DeviceContent->PSSetShaderResources(0, 1, &CubeTexture);
    D3d11DeviceContent->PSSetSamplers(0, 1, &CubeTextureSampleState);

    //tutorial11: ����ʱ���޳�, �Ի������
    //tutorial12: ���û��Ч��
    //D3d11DeviceContent->RSSetState(CCWcullMode);

    /*
    tutorial5:
        ʹ��DrawIndexed()����������
    tutorial8:
        ������������Ϊ12*3, ����cube1
    */
    D3d11DeviceContent->DrawIndexed(36, 0, 0);

    //tutorial11: ˳ʱ���޳�, �Ի�ñ���
    //tutorial12: ���û��Ч��
    //D3d11DeviceContent->RSSetState(CWcullMode);
    //D3d11DeviceContent->DrawIndexed(36, 0, 0);

    //tutorial8: ����cube2
    WVP = cube2World * cameraView * cameraProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP); //����ת��
    D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
    //tutorial10: ��������ɫ������������Ϣ���������״̬��Ϣ
    D3d11DeviceContent->PSSetShaderResources(0, 1, &CubeTexture);
    D3d11DeviceContent->PSSetSamplers(0, 1, &CubeTextureSampleState);

    //tutorial12: ���û��Ч��
    //tutorial11: ����ʱ���޳�, �Ի������
    //D3d11DeviceContent->RSSetState(CCWcullMode);

    //D3d11DeviceContent->DrawIndexed(36, 0, 0);

    ////tutorial11: ����ʱ���޳�, �Ի������
    //D3d11DeviceContent->RSSetState(CWcullMode);
    D3d11DeviceContent->DrawIndexed(36, 0, 0);

    //tutorial13: ���� ��������Ⱦ��Ϻ�, ��ʼ��Ⱦ����
    RenderText(L"FPS: ", fps);

    //��������ǰ�û���ӳ�䵽��ʾ��, ��ͼ�����
    SwapChain->Present(0, 0);
}

/*
tutorial13:
    ����D2D D3D10 DWrite�豸��ʼ��, ͨ���������ķ�ʽ����������Ⱦ
*/
bool InitialD2D_D3D10_DWrite(IDXGIAdapter1 *Adapter)
{
    //tutorial13: ����D3D10.1�豸��ʼ��
    hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL,
        D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
        D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &D3d10Device);

    //tutorial13: ����sharedTexture��ʼ��
    D3D11_TEXTURE2D_DESC SharedTextureDesc;
    ZeroMemory(&SharedTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
    SharedTextureDesc.Width = WIDTH;
    SharedTextureDesc.Height = HEIGHT;
    SharedTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    SharedTextureDesc.MipLevels = 1;
    SharedTextureDesc.ArraySize = 1;
    SharedTextureDesc.SampleDesc.Count = 1;
    SharedTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    SharedTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    SharedTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    hr = D3d11Device->CreateTexture2D(&SharedTextureDesc, NULL, &SharedTexture11);

    //tutorial13: ���� ��ȡ����������ź���IDXGIKeyedMutex
    hr = SharedTexture11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&KeyedMutexD3d11);

    //tutorial13: ���� ͨ��DXGI����D3D10�豸���Է��ʵ�D3D11����������
    IDXGIResource *SharedResource10;
    HANDLE SharedHandle10;
    hr = SharedTexture11->QueryInterface(__uuidof(IDXGIResource), (void **)&SharedResource10);
    hr = SharedResource10->GetSharedHandle(&SharedHandle10); //�ɷ��ʵľ��
    SharedResource10->Release();

    //tutorial13: ���� ΪD3D10�豸����D3D11�������ӻ����ź���(���ʹ������)
    IDXGISurface1 *SharedSurface10;
    hr = D3d10Device->OpenSharedResource(SharedHandle10,
        __uuidof(IDXGISurface1), (void **)&SharedSurface10);
    hr = SharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&KeyedMutexD3d10);

    //tutorial13: ���� ͨ��D2DFactory����D2D��Դ
    ID2D1Factory *D2dFactory;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void **)&D2dFactory);

    /*
    tutorial13: ����
        ����D2D��ȾĿ������
        Ӳ����Ⱦ + ���ظ�ʽ: Ĭ�ϵ�δ֪��ʽ+��͸��
    */
    D2D1_RENDER_TARGET_PROPERTIES D2dRenderTargetProperties;
    ZeroMemory(&D2dRenderTargetProperties, sizeof(D2dRenderTargetProperties));
    D2dRenderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
    D2dRenderTargetProperties.pixelFormat = D2D1::PixelFormat(
        DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

    /*
    tutorial13: ����
        ����DXGI��ȾĿ������, ʹ��D2D��D3D10�豸�ܹ�����
    */
    hr = D2dFactory->CreateDxgiSurfaceRenderTarget(
        SharedSurface10, &D2dRenderTargetProperties, &D2dRenderTarget);
    SharedSurface10->Release();
    D2dFactory->Release();

    //tutorial13: ���� ������ˢ
    hr = D2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &ColorBrush);

    /*
    tutorial13: ����
        ��ʼ��DWrite: ����DW Factory �����ʽ
    */
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown **>(&DWriteFactory));

    //tutorial13: ���� �����ʽ
    hr = DWriteFactory->CreateTextFormat(
        L"Script", NULL,
        DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        44.0f, L"en-us", &TextFormat);

    //tutorial13: ���� ������������
    hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);        //ˮƽ���������
    hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR); //��ֱ���򶥶˶���

    //tutorial13: ���� ��ֹDebug������D3D10�豸ͼԪ����
    D3d10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    return true;
}

/*
tutorial13:
    ����D2D �����ʼ��
    ���������θ��ǳ���
    �洢���������붥�㻺��
    ͨ��D2D��D3D10�Ĺ�����������ɫ����Դ��ͼ(shader resource view)
    ͨ����ɫ����Դ��ͼ���Ǵ�����������, ͨ�����ʹ������ɼ�
*/
void InitialD2DScreenTexture()
{
    //tutorial13: ���� ���ǳ����������εĶ�������������
    Vertex v[] =
    {
        Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
        Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f),
        Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f),
        Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
    };
    DWORD indices[] = {
        0,
        1,
        2,
        0,
        2,
        3,
    };
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA indexInitialData;
    indexInitialData.pSysMem = indices;
    D3d11Device->CreateBuffer(&indexBufferDesc, &indexInitialData, &D2dIndexBuffer);

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = v;
    hr = D3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &D2dVertexBuffer);

    //tutorial13: ���� ͨ��������������ɫ����Դ��ͼ
    D3d11Device->CreateShaderResourceView(SharedTexture11, NULL, &D2dTexture);
}

//tutorial13: ���� ��Ⱦ����
void RenderText(std::wstring text, int inInt)
{
    //tutorial13: ���� D3D11�豸���ͷŹ�������ʹ��D3D10�豸�ܹ��Ӵ�(����)
    KeyedMutexD3d11->ReleaseSync(0);
    KeyedMutexD3d10->AcquireSync(0, 5); //���ٽӴ�5ms
    //tutorial13: ���� D3D10�豸�Ӵ���, D2DĿ�꿪ʼ��Ⱦ����ձ���, alpha=0ȫ͸
    D2dRenderTarget->BeginDraw();
    D2dRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    //tutorial13: ���� �����������Ļ�ϵ��ַ���
    std::wostringstream printString;
    //tutorial14: �޸� ����FPSֵ����ʾ
    printString << text << inInt;
    printText = printString.str();
    //tutorial13: ���� ������ɫ, ���������ˢ
    D2D1_COLOR_F fontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
    ColorBrush->SetColor(fontColor);
    //tutorial13: ���� ����һ���������ֿ�: ǰ����Ϊ�û�(x,y)��ʼλ��, ������Ϊ�����С
    D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, WIDTH, HEIGHT);
    //tutorial13: ���� �������ֺ�, ��������
    D2dRenderTarget->DrawTextW(
        printText.c_str(), wcslen(printText.c_str()), TextFormat, layoutRect, ColorBrush);
    D2dRenderTarget->EndDraw();
    //tutorial13: ���� D3D10�豸�������� D3D11����
    KeyedMutexD3d10->ReleaseSync(1);
    KeyedMutexD3d11->AcquireSync(1, 5);
    //tutorial13: ���� ���û��״̬���ڻ���͸��������и���
    D3d11DeviceContent->OMSetBlendState(Transparency, NULL, 0xffffffff);
    //tutorial13: ���� D2D������������IA��
    D3d11DeviceContent->IASetIndexBuffer(D2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    //tutorial13: ���� D2D���㻺���IA
    UINT stride = sizeof(Vertex); //�����С
    UINT offset = 0;              //ƫ����
    D3d11DeviceContent->IAGetVertexBuffers(0, 1, &D2dVertexBuffer, &stride, &offset);
    //tutorial13: ���� ����WVP����
    WVP = XMMatrixIdentity(); //���þ���
    cbPerObj.WVP = XMMatrixTranspose(WVP);
    D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
    //tutorial13: ���� ���ù����D2D��Դ�������
    D3d11DeviceContent->PSSetShaderResources(0, 1, &D2dTexture);
    D3d11DeviceContent->PSGetSamplers(0, 1, &CubeTextureSampleState);
    //tutorial13: ���� ��Ⱦ������
    D3d11DeviceContent->RSSetState(CWcullMode); //˳ʱ����Ⱦ, �޳�����
    D3d11DeviceContent->DrawIndexed(6, 0, 0);
}

//tutorial14: ���� ������ʱ��
void StartTimer()
{
    LARGE_INTEGER frequencyCount;
    QueryPerformanceFrequency(&frequencyCount);

    CountsPerSecond = double(frequencyCount.QuadPart);

    QueryPerformanceCounter(&frequencyCount);
    CounterStart = frequencyCount.QuadPart;
}

//tutorial14: ���� �õ���ʱ����ʼ�󾭹���ʱ��
double GetTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return double(currentTime.QuadPart - CounterStart) / CountsPerSecond;
}

//tutorial14: ���� ����ÿ֡ʱ��
double GetFrameTime()
{
    LARGE_INTEGER currentTime;
    __int64 tickCount;
    QueryPerformanceCounter(&currentTime);

    tickCount = currentTime.QuadPart - FrameTimeOld;
    FrameTimeOld = currentTime.QuadPart;

    if (tickCount < 0.0f)
        tickCount = 0.0f;

    return float(tickCount) / CountsPerSecond;
}
/* ** ����ʵ�� ** */
