//Include and link appropriate libraries and headers, 解决LNK2019 无法解析的外部符号
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "DxErr.lib")
//tutorial13: 新增库以满足DX10中进行D2D文字渲染
#pragma comment(lib, "D3D10_1.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <DxErr.h> //用于错误抛出
//tutorial13: 新增库以满足DX10中进行D2D文字渲染
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>

/* 全局变量 */

LPCTSTR WndClassName = L"firstwindow"; //窗体名称
HWND hwnd = NULL;                      //窗体进程
//窗口长宽
const int WIDTH = 300;
const int HEIGHT = 300;

//交换链以及d3d设备、上下文、渲染目标视图
IDXGISwapChain *SwapChain;
ID3D11Device *D3d11Device;
ID3D11DeviceContext *D3d11DeviceContent;
ID3D11RenderTargetView *RenderTargetView;
//颜色,用于背景颜色更改
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
HRESULT hr; //用于错误监测

//ID3D11Buffer* triangleVertBuffer;//三角形顶点缓存, tutorial5: 被正方形顶点缓存替代
ID3D11VertexShader *VS;          //顶点着色器
ID3D11PixelShader *PS;           //像素着色器
ID3D10Blob *VS_Buffer;           //顶点着色器缓存
ID3D10Blob *PS_Buffer;           //像素着色器缓存
ID3D11InputLayout *vertexLayout; //顶点输入布局

/*
tutorial10:
    删除颜色属性, 替换为纹理属性
*/
struct Vertex //顶点结构
{
    Vertex() {}
    //tutorial4: 增加RGBA颜色元素, tutorial10: 替换为纹理属性
    Vertex(float x, float y, float z,
        float u, float v) : position(x, y, z), textureCoord(u, v) {}

    XMFLOAT3 position;
    XMFLOAT2 textureCoord;
};
//输入布局, tutorial10: 根据顶点结构修改
D3D11_INPUT_ELEMENT_DESC layout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, //偏移量
};
UINT numElements = ARRAYSIZE(layout); //保存布局数组的大小

//tutorial5: 加入索引缓存, 为了展示索引的功能, 加入矩形演示
//矩形的顶点缓存与索引缓存
ID3D11Buffer *squareIndexBuffer;
ID3D11Buffer *squareVertexBuffer;

//tutorial6: 加入深度/模板视图与深度/模板缓存
ID3D11DepthStencilView *depthStencilView;
ID3D11Texture2D *depthStencilBuffer;

//tutorial7: 加入常量缓存, 存储在WVP矩阵, 逐对象刷新
ID3D11Buffer *cbPerObjectBuffer;
//tutorial7: 加入各个空间转换矩阵的定义
XMMATRIX WVP;
XMMATRIX worldSpace;
XMMATRIX cameraView;
XMMATRIX cameraProjection;
//tutorial7: 加入向量定义
XMVECTOR cameraPosition;
XMVECTOR cameraTarget;
XMVECTOR cameraUp; //摄像机的向上方向, 可见Note中的 视图空间(view space)
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
float rot = 0.01f; //旋转角度

//tutorial9: 设置管道的RS阶段的呈现状态
ID3D11RasterizerState *WireFrame;

//tutorial10: 新增保存从文件中加载的纹理以及保存采样器状态信息的接口
ID3D11ShaderResourceView *CubeTexture;
ID3D11SamplerState *CubeTextureSampleState;

//tutorial11: 新增一个混合状态, 两个光栅状态, 其中一个顺时针另一个逆时针
ID3D11BlendState *Transparency;
ID3D11RasterizerState *CCWcullMode; //counter clockwise 逆时针
ID3D11RasterizerState *CWcullMode;  //clockwise 顺时针

//turorial12: 新增一个新的渲染状态, 用于看见正方体的后面部分
ID3D11RasterizerState *noCull;

/*
tutorial13: 新增
    D3D10设备 D3d10Device
    控制D3D10与D3D11设备的信号量 KeyedMutexD3d10 KeyedMutexD3d11
    D2D的渲染目标 D2dRenderTarget
    D2D的固体颜色笔刷 ColorBrush: 用于写字
    存储2D文字纹理的后置缓存 BackBuffer11
    共享文字纹理 SharedTexture11
    D2D顶点缓存 D2dVertexBuffer
    D2D索引缓存 D2dIndexBuffer
    D2D着色器资源视图 D2dTexture
    DWriteFactory: 渲染文字的接口, 通知D2D如何绘制
    TextFormat: 文字格式
    用于文字传输的宽字符串 printText
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

//tutorial14: 新增 用于传递FPS值
double CountsPerSecond = 0.0;
__int64 CounterStart = 0;

int FrameCount = 0;
int fps = 0;

__int64 FrameTimeOld = 0;
double FrameTime;

/* ** 全局变量 ** */

/* 函数声明 */

//窗口初始化,涉及类、窗口创建以及窗口显示与更新
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int Width, int Height, bool windowed);
//运行主体
int MessageLoop();
//按键响应,涉及esc热键退出以及窗口销毁
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//d3d11初始化
bool InitializeDirect3dApp(HINSTANCE hInstance);
//释放对象,防止内存泄漏
void ReleaseObjects();
//初始化场景
bool InitializeScene();
//场景的每帧更新
//tutorial14: 修改 新增time形参, 根据时间进行更新, 防止不同FPS值下物体运动速度不同
void UpdateScene(double time);
//场景映射到屏幕
void DrawScene();

/*
tutorrial13: 新增
    InitialD2D_D3D10_DWrite: 初始化D2D D3D10 DWrite
    InitialD2DScreenTexture: 初始化D2D纹理
    RenderText: 渲染文字
*/
bool InitialD2D_D3D10_DWrite(IDXGIAdapter1 *Adapter);
void InitialD2DScreenTexture();
//tutorial14: 修改 新增inInt形参, 传递FPS值, 虽然FPS是全局变量, 但为了结构的清晰仍然使用参数传递
void RenderText(std::wstring text, int inInt);

/*
tutorial14: 新增
    StartTimer: 启动计时器
    GetTime: 获取时间
    GetFrameTime: 获取每帧时间
*/
void StartTimer();
double GetTime();
double GetFrameTime();

/* ** 函数声明 ** */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    //监测窗体是否初始化
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

    //主体
    MessageLoop();

    //释放内存
    ReleaseObjects();

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
            //tutorial14: 新增 计算每秒帧数
            FrameCount++;         //每次循环则有一帧
            if (GetTime() > 1.0f) //一秒后, 统计一秒内的帧数
            {
                fps = FrameCount;
                FrameCount = 0;
                StartTimer(); //重新计时
            }
            FrameTime = GetFrameTime(); //每一帧的时间

            UpdateScene(FrameTime); //根据这个时间进行场景更新
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
    tutorial13: 新增
        DXGIFactory, 从而能枚举出
        Adapter, 通过适配器使得D3D11设备能与D3D10同步
    */
    IDXGIFactory1 *DXGIFactory;
    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&DXGIFactory);
    IDXGIAdapter1 *Adapter;
    hr = DXGIFactory->EnumAdapters1(0, &Adapter);
    DXGIFactory->Release(); //用后即销毁

    /*
    tutorial13: 修改
        BGRA格式
    */
    DXGI_MODE_DESC bufferDesc;                   // 后置缓冲
    ZeroMemory(&bufferDesc, sizeof(bufferDesc)); //确保对象清空, tutorial5 :使用变量名做为初始化的参数, 而非类型名
    bufferDesc.Width = WIDTH;
    bufferDesc.Height = HEIGHT;
    //144Hz: 144/1
    bufferDesc.RefreshRate.Numerator = 144;         //分子
    bufferDesc.RefreshRate.Denominator = 1;         //分母
    bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //tutorial13: 修改为BGRA格式
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;                //交换链,
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc)); //确保对象清空, tutorial5: 使用变量名做为初始化的参数, 而非类型名
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    /* 创建d3d设备和交换链
    tutorial13: 修改
        进行适配器绑定, 取消GPU硬件驱动(硬件驱动在D3D10中使用: 对文字渲染), 增加DEBUG模式与BGRA支持模式
    */
    hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
        D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, NULL,
        D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &D3d11Device, NULL, &D3d11DeviceContent);
    //添加()错误抛出
    if (FAILED(hr))
    {
        MessageBox(NULL, DXGetErrorDescription(hr),
            TEXT("D3D11CreateDeviceAndSwapChain"), MB_OK);
        return 0;
    }

    /*
    tutorial13: 新增
        通过适配器初始化D3D10设备 D2D设备 DWrite设备
    */
    InitialD2D_D3D10_DWrite(Adapter);
    Adapter->Release(); //用后即销毁

    //tutori13: 去除不必要的代码
    //创建后置缓存
    //ID3D11Texture2D* BackBuffer;
    hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&BackBuffer11);
    hr = D3d11Device->CreateRenderTargetView(BackBuffer11, NULL, &RenderTargetView);
    ////添加错误抛出
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, DXGetErrorDescription(hr),
    //        TEXT("SwapChain->GetBuffer()"), MB_OK);
    //    return 0;
    //}

    ////创建渲染目标
    //hr = D3d11Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
    //BackBuffer->Release();//后置缓存使用完毕,直接回收
    ////添加错误抛出
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, DXGetErrorDescription(hr),
    //        TEXT("D3d11Device->CreateRenderTargetView()"), MB_OK);
    //    return 0;
    //}

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

void ReleaseObjects()
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

    //tutorial11: 透明混合状态, 顺时针逆时针光栅状态释放
    Transparency->Release();
    CCWcullMode->Release();
    CWcullMode->Release();

    //turorial12: 渲染状态释放
    noCull->Release();

    /*
    tutorial13:
        新增 新增的全局变量的释放
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

//在这里放置物体,贴图,加载模型,音乐
bool InitializeScene()
{
    //tutorial13: 新增 D2D共享纹理初始化
    InitialD2DScreenTexture();

    //顶点着色器与像素着色器通过文件编译
    hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
    hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
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
    tutorial10:
        由于顶点结构属性变换(纹理替换颜色), 此处的顶点集合创建也要变化
        由于确保每个面贴图都要渲染正确, 因此一共6*4个顶点都要加入(即使有重复)
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
        增加索引,点012构成一个三角形,023构成另一个, 因为布局通过三角形进行传输
    tutorial8:
        增加索引, 构成正方体的六个面
    tutorial10:
        由于顶点集合变为24个, 索引也要变换
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
        更新顶点缓存长度, 正方体, 8个顶点, 与前面的顶点集合数量相同
    tutorial10:
        更新顶点缓存长度, 正方体, 24个顶点, 与前面的顶点集合数量相同
    */
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    //顶点子资源初始化, tutorial5:使用变量名做为初始化的参数, 而非类型名
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = v;
    hr = D3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertexBuffer);
    //顶点缓存设置
    UINT stride = sizeof(Vertex); //顶点大小
    UINT offset = 0;              //偏移量
    D3d11DeviceContent->IASetVertexBuffers(0, 1,
        &squareVertexBuffer, &stride, &offset); //顶点缓存与IA绑定
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
    cameraProjection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)WIDTH / HEIGHT, 1.0f, 1000.0f);

    //tutorial9: 创建光栅状态以及与RS绑定
    //tutorial10: 为了加载贴图将线框渲染改为实体渲染
    D3D11_RASTERIZER_DESC wireFrameDesc;
    ZeroMemory(&wireFrameDesc, sizeof(wireFrameDesc));
    wireFrameDesc.FillMode = D3D11_FILL_SOLID; //tutorial10: 为了加载贴图将线框渲染改为实体渲染
    wireFrameDesc.CullMode = D3D11_CULL_NONE;
    hr = D3d11Device->CreateRasterizerState(&wireFrameDesc, &WireFrame);
    /* tutorial9: WireFrame创建测试单元开始 */
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, DXGetErrorDescription(hr), TEXT("创建光栅状态"), MB_OK);
    //    return 0;
    //}
    /* WireFrame创建测试单元结束 */
    D3d11DeviceContent->RSSetState(WireFrame);

    //tutorial10: 从文件中加载纹理
    //tutorial12: 换成一个更适合看出裁切效果的png图, 失败了
    hr = D3DX11CreateShaderResourceViewFromFile(D3d11Device, L"braynzar.png", NULL, NULL, &CubeTexture, NULL);
    /* tutorial9: CubeTexture创建测试单元开始 */
    if (FAILED(hr))
    {
        MessageBox(NULL, DXGetErrorDescription(hr), TEXT("从文件中加载纹理"), MB_OK);
        return 0;
    }
    /* CubeTexture创建测试单元结束 */
    //tutorial10: 采样器描述创建与初始化
    D3D11_SAMPLER_DESC sampleDesc;
    ZeroMemory(&sampleDesc, sizeof(sampleDesc));
    sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //使用线性插值缩小, 放大, 和mip级采样
    sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    //在每个(u,v)整数结点平铺纹理, 例如u属于(0,3), 则平铺三次
    sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampleDesc.MinLOD = 0;
    sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = D3d11Device->CreateSamplerState(&sampleDesc, &CubeTextureSampleState);
    /* tutorial10: CubeTextureSampleState创建测试单元开始 */
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, DXGetErrorDescription(hr), TEXT("纹理采样状态"), MB_OK);
    //    return 0;
    //}
    /* CubeTextureSampleState创建测试单元结束 */

    //tutorial11: 新增混合方程描述
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));
    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR; //原数据来自像素着色器(RGB)
    /*
        数据来源于ID3D11Device::OMSetBlendState函数中设置的belnd factor: D3D11_BLEND_BLEND_FACTOR
    tutorial13: 修改DestBlend
        使得只有文本渲染, 其余纹理覆盖场景
    */
    rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;                         //运算符
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;                      //原数据(1,1,1,1)白色
    rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;                    //原数据(0,0,0,0)黑色
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;                    //运算符
    rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL; //RGBA四个都启用
    blendDesc.AlphaToCoverageEnable = false;                   //取消A2C
    blendDesc.RenderTarget[0] = rtbd;
    D3d11Device->CreateBlendState(&blendDesc, &Transparency);
    /*
    tutorial11:
        生成顺时针逆时针剔除两个光栅器状态, 新增一个光栅器描述
        每次绘制两次正方体, 这样在正方体旋转时, 就能从正面透视至背面
    */
    D3D11_RASTERIZER_DESC cmDesc;
    ZeroMemory(&cmDesc, sizeof(cmDesc));
    cmDesc.FillMode = D3D11_FILL_SOLID;
    cmDesc.CullMode = D3D11_CULL_BACK;
    cmDesc.FrontCounterClockwise = true; //逆时针
    hr = D3d11Device->CreateRasterizerState(&cmDesc, &CCWcullMode);
    /* tutorial11: CCWcullMode创建测试单元开始 */
    if (FAILED(hr))
    {
        MessageBox(NULL, DXGetErrorDescription(hr), TEXT("逆时针光栅状态"), MB_OK);
        return 0;
    }
    /* CCWcullMode创建测试单元结束 */
    cmDesc.FrontCounterClockwise = false; //顺时针
    hr = D3d11Device->CreateRasterizerState(&cmDesc, &CWcullMode);
    /* tutorial11: CWcullMode创建测试单元开始 */
    if (FAILED(hr))
    {
        MessageBox(NULL, DXGetErrorDescription(hr), TEXT("顺时针光栅状态"), MB_OK);
        return 0;
    }
    /* CWcullMode创建测试单元结束 */

    //tutorial12: 取消背面剔除
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    D3d11Device->CreateRasterizerState(&rasterDesc, &noCull);

    return true;
}

//tutorial8: 更新正方体围绕另一个自旋的正方体旋转
void UpdateScene(double time)
{
    //保证正方体旋转
    //tutorial14: 新增物体旋转与时间关联, 防止不同FPS值下物体转速不同
    rot += 1.0f * time;
    if (rot > 6.28) //大于2pi, 角度归零
        rot = 0.0f;
    //重置正方体1
    cube1World = XMMatrixIdentity();
    //设定正方体1的世界空间矩阵
    XMVECTOR rotationAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //旋转方向(向量), y轴
    Rotation = XMMatrixRotationAxis(rotationAxis, rot);          //旋转矩阵
    Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);         //平移矩阵
    cube1World = Translation * Rotation;
    //重置正方体2
    cube2World = XMMatrixIdentity();
    //设定正方体1的世界空间矩阵
    Rotation = XMMatrixRotationAxis(rotationAxis, -rot);
    Scale = XMMatrixScaling(1.3f, 1.3f, 1.3f); //缩放
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
    float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };                        //背景颜色初始化为黑
    D3d11DeviceContent->ClearRenderTargetView(RenderTargetView, bgColor); //背景颜色清空

    //tutorial6: 刷新深度模板视图
    D3d11DeviceContent->ClearDepthStencilView(depthStencilView,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f, 0);

    //tutorial13: 新增 渲染正方形之前, 绑定正确的顶点与索引缓存, 由于tutorial13创建了新的
    D3d11DeviceContent->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    UINT stride = sizeof(Vertex); //顶点大小
    UINT offset = 0;              //偏移量
    D3d11DeviceContent->IAGetVertexBuffers(0, 1, &squareVertexBuffer, &stride, &offset);

    //tutorial12: 先默认渲染, 然后无背面剔除渲染
    //tutorial13: 去除渲染部分, 在renderText中已经实现
    //D3d11DeviceContent->RSSetState(NULL);//null为默认
    //D3d11DeviceContent->RSSetState(noCull);

    //tutorial12: 产生裁切效果因此禁用混合效果
    ////tutorial11: 新增混合因子, 新增混合状态与OM绑定
    //float blendFactor[] = { 0.75f,0.75f,0.75f,1.0f };//混合因子RGB中25%透明
    //D3d11DeviceContent->OMSetBlendState(0, 0, 0xffffffff);//禁用混合状态用于绘制不透明对象
    ///*    渲染不透明对象代码   */
    //D3d11DeviceContent->OMSetBlendState(Transparency, blendFactor, 0xffffffff);//启用混合状态用于绘制透明对象
    ///*    渲染透明对象代码   */

    ///*
    //tutorial11:
    //    新增判断两个正方体的位置关系, 从而使得渲染关系不会出错, 从而导致融合出问题(先渲染远离摄像机的)
    //*/
    //XMVECTOR cubePosition = XMVectorZero();//向量清零
    ////获取正方体1位置与摄像机位置的距离
    //cubePosition = XMVector3TransformCoord(cubePosition, cube1World);//通过正方体1的世界矩阵获得位置
    //float distX = XMVectorGetX(cubePosition) - XMVectorGetX(cameraPosition);
    //float distY = XMVectorGetY(cubePosition) - XMVectorGetY(cameraPosition);
    //float distZ = XMVectorGetZ(cubePosition) - XMVectorGetZ(cameraPosition);
    //float cube1Distance = distX * distX + distY * distY + distZ * distZ;//只是比较, 无需sqrt得到正确的值, 降低性能损失
    //cubePosition = XMVectorZero();//向量清零
    ////获取正方体2位置与摄像机位置的距离
    //cubePosition = XMVector3TransformCoord(cubePosition, cube2World);//通过正方体2的世界矩阵获得位置
    //distX = XMVectorGetX(cubePosition) - XMVectorGetX(cameraPosition);
    //distY = XMVectorGetY(cubePosition) - XMVectorGetY(cameraPosition);
    //distZ = XMVectorGetZ(cubePosition) - XMVectorGetZ(cameraPosition);
    //float cube2Distance = distX * distX + distY * distY + distZ * distZ;
    ////由于两个正方体完全相同, 因此想要获得离得更远的世界矩阵, 比较后, 直接两者互换即可
    //if (cube1Distance < cube2Distance)
    //{
    //    XMMATRIX tempMatrix = cube1World;
    //    cube1World = cube2World;
    //    cube2World = tempMatrix;//这样cube1World就是离得更远更先渲染的正方体
    //}
    /*    tutorial12: 禁用混合效果结束   */

    //tutorial7: 定义世界空间转换矩阵与WVP矩阵
    //tutorial8: 绘制cube1
    //worldSpace = XMMatrixIdentity();//返回一个空矩阵, tutorial8: cube1World与cube2World代替
    WVP = cube1World * cameraView * cameraProjection; //一个空间转换公式
    //tutorial7: 更新常量缓存
    cbPerObj.WVP = XMMatrixTranspose(WVP); //矩阵转置
    D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

    //tutorial10: 向像素着色器发送纹理信息与纹理采样状态信息
    D3d11DeviceContent->PSSetShaderResources(0, 1, &CubeTexture);
    D3d11DeviceContent->PSSetSamplers(0, 1, &CubeTextureSampleState);

    //tutorial11: 先逆时针剔除, 以获得正面
    //tutorial12: 禁用混合效果
    //D3d11DeviceContent->RSSetState(CCWcullMode);

    /*
    tutorial5:
        使用DrawIndexed()绘制正方形
    tutorial8:
        更新索引数量为12*3, 绘制cube1
    */
    D3d11DeviceContent->DrawIndexed(36, 0, 0);

    //tutorial11: 顺时针剔除, 以获得背面
    //tutorial12: 禁用混合效果
    //D3d11DeviceContent->RSSetState(CWcullMode);
    //D3d11DeviceContent->DrawIndexed(36, 0, 0);

    //tutorial8: 绘制cube2
    WVP = cube2World * cameraView * cameraProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP); //矩阵转置
    D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
    //tutorial10: 向像素着色器发送纹理信息与纹理采样状态信息
    D3d11DeviceContent->PSSetShaderResources(0, 1, &CubeTexture);
    D3d11DeviceContent->PSSetSamplers(0, 1, &CubeTextureSampleState);

    //tutorial12: 禁用混合效果
    //tutorial11: 先逆时针剔除, 以获得正面
    //D3d11DeviceContent->RSSetState(CCWcullMode);

    //D3d11DeviceContent->DrawIndexed(36, 0, 0);

    ////tutorial11: 先逆时针剔除, 以获得正面
    //D3d11DeviceContent->RSSetState(CWcullMode);
    D3d11DeviceContent->DrawIndexed(36, 0, 0);

    //tutorial13: 新增 正方体渲染完毕后, 开始渲染字体
    RenderText(L"FPS: ", fps);

    //交换链将前置缓存映射到显示器, 即图像呈现
    SwapChain->Present(0, 0);
}

/*
tutorial13:
    新增D2D D3D10 DWrite设备初始化, 通过共享表面的方式进行文字渲染
*/
bool InitialD2D_D3D10_DWrite(IDXGIAdapter1 *Adapter)
{
    //tutorial13: 新增D3D10.1设备初始化
    hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL,
        D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
        D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &D3d10Device);

    //tutorial13: 新增sharedTexture初始化
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

    //tutorial13: 新增 获取共享纹理的信号量IDXGIKeyedMutex
    hr = SharedTexture11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&KeyedMutexD3d11);

    //tutorial13: 新增 通过DXGI创建D3D10设备可以访问的D3D11纹理句柄交互
    IDXGIResource *SharedResource10;
    HANDLE SharedHandle10;
    hr = SharedTexture11->QueryInterface(__uuidof(IDXGIResource), (void **)&SharedResource10);
    hr = SharedResource10->GetSharedHandle(&SharedHandle10); //可访问的句柄
    SharedResource10->Release();

    //tutorial13: 新增 为D3D10设备访问D3D11纹理增加互斥信号量(访问共享表面)
    IDXGISurface1 *SharedSurface10;
    hr = D3d10Device->OpenSharedResource(SharedHandle10,
        __uuidof(IDXGISurface1), (void **)&SharedSurface10);
    hr = SharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&KeyedMutexD3d10);

    //tutorial13: 新增 通过D2DFactory创建D2D资源
    ID2D1Factory *D2dFactory;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void **)&D2dFactory);

    /*
    tutorial13: 新增
        创建D2D渲染目标属性
        硬件渲染 + 像素格式: 默认的未知格式+不透明
    */
    D2D1_RENDER_TARGET_PROPERTIES D2dRenderTargetProperties;
    ZeroMemory(&D2dRenderTargetProperties, sizeof(D2dRenderTargetProperties));
    D2dRenderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
    D2dRenderTargetProperties.pixelFormat = D2D1::PixelFormat(
        DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

    /*
    tutorial13: 新增
        创建DXGI渲染目标属性, 使得D2D与D3D10设备能够访问
    */
    hr = D2dFactory->CreateDxgiSurfaceRenderTarget(
        SharedSurface10, &D2dRenderTargetProperties, &D2dRenderTarget);
    SharedSurface10->Release();
    D2dFactory->Release();

    //tutorial13: 新增 创建笔刷
    hr = D2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &ColorBrush);

    /*
    tutorial13: 新增
        初始化DWrite: 创建DW Factory 共享格式
    */
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown **>(&DWriteFactory));

    //tutorial13: 新增 字体格式
    hr = DWriteFactory->CreateTextFormat(
        L"Script", NULL,
        DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        44.0f, L"en-us", &TextFormat);

    //tutorial13: 新增 字体排列设置
    hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);        //水平方向左对齐
    hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR); //竖直方向顶端对齐

    //tutorial13: 新增 防止Debug报警告D3D10设备图元设置
    D3d10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    return true;
}

/*
tutorial13:
    新增D2D 纹理初始化
    创建正方形覆盖场景
    存储索引缓存与顶点缓存
    通过D2D与D3D10的共享纹理创建着色器资源视图(shader resource view)
    通过着色器资源视图覆盖创建的正方形, 通过混合使得字体可见
*/
void InitialD2DScreenTexture()
{
    //tutorial13: 新增 覆盖场景的正方形的顶点与索引创建
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

    //tutorial13: 新增 通过共享纹理创建着色器资源视图
    D3d11Device->CreateShaderResourceView(SharedTexture11, NULL, &D2dTexture);
}

//tutorial13: 新增 渲染文字
void RenderText(std::wstring text, int inInt)
{
    //tutorial13: 新增 D3D11设备先释放共享纹理使得D3D10设备能够接触(互斥)
    KeyedMutexD3d11->ReleaseSync(0);
    KeyedMutexD3d10->AcquireSync(0, 5); //至少接触5ms
    //tutorial13: 新增 D3D10设备接触后, D2D目标开始渲染并清空背景, alpha=0全透
    D2dRenderTarget->BeginDraw();
    D2dRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    //tutorial13: 新增 用于输出到屏幕上的字符串
    std::wostringstream printString;
    //tutorial14: 修改 新增FPS值的显示
    printString << text << inInt;
    printText = printString.str();
    //tutorial13: 新增 字体颜色, 并赋予给笔刷
    D2D1_COLOR_F fontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
    ColorBrush->SetColor(fontColor);
    //tutorial13: 新增 创建一个矩形文字框: 前两个为用户(x,y)起始位置, 后两个为长宽大小
    D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, WIDTH, HEIGHT);
    //tutorial13: 新增 绘制文字后, 结束绘制
    D2dRenderTarget->DrawTextW(
        printText.c_str(), wcslen(printText.c_str()), TextFormat, layoutRect, ColorBrush);
    D2dRenderTarget->EndDraw();
    //tutorial13: 新增 D3D10设备用完后挂起 D3D11唤醒
    KeyedMutexD3d10->ReleaseSync(1);
    KeyedMutexD3d11->AcquireSync(1, 5);
    //tutorial13: 新增 启用混合状态用于绘制透明纹理进行覆盖
    D3d11DeviceContent->OMSetBlendState(Transparency, NULL, 0xffffffff);
    //tutorial13: 新增 D2D的索引缓存与IA绑定
    D3d11DeviceContent->IASetIndexBuffer(D2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    //tutorial13: 新增 D2D顶点缓存绑定IA
    UINT stride = sizeof(Vertex); //顶点大小
    UINT offset = 0;              //偏移量
    D3d11DeviceContent->IAGetVertexBuffers(0, 1, &D2dVertexBuffer, &stride, &offset);
    //tutorial13: 新增 更改WVP矩阵
    WVP = XMMatrixIdentity(); //重置矩阵
    cbPerObj.WVP = XMMatrixTranspose(WVP);
    D3d11DeviceContent->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    D3d11DeviceContent->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
    //tutorial13: 新增 设置共享的D2D资源与采样率
    D3d11DeviceContent->PSSetShaderResources(0, 1, &D2dTexture);
    D3d11DeviceContent->PSGetSamplers(0, 1, &CubeTextureSampleState);
    //tutorial13: 新增 渲染正方形
    D3d11DeviceContent->RSSetState(CWcullMode); //顺时针渲染, 剔除背面
    D3d11DeviceContent->DrawIndexed(6, 0, 0);
}

//tutorial14: 新增 启动计时器
void StartTimer()
{
    LARGE_INTEGER frequencyCount;
    QueryPerformanceFrequency(&frequencyCount);

    CountsPerSecond = double(frequencyCount.QuadPart);

    QueryPerformanceCounter(&frequencyCount);
    CounterStart = frequencyCount.QuadPart;
}

//tutorial14: 新增 得到计时器开始后经过的时间
double GetTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return double(currentTime.QuadPart - CounterStart) / CountsPerSecond;
}

//tutorial14: 新增 计算每帧时间
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
/* ** 函数实现 ** */
