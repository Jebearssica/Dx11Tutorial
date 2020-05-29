# API-Note

记录tutorial中使用的新的函数与数据结构

更多DirectX的API说明可查阅微软官网
<https://docs.microsoft.com/zh-cn/windows/win32/api/d3d11/>

## Tutorial1: win32 API

### 函数与类

更多win32 API接口可查阅手册<http://www.yfvb.com/help/win32sdk/webhelpcontents.htm>

* function InitializeWindow()
  * hInstance: 句柄
  * ShowWnd: 控制如何显示窗口的命令
  * Width,Height: 长宽
  * windowed: 是否窗口化(全屏化)
* class WNDCLASS
  * cbSize: 类长度
  * style: 窗口类型(以CS开头),举几个例子
    * CS_CLASSDC: 一个OK按钮
    * CS_DBLCLKS: 当用户在属于该类的窗口中的光标位于用户双击鼠标时,指示Windows向窗口过程发送双击消息
    * CS_HREDRAW: 如果横向调整,则整个窗口将被重绘
    * CS_VREDRAW: 如果纵向调整,则整个窗口将被重绘
    * CS_NOCLOSE: 无关闭键
    * CS_PARENTDC: 使得子窗口在父窗口上绘制
  * lpfnWndProc: 指向窗口运行函数的指针
  * cbClsExtra: WNDCLASSEX之后分配的额外字节
  * cbWndExtra: 整个窗口实体后分配的额外字节
  * hInstance: 当前窗口的句柄
  * 后面几个看名字就知道了
* function CreateWindowEx()
  * 突然不想写了,以后自己查吧,或者以后闲着没事自己补充

## Tutorial2: Initializing Direct3D 11

### BackBuffer: DXGI_MODE_DESC

* RefreshRate: 刷新率
* Format: 显示格式
* ScanlineOrdering: 描述光栅化器将渲染到表面上的方式,由于使用双缓冲,因此渲染顺序不重要
* Scaling: 拉伸图像以适应监视器的分辨率
  * DXGI_MODE_SCALING_UNSPECIFIED: 不确定模式
  * DXGI_MODE_SCALING_CENTERED: 保持中央嵌入模式
  * DXGI_MODE_SCALING_STRETCHED: 自适应模式

```c++
typedef struct DXGI_MODE_DESC {
  UINT                     Width;
  UINT                     Height;
  DXGI_RATIONAL            RefreshRate;
  DXGI_FORMAT              Format;
  DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
  DXGI_MODE_SCALING        Scaling;
} DXGI_MODE_DESC, *LPDXGI_MODE_DESC;
```

### SwapChain: DXGI_SWAP_CHAIN_DESC

* BufferDesc: 后置缓存
* SampleDesc: 多采样抗锯齿
* BufferUsage: CPU对后置缓存的访问
* BufferCount: 后置缓存数
* OutputWindow: 窗口句柄
* SwapEffect: 控制显示控件与缓存如何交换,使得效率最大
* Flags: 描述交换链行为的额外标志

```c++
typedef struct DXGI_SWAP_CHAIN_DESC {
  DXGI_MODE_DESC   BufferDesc;
  DXGI_SAMPLE_DESC SampleDesc;
  DXGI_USAGE       BufferUsage;
  UINT             BufferCount;
  HWND             OutputWindow;
  BOOL             Windowed;
  DXGI_SWAP_EFFECT SwapEffect;
  UINT             Flags;
} DXGI_SWAP_CHAIN_DESC;
```

### D3D11CreateDeviceAndSwapChain()

创建d3d设备,设备上下文,交换链

* pAdapter: 视频适配器指针
* DriverType: 描述d3d如何被驱动
* D3D_DRIVER_TYPE_HARDWARE: GPU驱动
* Software: 指向软件光栅化的dll文件句柄
* pFeatureLevels: 指针, Dx编译特色等等
* FeatureLevels: 上述指针数量
* ppSwapChain: IDXGISwapChain接口指针
* pFeatureLevel: 保持可用的最高功能级别
* ppImmediateContext: 设备上下文将用于设备的呈现方法,以支持多线程并提高性能

```c++
HRESULT D3D11CreateDeviceAndSwapChain(
  __in   IDXGIAdapter *pAdapter,
  __in   D3D_DRIVER_TYPE DriverType,
  __in   HMODULE Software,
  __in   UINT Flags,
  __in   const D3D_FEATURE_LEVEL *pFeatureLevels,
  __in   UINT FeatureLevels,
  __in   UINT SDKVersion,
  __in   const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
  __out  IDXGISwapChain **ppSwapChain,
  __out  ID3D11Device **ppDevice,
  __out  D3D_FEATURE_LEVEL *pFeatureLevel,
  __out  ID3D11DeviceContext **ppImmediateContext
);
```

### GetBuffer()

创建我们的后置缓存, 用于创建渲染目标视图

* Buffer: 只访问第一个缓存, 为0
* riid: 更改后置缓存的接口类型的参考ID
* ppSurface: 指向即将渲染的表面

```c++
HRESULT GetBuffer(
  [in]       UINT Buffer,
  [in]       REFIID riid,
  [in, out]  void **ppSurface
);
```

### CreateRenderTargetView()

创建渲染目标视图

* pResource: 后置缓存做为资源指针
* pDesc: 设为NULL使得视图能访问minimap level 0的子资源(?)
* ppRTView: 渲染目标视图的指针

```c++
HRESULT CreateRenderTargetView(
  [in]   ID3D11Resource *pResource,
  [in]   const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
  [out]  ID3D11RenderTargetView **ppRTView
);
```

### OMSetRenderTargets()

绑定渲染目标视图与管道输出合并, 同时也绑定我们的深度/模板缓冲区

* NumViews: 绑定的渲染目标数量
* ppRenderTargetViews: 指向渲染目标视图的指针
* pDepthStencilView: 指向深度/模板缓冲区的指针

```c++
void OMSetRenderTargets(
  [in]  UINT NumViews,
  [in]  ID3D11RenderTargetView *const **ppRenderTargetViews,
  [in]  ID3D11DepthStencilView *pDepthStencilView
);
```

## Tutorial3: Begin Drawing

### D3D11_INPUT_ELEMENT_DESC

* SemanticName: 与元素关联的字符串,此字符串将用于将顶点结构中的元素映射到顶点着色器中的元素
* SemanticIndex: 上一个的索引, 如"POSITION0"与"POSITION1"
* Format: 顶点组件格式
* InputSlot: 输入槽, 可单线进入也可多线进入
* AlignedByteOffset: 描述的元素的字节偏移量(?没看懂)
  * 解释: 如下所示, 第一组占用32\*3 bits, 即4\*3 bytes, 因此第二组起始位置为0+12, 即偏移量为12
    * 其中DXGI_FORMAT_R32G32B32_FLOAT, 相当于一个三元组格式, 三元组中每个元素占用32bits

```c++
D3D11_INPUT_ELEMENT_DESC layout[] =
{
  {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
  {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},//偏移量
};
```

* InstanceDataStepRate: 实例化

```c++
typedef struct D3D11_INPUT_ELEMENT_DESC
{
   LPCSTR                         SemanticName;
   UINT                         SemanticIndex;
   DXGI_FORMAT                     Format;
   UINT                         InputSlot;
   UINT                         AlignedByteOffset;
   D3D11_INPUT_CLASSIFICATION     InputSlotClass;
   UINT                         InstanceDataStepRate;
}     D3D11_INPUT_ELEMENT_DESC;
```

### D3DX11CompileFromFile()

用于通过效果文件编译生成着色器

* pSrcFile: 着色器所在的文件名
* pDefines: 指向宏数组的指针
* pInclude: 如果着色器通过#include添加, 则非NULL, 否则是NULL
* pFunctionName: 着色器函数名称
* pProfile: 着色器版本
* Flags1: 编译标志
* Flags2: 效果标志
* pPump: 与多线程相关
* ppShader: 包含着色器和有关着色器信息的缓冲区
* ppErrorMsgs: 错误信息
* pHResult: 返回值, 可以通过 HRESULT hr = D3DX11CompileFromFile(...); 或D3DX11CompileFromFile(...,&hr)两种方式实现

```c++
HRESULT WINAPI D3DX11CompileFromFile(
            LPCSTR pSrcFile,
            CONST D3D10_SHADER_MACRO* pDefines,
            LPD3D10INCLUDE pInclude
            LPCSTR pFunctionName,
            LPCSTR pProfile,
            UINT Flags1,
            UINT Flags2,
            ID3DX11ThreadPump* pPump,
            ID3D10Blob** ppShader,
            ID3D10Blob** ppErrorMsgs,
            HRESULT* pHResult);
```

### CreateVertexShader()与CreatePixelShader()

* pShaderBytecode: 着色器缓存起点
* BytecodeLength: 缓存长度
* pClassLinkage: 类链接接口
* ppVertexShader ppPixelShader: 返回的着色器

```c++
HRESULT CreateVertexShader(
  [in]        const void *pShaderBytecode,
  [in]        SIZE_T BytecodeLength,
  [in]        ID3D11ClassLinkage *pClassLinkage,
  [in, out]   ID3D11VertexShader **ppVertexShader) = 0;
);

HRESULT CreatePixelShader(
  [in]        const void *pShaderBytecode,
  [in]        SIZE_T BytecodeLength,
  [in]        ID3D11ClassLinkage *pClassLinkage,
  [in, out]   ID3D11PixelShader **ppPixelShader) = 0;
);
```

### VSSetShader()与PSSetShader()

* ppClassInstances: 实例化
* NumClassInstances: 上面的数组数量

```c++
void  VSSetShader(
  [in]   ID3D11VertexShader *pVertexShader,
  [in]   (NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
  [in]   UINT NumClassInstances);
);

void PSSetShader(
  [in]   ID3D11PixelShader *pPixelShader,
  [in]   (NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
  [in]   UINT NumClassInstances);
);
```

### D3D11_BUFFER_DESC

好像之前看到过, 懒得写了

```c++
typedef struct D3D11_BUFFER_DESC
{
   UINT             ByteWidth;
   D3D11_USAGE         Usage;
   UINT             BindFlags;
   UINT             CPUAccessFlags;
   UINT             MiscFlags;
   UINT             StructureByteStride;
}    D3D11_BUFFER_DESC;
```

### D3D11_SUBRESOURCE_DATA

* pSysMem: 放入缓存的数据
* SysMemPitch: 以字节为单位的从纹理中的一行到下一行的距离, 用于2D和3D的纹理
* SysMemSlicePitch: 在3D纹理中. 从一个深度级别到下一个深度级别的距离

```c++
typedef struct D3D11_SUBRESOURCE_DATA
{
   const    void *pSysMem;
   UINT     SysMemPitch;
   UINT     SysMemSlicePitch;
}     D3D11_SUBRESOURCE_DATA;
```

### CreateBuffer()

用于创建缓存

* pDesc: 指向缓存描述
* pInitialData: 指向一个子资源数据结构的指针. 该结构包含我们要放在这里的数据, 如果之后再添加数据则设为null
* ppBuffer: 返回的缓存

```c++
HRESULT CreateBuffer(
   [in]    const D3D11_BUFFER_DESC *pDesc,
   [in]    const D3D11_SUBRESOURCE_DATA *pInitialData,
   [in]    ID3D11Buffer **ppBuffer
);
```

### IASetVertexBuffers()

顶点缓存与IA绑定

* StartSlot: 与输入槽(input slot)绑定
* NumBuffers: 绑定的缓存数量
* ppVertexBuffers: 指向顶点缓存
* pStrides: 每个顶点的大小
* pOffsets: 起始位置的缓冲区开始的字节偏移量(?)

```c++
void IASetVertexBuffers(
   [in]   UINT StartSlot,
   [in]   UINT NumBuffers,
   [in]   ID3D11Buffer *const *ppVertexBuffers,
   [in]   const UINT *pStrides,
   [in]   const UINT *pOffsets
);
```

### CreateInputLayout()

创建输入布局

* pInputElementDescs: 包含顶点布局的数组
* NumElements: 顶点布局元素数量
* pShaderBytecodeWithInputSignature: 指向顶点着色器的指针
* BytecodeLength: 顶点着色器大小
* ppInputLayout: 返回输入布局

```c++
HRESULT CreateInputLayout(
   [in]   const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
   [in]   UINT NumElements,
   [in]   const void *pShaderBytecodeWithInputSignature,
   [in]   SIZE_T BytecodeLength,
   [out]  ID3D11InputLayout **ppInputLayout
);
```

### IASetInputLayout()

输入布局绑定设置(应该是设置与IA绑定)

* pInputLayout: 输入布局

```c++
void STDMETHODCALLTYPE IASetInputLayout(
   [in]   ID3D11InputLayout *pInputLayout
);
```

### IASetPrimitiveTopology()

设置图元拓扑, 向IA传输需要传递的图元类型

* D3D10_PRIMITIVE_TOPOLOGY_POINTLIST: 独立点集
* D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP: 线条(一条连续线条)
* D3D10_PRIMITIVE_TOPOLOGY_LINELIST: 线条集(多个线条)
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: 三角形带(每个三角形通过共用顶点链接(可共用多个顶点))
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST: 三角形集(相互分离)
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ: 几何着色器使用

### RSSetViewports()

设置视图, 将其绑定到光栅着色器, 第一个参数为绑定的视图数量, 第二个为视图组的指针

### ID3D11DeviceContext::Draw()

第一个参数是要绘制的顶点数. 第二个参数是要开始绘制的顶点数组开始的偏移量。

## Tutorial4: Color!

无

## Tutorial5: Indices

### D3D11DeviceContext::DrawIndexed()

通过索引缓存绘制

* IndexCount: 绘制的索引个数
* StartIndexLocation: 起始点索引缓冲的偏移量
* BaseVertexLocation: StartIndexLocation的偏移量
  * 即m_pImmediateContext->DrawIndexed(3, 3, 0) 画234
  * m_pImmediateContext->DrawIndexed(3, 0, 3) 画345(0+3,1+3,2+3)
  * 应用范围: 很难距离, 以后补充(?)

```c++
void DrawIndexed(
  UINT IndexCount,
  UINT StartIndexLocation,
  INT  BaseVertexLocation
);
```

## Tutorial6: Depth

### ID3D11Device::CreateDepthStencilView()

* pResource: 深度模板缓存描述
* pDesc: 深度模具视图描述
* ppDepthStencilView: 返回的深度模板缓存

```c++
HRESULT CreateDepthStencilView(
  ID3D11Resource                      *pResource,
  const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
  ID3D11DepthStencilView              **ppDepthStencilView
);
```

### ID3D11DeviceContext::ClearDepthStencilView()

* pDepthStencilView: 我们要清除的深度/模板视图
* ClearFlags: 确定要清除的数据类型
* Depth: 深度值
  * 如果我们在此处设置0.0f, 则不会在屏幕上绘制任何东西
  * 如果我们在此处设置1.0f, 则确保在屏幕上绘制所有对象
* Stencil: 我们将模板设置为的值

```c++
void ClearDepthStencilView(
  ID3D11DepthStencilView *pDepthStencilView,
  UINT                   ClearFlags,
  FLOAT                  Depth,
  UINT8                  Stencil
);
```

## Tutorial7: World View and Local Spaces (static Camera)

### XMMatrixPerspectiveFovLH()

创建投影空间

* FovAngleY: FOV沿y轴的弧度(中心垂直弧度)
* AspectRatio: 长宽比
* NearZ: 图示5的近z面距离
* FarZ: 图示5的远z面距离

```c++
XMMATRIX XMMatrixPerspectiveFovLH
(
    FLOAT FovAngleY,
    FLOAT AspectRatio,
    FLOAT NearZ,
    FLOAT FarZ
)
```

注意:

* 对象离近z面近或者离远z面远, 都不会渲染物体

### XMMatrixIdentity()

返回一个空矩阵

## Tutorial8: Transformations

### XMMatrixScaling()

返回一个缩放矩阵, 用于缩放物体

```c++
XMMATRIX XMMatrixScaling
(
    FLOAT ScaleX,    // x=axis scale
    FLOAT ScaleY,     // y-axis scale
    FLOAT ScaleZ    // z-axis scale
)
```

## Tutorial9: Render States

### D3D11_RASTERIZER_DESC

* FillMode:
  * 用于线框渲染的D3D11_FILL_WIREFRAME
  * 用于实体渲染的D3D11_FILL_SOLID(默认)
* CullMode:
  * D3D11_CULL_NONE来禁用剔除(不渲染三角形的另一侧)
  * D3D10_CULL_FRONT进行正面剔除(这样就不会渲染正面)
  * D3D11_CULL_BACK剔除背面(默认)
* FrontCounterClockwise: 真意味着如果三角形顶点是顺时针呈现到相机, 那么这是正面
* DepthBias: 指定添加到给定像素的深度值
* DepthBiasClamp: 指定像素的最大深度偏差
* SlopeScaledDepthBias: 指定给定像素斜率上的标量
* DepthClipEnable: 启用或禁用根据与相机的距离的裁剪
* ScissorEnable: 启用或禁用剪刀矩形剔除
  * 剪刀矩形剔除(scissor-rectangle culling): 所有在活动剪刀矩形外的像素都被剔除(?)
  * Todo: scissor-rectangle culling是个啥
* MultisampleEnable: 多样本抗锯齿
* AntialiasedLineEnable: 线性抗锯齿

```c++
typedef struct D3D11_RASTERIZER_DESC {
  D3D11_FILL_MODE FillMode;
  D3D11_CULL_MODE CullMode;
  BOOL            FrontCounterClockwise;
  INT             DepthBias;
  FLOAT           DepthBiasClamp;
  FLOAT           SlopeScaledDepthBias;
  BOOL            DepthClipEnable;
  BOOL            ScissorEnable;
  BOOL            MultisampleEnable;
  BOOL            AntialiasedLineEnable;
} D3D11_RASTERIZER_DESC;
```

### ID3D11Device::CreateRasterizerState()

* pRasterizerDesc: 描述指针
* ppRasterizerState: 返回的ID3D11RasterizerState客体

```c++
HRESULT CreateRasterizerState1(
  const D3D11_RASTERIZER_DESC1 *pRasterizerDesc,
  ID3D11RasterizerState1       **ppRasterizerState
);
```

### ID3D11DeviceContext::RSSetState()

用于光栅状态与RS阶段绑定

## Tutorial10: Textures

### D3DX11CreateShaderResourceViewFromFile()

从文件中加载纹理

* pDevice: 指向我们d3d设备
* pSrcFile: 文件路径
  * 一般与exe在同一个文件夹下, 因此直接是文件名
  * 不在同一个文件夹下, 要给出路径
* pLoadInfo: 纹理如何被加载
* pPump: 仅当我们需要多线程时才使用, 并在加载该文件时让程序继续运行
* ppShaderResourceView: 指向着色器资源视图, 用于保存纹理数据
* pHResult: 返回指针

```c++
HRESULT D3DX11CreateShaderResourceViewFromFile(
  __in   ID3D11Device *pDevice,
  __in   LPCTSTR pSrcFile,
  __in   D3DX11_IMAGE_LOAD_INFO *pLoadInfo,
  __in   ID3DX11ThreadPump *pPump,
  __out  ID3D11ShaderResourceView **ppShaderResourceView,
  __out  HRESULT *pHResult
);
```

### D3D11_SAMPLER_DESC

采样器状态描述

* Filter: 筛选方法
* AddressU AddressV AddressW: uvw坐标
* MipLODBias: mipmap level的偏移量
  * 如计算出的level是3, 偏移量为2, 实际mipmap level为5
* MaxAnisotropy: D3D11_FILTER_ANISOTROPIC或D3D11_FILTER_COMPARISON_ANISOTROPIC则使用钳位值
  * 钳位: 是指将某点的电位限制在规定电位的措施
* ComparisonFunc: 这会将采样的mipmap数据与此纹理的另一个mipmap采样数据进行比较
* BorderColor: 如果对uwv任意一个设置了D3D11_TEXTURE_ADDRESS_BORDER, 那么这个就是贴图与边缘之间片面的颜色
* MinLOD: mipmap level最低值
* MaxLOD: mipmap level最高值

```c++
typedef struct D3D11_SAMPLER_DESC {
  D3D11_FILTER               Filter;
  D3D11_TEXTURE_ADDRESS_MODE AddressU;
  D3D11_TEXTURE_ADDRESS_MODE AddressV;
  D3D11_TEXTURE_ADDRESS_MODE AddressW;
  FLOAT                      MipLODBias;
  UINT                       MaxAnisotropy;
  D3D11_COMPARISON_FUNC      ComparisonFunc;
  FLOAT                      BorderColor[4];
  FLOAT                      MinLOD;
  FLOAT                      MaxLOD;
} D3D11_SAMPLER_DESC;
//default默认值
Filter            MIN_MAG_MIP_LINEAR
AddressU        Clamp
AddressV        Clamp
AddressW        Clamp
MinLOD            -3.402823466e+38F (-FLT_MAX)
MaxLOD            3.402823466e+38F (FLT_MAX)
MipMapLODBias    0.0f
MaxAnisotropy    16
ComparisonFunc    Never
BorderColor        float4(0.0f,0.0f,0.0f,0.0f)
```

## Tutorial11: Blending

### D3D11_BLEND_DESC

* AlphaToCoverageEnable: 是否使用透明覆盖作为多采样技术
* IndependentBlendEnable: false则只和下列RenderTarget[0]混合
* RenderTarget: 最多有8个混合对象

```c++
typedef struct D3D11_BLEND_DESC {
  BOOL                           AlphaToCoverageEnable;
  BOOL                           IndependentBlendEnable;
  D3D11_RENDER_TARGET_BLEND_DESC RenderTarget[8];
} D3D11_BLEND_DESC;
```

### D3D11_RENDER_TARGET_BLEND_DESC

* BlendEnable: 此次渲染目标是否进行混合
* SrcBlend: 原混合因子(SBF)
* DestBlend: 目标混合因子(DBF)
* BlendOp: 指定要使用的混合操作
* SrcBlendAlpha: 针对透明度的原混合因子(SBF)
* DestBlendAlpha: 针对透明度的目标混合因子(SBF)
* BlendOpAlpha: 用于alpha通道的混合操作
* RenderTargetWriteMask: 我们指定要混合的通道,RGBA, 或者他们的组合, 类型如下

```c++
typedef struct D3D11_RENDER_TARGET_BLEND_DESC {
  BOOL           BlendEnable;
  D3D11_BLEND    SrcBlend;
  D3D11_BLEND    DestBlend;
  D3D11_BLEND_OP BlendOp;
  D3D11_BLEND    SrcBlendAlpha;
  D3D11_BLEND    DestBlendAlpha;
  D3D11_BLEND_OP BlendOpAlpha;
  UINT8          RenderTargetWriteMask;
} D3D11_RENDER_TARGET_BLEND_DESC;
//RenderTargetWriteMask类型
typedef enum D3D11_COLOR_WRITE_ENABLE {
  D3D11_COLOR_WRITE_ENABLE_RED     = 1,
  D3D11_COLOR_WRITE_ENABLE_GREEN   = 2,
  D3D11_COLOR_WRITE_ENABLE_BLUE    = 4,
  D3D11_COLOR_WRITE_ENABLE_ALPHA   = 8,
  D3D11_COLOR_WRITE_ENABLE_ALL     =
      ( D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN |  
        D3D11_COLOR_WRITE_ENABLE_BLUE | D3D11_COLOR_WRITE_ENABLE_ALPHA )
} D3D11_COLOR_WRITE_ENABLE;
```

## Tutorial12: Pixel Clipping、

无

## Tutorial13: Simple Font

### D2D1_RENDER_TARGET_PROPERTIES

* type: 硬渲染或软渲染选择
* pixelFormat: 像素格式
* dpiX: X轴方向上的DPI
* dpiY: Y轴方向上的DPI
* usage: msdn远程指定渲染目标
* minLevel: 设置硬件渲染最低等级
  * 低于这个等级使用软件渲染, 或者硬件渲染出问题, 则使用软件渲染
  * 本次渲染不设置, 因为创建DXGI渲染目标

```c++
struct D2D1_RENDER_TARGET_PROPERTIES {
  D2D1_RENDER_TARGET_TYPE  type;
  D2D1_PIXEL_FORMAT        pixelFormat;
  FLOAT                    dpiX;
  FLOAT                    dpiY;
  D2D1_RENDER_TARGET_USAGE usage;
  D2D1_FEATURE_LEVEL       minLevel;
};
```

### CreateTextFormat()

* fontFamilyName: 字体系列的字符串, 本次使用"Script"
* fontCollection: 指向字体集合对象的指针, 从中获取字体, 使用null使用系统字体
* fontWeight: 值越大越粗体
* fontStyle: DWRITE_FONT_STYLE枚举类型(字体类型)
* fontStretch: 字符的宽度(0~9)
* fontSize: 字体大小(与设备无关, 相当于word里的字号大小)
* localeName: 指定字体语言
* textFormat: 返回的一个存储字体格式的IDWriteTextFormat对象

```c++
HRESULT CreateTextFormat(
  [in]   const WCHAR * fontFamilyName,
         IDWriteFontCollection * fontCollection,
         DWRITE_FONT_WEIGHT  fontWeight,
         DWRITE_FONT_STYLE  fontStyle,
         DWRITE_FONT_STRETCH  fontStretch,
         FLOAT  fontSize,
  [in]   const WCHAR * localeName,
  [out]  IDWriteTextFormat ** textFormat
)
```

### DrawText()

用于绘制文字

* string: 绘制的文字
* stringLength: 长度
* textFormat: 格式
* layoutRect: 文字框
* defaultForegroundBrush: 笔刷
* options: 文本是否对齐像素 超出文字框是否裁切
* measuringMode:

TODO: measuringMode究竟是个啥 glyph metrics进行文字测量有啥用

```c++
void DrawText(
  [in]   WCHAR *string,
         UINT stringLength,
  [in]   IDWriteTextFormat *textFormat,
  [ref]  const D2D1_RECT_F &layoutRect,
  [in]   ID2D1Brush *defaultForegroundBrush,
         D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE,
         DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL
);
```

## Tutorial14: High Resolution Timer

### QueryPerformanceCounter()

用于时间间隔测量

```c++
BOOL QueryPerformanceCounter(
  LARGE_INTEGER *lpPerformanceCount
);
```

### QueryPerformanceFrequency()

用于应用的频率查询

```c++
BOOL QueryPerformanceFrequency(
  LARGE_INTEGER *lpFrequency
);
```
