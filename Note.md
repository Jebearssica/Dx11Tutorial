# 笔记

## Tutorial1: win32 API

### 新知识

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

### 其他问题

* 使用VS自带的GitHub扩展进行版本控制

详情可参见链接: <https://zhuanlan.zhihu.com/p/31031838>

* 在已经创建的项目中可以使用扩展来在GitHub上创建新的仓库也可以在创建项目的开始就与远端同步

* VS对应的切换到下一行空行的快捷键是ctrl+shift+enter,与VS Code中的ctrl+enter功能一样

### 编译问题

* 关于长字符串与LPCTSTR类型不符

1. 通过改变项目属性中的字符集-使用多字节字符集
2. 或者在每个长字符串前加上L, 示例如下

```c++
LPCTSTR WndClassName = L"firstwindow";
```

* 调试的过程中应该注意CALLBACK回调函数的运行时刻,并不是按照线性步进运行的

类似于单片机的**事件驱动模型**("event-driven" programming model)

## Tutorial2: Initializing Direct3D 11

### 新知识

* 双缓冲: 为了解决窗体进行复杂图像处理后,屏幕在重绘时由于过频的刷新引起闪烁的问题
  * 内存上创建一个与显示控件相同大小的画布,在画布上完成绘制后使用画布覆盖显示控件以达到显示一次则刷新一次
* 三缓冲: 双缓冲存在显示控件等待后台屏幕绘制的过程,三缓冲(以及更高的多重缓冲)解决这个问题,做到更低时延
  * 两个后置缓存(backBuffer)后台绘制,显示控件读取前置缓存,前置缓存读取后置缓存,由于有两个后置缓存,所以前置缓存无需等待
  * 第二种方法:三个缓存相当于三个交换链,以队列的方式(先进先出)依次绘制-被读取
* rgba色彩管理: 比rgb多一个a, 代表Alpha, 控制透明度
* __uuidof: 用来获取 某种结构、接口及其指针、引用、变量 所关联的GUID,类袭似于某些语言百中获取类型 typeof 这样的操作。
* SwapChain: 交换链技术,与三重缓冲的对比详见<https://zhuanlan.zhihu.com/p/104244526>
* 各个图形厂商都有自己对渲染的优化, 要具体情况具体写
* 直接在程序中编写错误抛出有助于快速定位error, 也能解决一些意想不到的bug
  * 通过查看HRESULT(句柄返回结果), 可以判断函数运行情况
    * S_OK: 运行成功
    * E_NOTIMPL: 函数未实现
    * E_NOINTERFACE: 接口不支持
    * E_ABORT: 中止
    * E_FAIL: 报错
    * E_INVALIDARG: 一个或多个参数无效
    * DXGetErrorDescription(): 可得到详细报错信息, 需要DXErr.h

### 函数与类

* 有关BackBuffer: DXGI_MODE_DESC
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

* 有关SwapChain: DXGI_SWAP_CHAIN_DESC
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

* 有关D3D11CreateDeviceAndSwapChain(): 创建d3d设备,设备上下文,交换链
  * pAdapter: 视频适配器指针
  * DriverType: 描述d3d如何被驱动
    * D3D_DRIVER_TYPE_HARDWARE: GPU驱动
  * Software: 指向软件光栅化的dll文件句柄
  * pFeatureLevels: 指针Dx版本
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

* GetBuffer(): 创建我们的后置缓存, 用于创建渲染目标视图。
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

* CreateRenderTargetView(): 创建渲染目标视图
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

* OMSetRenderTargets(): 绑定渲染目标视图与管道输出合并,同时也绑定我们的深度/模板缓冲区
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

### 其他问题

* LNK2019无法解析的外部符号

#pragma comment ( lib, "D3D11.lib"), 链接对应的库即可

* 无法解析的外部符号___vsnprintf

出现这个问题的原因是vs2017默认编译时将许多标准库采用内联方式处理,因而没有可以链接的标准库文件,所以要专门添加标准库文件来链接标准库中的函数。

根据链接添加库文件legacy_stdio_definitions.lib
<https://jingyan.baidu.com/article/48206aeab8516f216ad6b38c.html>

## Tutorial3: Begin Drawing

### 新知识

* 渲染管线: 负责将3D摄像机(相当于人眼)看到的3D场景转换成一张2D图片显示在屏幕上.
  * 类似于流水线进程, 一个阶段的输出就是下一个阶段的输入
* dx11中管线的进程: 其中, 椭圆进程表示由用户创建, 矩形进程可通过direct3d 11 device context修改设定
* ZeroMemory(), 将一段内存通过零填充, 相当于初始化指针(比给指针一个null要牛逼一点?)
  * Todo: 比较一下

```c++
D3D11_BUFFER_DESC vertexBufferDesc;
ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );
```

![十个管线流程][1]

#### IA:Input Assembler

读取几何数据、顶点和索引。然后,它使用数据创建几何图元,如traingles、square、lines和points,这些图元将被其他阶段输入和使用。

* 传入数据之前, 需要设置缓冲区并设置基本拓扑、输入布局和活动缓冲区。
    * 基本拓扑(图元拓扑?)(Primitive Topology): 顶点存储着物体的诸多信息(比如位置, 法线, 颜色等等), 将他们按照一定的规则组织便可以形成图形(这个规则被称为Primitive Topology).
      * <https://blog.csdn.net/u010333737/article/details/78556583>
* 先创建缓存: IA使用的两个缓冲区是顶点缓冲区和索引缓冲区
* 再创建输入布局对象: 告诉Direct3d我们的文本结构由什么组成,以及如何处理我们的顶点结构中的每个组件
  * 通过ID3D11Device::CreateInputLayout()实例化
* 创建一个顶点缓存
  * 通过ID3D11Device::CreateBuffer()实例化
* 绑定布局说明和顶点缓存
  * ID3D11DeviceContext::IASetVertexBuffers()与IASetInputLayout()
* 设置基本拓扑,以便IA知道如何使用顶点并制作诸如三角形或直线之类的基本元素。
  * 即, 输入六个顶点究竟是三条线, 还是两个独立三角形
  * ID3D11DeviceContext::IASetPrimitiveTopology()
* 管线准备就绪,通过ID3D11DeviceContext::Draw()传输图元

```c++
//The vertex Structure, 假设你的顶点结构中有位置、颜色两个元素
struct Vertex
{
    D3DXVECTOR3 pos;
    D3DXCOLOR   color;
};


//The input-layout description, 你的输入布局对象将会有两个元素分别对应位置与颜色
D3D11_INPUT_ELEMENT_DESC layout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
```

#### VS:Vertex Shader

是可编程的着色器,通过VS可以执行诸如变换,缩放,照明,对纹理进行位移贴图之类的操作。

即使顶点没有变化, 也必须实现顶点着色器使得管道正常工作

管道中的顶点着色器通过类C++语言HLSL实现, 输入一个顶点返回一个顶点

#### HS: Hull Shader

* 外壳着色器阶段(HS)，细化器阶段(TS)和域着色器阶段(DS)这三个新阶段共同协作以实现称为镶嵌(tesselation)的东西。
  * 作用是将一个图元（例如三角形或直线）分割成许多较小的部分，从而以极快的速度增加模型的细节。
  * 在显示之前, 先将图元在GPU中创建, 然后直接映射到屏幕
    * 作用: 降低了在CPU 内存中创建的时间

#### TS: Tessellator 

从HS中获取数据, 对图元进行分割, 再将数据传给DS

#### DS: Domain Shader

可编程着色器, 从HS获取顶点位置, 并接受TS的数据从而添加细节

如果仅仅是直接接受顶点位置, 那么并不会产生更多细节, 因为在三角形或直线的中心添加更多顶点不会增加任何细节

#### GS: Geometry Shader

在顶点和片段着色器之间有一个可选的着色器，叫做几何着色器(Geometry Shader)。几何着色器以一个或多个表示为一个单独基本图形（primitive）的顶点作为输入，比如可以是一个点或者三角形。几何着色器在将这些顶点发送到下一个着色阶段之前，可以将这些顶点转变为它认为合适的内容。几何着色器有意思的地方在于它可以把（一个或多个）顶点转变为完全不同的基本图形（primitive），从而生成比原来多得多的顶点。

#### SO: Stream Output

用于从管线中获取顶点数据，特别是在没有GS的情况下

* 从SO输出的顶点数据始终以列表形式发送至内存, 永远不会发出不完整的图元
  * 不完整的图元: 例如一个只有两个顶点信息的三角形

#### RS: Rasterization Stage

RS阶段获取发送给它的矢量信息（形状和图元），并通过在每个图元之间插入每个顶点的值将它们转换为像素。 它还处理裁剪，基本上是裁剪屏幕视图之外的图元。

上述文字也解释了什么叫渲染(*Rasterization*)

#### PS: Pixel Shader

* 该阶段进行计算并修改将在屏幕上看到的每个像素, 计算每个像素片段的最终颜色
* 与顶点着色器一样, 一一映射
  * 一个像素输入, 输出一个像素
* 像素片段: 是将被绘制到屏幕上的每个潜在像素

#### OM: Output Merger

此阶段获取像素片段和深度/模板缓冲区，并确定实际将哪些像素写入渲染目标

此阶段后, 将后置缓存映射到屏幕上

#### 着色器的设置

d3d是一个状态机, 只能保存当前的状态和设定, 因此对着色器的设置需要在运行时(每次渲染材质之前)设置, 而不仅仅是初始化的时候设置

#### 视图viewport

一个虚拟的窗口, 用来告知光栅化程序哪个部分是需要绘制给用户看到的

#### 偏移量offset

数组的偏移量, 如ID3D11DeviceContext::Draw()中的第二个参数offset
比如开始绘制的顶点组v, 从第三个顶点开始绘制, 偏移量为2

#### 效果文件(effect Files)

### 函数与类

#### D3D11_INPUT_ELEMENT_DESC

* SemanticName: 与元素关联的字符串,此字符串将用于将顶点结构中的元素映射到顶点着色器中的元素
* SemanticIndex: 上一个的索引, 如"POSITION0"与"POSITION1"
* Format: 顶点组件格式
* InputSlot: 输入槽, 可单线进入也可多线进入
* AlignedByteOffset: 描述的元素的字节偏移量(?没看懂)
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

#### D3DX11CompileFromFile()

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
            LPD3D10INCLUDE pInclude,
            LPCSTR pFunctionName, 
            LPCSTR pProfile, 
            UINT Flags1, 
            UINT Flags2, 
            ID3DX11ThreadPump* pPump, 
            ID3D10Blob** ppShader, 
            ID3D10Blob** ppErrorMsgs, 
            HRESULT* pHResult);
```

#### CreateVertexShader()与CreatePixelShader()

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

#### VSSetShader()与PSSetShader()

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

#### D3D11_BUFFER_DESC

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

#### D3D11_SUBRESOURCE_DATA

* pSysMem: 放入缓存的数据
* SysMemPitch: 以字节为单位的从纹理中的一行到下一行的距离, 用于2D和3D的纹理
* SysMemSlicePitch: 在3D纹理中，从一个深度级别到下一个深度级别的距离

```c++
typedef struct D3D11_SUBRESOURCE_DATA
{
   const    void *pSysMem;
   UINT     SysMemPitch;
   UINT     SysMemSlicePitch;
}     D3D11_SUBRESOURCE_DATA;
```

#### CreateBuffer()

用于创建缓存

* pDesc: 指向缓存描述
* pInitialData: 指向一个子资源数据结构的指针，该结构包含我们要放在这里的数据, 如果之后再添加数据则设为null
* ppBuffer: 返回的缓存

```c++
HRESULT CreateBuffer( 
   [in]    const D3D11_BUFFER_DESC *pDesc,
   [in]    const D3D11_SUBRESOURCE_DATA *pInitialData,
   [in]    ID3D11Buffer **ppBuffer
);
```

#### IASetVertexBuffers()

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

#### CreateInputLayout()

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

#### IASetInputLayout()

输入布局绑定设置(应该是设置与IA绑定)

* pInputLayout: 输入布局

```c++
void STDMETHODCALLTYPE IASetInputLayout( 
   [in]   ID3D11InputLayout *pInputLayout
);
```

#### IASetPrimitiveTopology()

设置图元拓扑, 向IA传输需要传递的图元类型

* D3D10_PRIMITIVE_TOPOLOGY_POINTLIST: 独立点集
* D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP: 线条(一条连续线条)
* D3D10_PRIMITIVE_TOPOLOGY_LINELIST: 线条集(多个线条)
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: 三角形带(每个三角形通过共用顶点链接(可共用多个顶点))
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST: 三角形集(相互分离)
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ: 几何着色器使用

#### RSSetViewports()

设置视图, 将其绑定到光栅着色器, 第一个参数为绑定的视图数量, 第二个为视图组的指针

#### ID3D11DeviceContext::Draw()

第一个参数是要绘制的顶点数，第二个参数是要开始绘制的顶点数组开始的偏移量。

### 编译问题

#### LNK1104 无法打开d3dx11.lib

上面的解决平台方案调成x86, 因为项目属性里的库文件没有包含dx的x64库, 新添加这个也可以解决

#### VS_Buffer 是 nullptr导致中断

几个全局变量指针未初始化, 先尝试使用zeromemory(错的)

很显然是D3DX11CompileFromFile()这一步编译未成功, 导致还是空指针

添加.fx特效文件后, 很显然就能够获得一个非空指针

#### VS+fx文件编译:未定义main函数

添加了.fx文件后报错

由于vs不自带.fx文件的模板, 因此使用的是HLSL模板, vs会用HLSL编译器进行编译
属性改为"不参与生成"就行

测试之后.fx运行正确, 显示蓝色三角形, 但是自己写的main有问题, 需要修改

输入布局D3D11_INPUT_ELEMENT_DESC中, 颜色布局为RGB并不是RGBA

<http://blog.sina.com.cn/s/blog_5e83fce60102vd0r.html>

[1]:images/render-pipeline-stages.png
