# 笔记

Todo部分通过GitHub中的issue形式保留, 将会从Note和代码文件中移植Todo

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

更多DirectX的API说明可查阅微软官网
<https://docs.microsoft.com/zh-cn/windows/win32/api/d3d11/>

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

* 外壳着色器阶段(HS). 细化器阶段(TS)和域着色器阶段(DS)这三个新阶段共同协作以实现称为镶嵌(tesselation)的东西。
  * 作用是将一个图元（例如三角形或直线）分割成许多较小的部分. 从而以极快的速度增加模型的细节。
  * 在显示之前, 先将图元在GPU中创建, 然后直接映射到屏幕
    * 作用: 降低了在CPU 内存中创建的时间

#### TS: Tessellator 

从HS中获取数据, 对图元进行分割, 再将数据传给DS

#### DS: Domain Shader

可编程着色器, 从HS获取顶点位置, 并接受TS的数据从而添加细节

如果仅仅是直接接受顶点位置, 那么并不会产生更多细节, 因为在三角形或直线的中心添加更多顶点不会增加任何细节

#### GS: Geometry Shader

在顶点和片段着色器之间有一个可选的着色器. 叫做几何着色器(Geometry Shader)。几何着色器以一个或多个表示为一个单独基本图形（primitive）的顶点作为输入. 比如可以是一个点或者三角形。几何着色器在将这些顶点发送到下一个着色阶段之前. 可以将这些顶点转变为它认为合适的内容。几何着色器有意思的地方在于它可以把（一个或多个）顶点转变为完全不同的基本图形（primitive）. 从而生成比原来多得多的顶点。

#### SO: Stream Output

用于从管线中获取顶点数据. 特别是在没有GS的情况下

* 从SO输出的顶点数据始终以列表形式发送至内存, 永远不会发出不完整的图元
  * 不完整的图元: 例如一个只有两个顶点信息的三角形

#### RS: Rasterization Stage

RS阶段获取发送给它的矢量信息（形状和图元）. 并通过在每个图元之间插入每个顶点的值将它们转换为像素。 它还处理裁剪. 基本上是裁剪屏幕视图之外的图元。

上述文字也解释了什么叫渲染(*Rasterization*)

#### PS: Pixel Shader

* 该阶段进行计算并修改将在屏幕上看到的每个像素, 计算每个像素片段的最终颜色
* 与顶点着色器一样, 一一映射
  * 一个像素输入, 输出一个像素
* 像素片段: 是将被绘制到屏幕上的每个潜在像素

#### OM: Output Merger

此阶段获取像素片段和深度/模板缓冲区. 并确定实际将哪些像素写入渲染目标

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
* SysMemSlicePitch: 在3D纹理中. 从一个深度级别到下一个深度级别的距离

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
* pInitialData: 指向一个子资源数据结构的指针. 该结构包含我们要放在这里的数据, 如果之后再添加数据则设为null
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

第一个参数是要绘制的顶点数. 第二个参数是要开始绘制的顶点数组开始的偏移量。

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

#### 上传GitHub之后merge conflict(合并冲突)

* 进入pr界面, resolve conflict.

```github
<<<<<<<<dev
dev分支的修改
========
>>>>>>>>master
```

很显然, 我自己知道dev的分支是对的, 所以删掉master分支的部分以及注释, 保留内容如下, 之后就可以重新commit合并分支

```github
dev分支的修改
```

* 第二种解决方法 git rebase(变基)

```git
git rebase --skip 跳过冲突, 将分支起始点移动到主干的起始点
```

具体变基过程如下图所示

![产生冲突][2]
![变基][3]
![回到主分支再次合并][4]

## Tutorial4: Color!

### 新知识

* 代码中的注释应该尽可能的加上改动的时间以及原因, 而不仅仅是在文档中写出, 增加可读性(会太复杂吗?)
  * 因此tutorial4以及之后的更新, 代码中均会有注释体现, 没有的就是前三个tutorial的更新

### 编译问题

* 与上一个tutorial的VS_Buffer 是 nullptr同样的问题
  * 编译选项改成x86, 文件夹视图是默认x64编译
  * 项目属性已添加了x64的源文件
* 项目属性已添加了x64的源文件, 报无法打开d3dx11.lib
  * 老老实实x86编译

#### x86与x64编译区别(解决issue)

-Todo[x]: x86与x64编译区别

<https://www.zhihu.com/question/303496588>
<https://www.cnblogs.com/xietianjiao/p/11599117.html>

* 定义:
  * x86: 将程序集编译为由兼容 x86 的 32 位公共语言运行库运行。
  * x64: 将程序集编译为由支持 AMD64 或 EM64T 指令集的计算机上的 64 位公共语言运行库运行。
* 选择不同后的行为(在 64 位 Windows 操作系统上)
  * 用 x86 编译的程序集将在 WOW64 下运行的 32 位 CLR 上执行
  * 用 x64 编译的程序集将在 64 位 CLR 上执行
* 设置正确编译不通过两个原因
  * 不兼容, 你编译的程序没有对应架构的库, 或者有对应的库, 但是用法稍有区别
  * 指针类型错误, 你程序对指针的加减, 或者指针强制转换有型别错误

## Tutorial5: Indices

### 新知识

#### 索引

索引(index)的适用范围:

* 绘制几何图形
* 加载模型

功能: 减少加载重复的顶点, 有些顶点被共用, 会被多次写入缓存, 使用索引缓存可以避免

```c++
//没有索引缓存, 通过两个三角形构成一个长方形
vertex = {v1, v2, v3, v3, v2, v4};
//有索引缓存, 直接实现
vertex = {v1, v2, v3, v4};
index = {1, 2, 3, 3, 2, 4};
```

这就是第一次接触到面片, 绝大部分图形通过三角形进行模拟

#### DWORD

表示unsigned long

### 函数与类

#### D3D11DeviceContext::DrawIndexed()

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

### 编译问题

* 编译完成但是背景黑色不显示正方形
  * D3d11DeviceContent->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    * 编码格式是DXGI_FORMAT_R32_UINT不是DXGI_FORMAT_R32_FLOAT

## Tutorial6: Depth

创建深度/模板缓冲区(depth/stencil)和视图, 并且与OM阶段绑定

### 新知识

#### 深度/模板视图

* 作用: 让管线的OM阶段检查渲染目标上所有像素细分的深度/模板值
  * 如一个球后面有一个正方体, 到达OM阶段时, 将像素片段深度值与该位置中已经存在的像素片段进行比较
    * 如果新的像素片段深度值小于已经存在的像素片段，则将丢弃已经存在的像素片段，并将新的像素片段保留在渲染目标上
    * 如果先渲染球再渲染正方体, 由于球在前面, 深度值更小, 丢弃新的正方体的像素片段
    * 所有几何图形绘制完毕后, 渲染目标剩余的像素就是最终显示到屏幕上的像素

#### XXXXXX_DESC

是一个定义的描述, 就像前面定义后置缓存一样

### 类与函数

#### ID3D11Device::CreateDepthStencilView()

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

#### ID3D11DeviceContext::ClearDepthStencilView()

* pDepthStencilView: 我们要清除的深度/模板视图
* ClearFlags: 确定要清除的数据类型
* Depth: 深度值
  * 如果我们在此处设置0.0f，则不会在屏幕上绘制任何东西
  * 如果我们在此处设置1.0f，则确保在屏幕上绘制所有对象
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

### 新知识

#### 本地(对象)空间(Local(object)space)

局部空间是相对于对象的空间, 对于一个正方形, 如果定义的顶点都基于全局对象, 那么进行其他顶点的定义就很困难

局部空间定义的顶点, 是相对于该对象中的其他顶点, 通常在包含3D对象的文件中定义, 该文件由3D建模程序创建

#### 世界空间(World space)

世界空间用于在世界空间中对每个对象进行相对定位

* 世界空间是物体在三维场景中的位置、大小和旋转角度
* 所有对象围绕着中心点(0,0,0)
* 要创建世界空间矩阵, 我们需要对要创建世界空间矩阵的对象进行转换
* 使用世界空间矩阵, 可以将对象顶点从局部空间转换到世界空间

#### 视图空间(view space)

* 基本上视作摄像空间: 摄像机固定在(0,0,0), 摄像机向下看z轴, 相机的向上方向是y轴
* 当我们进行转换时, 看起来相机在世界各地移动, 而实际上世界在移动, 而相机仍然在静止。
* 视图空间是通过创建一个矩阵来描述摄像机的位置、视图方向(目标)和向上(摄像机的y轴)来定义的

#### 投影空间(Projection space)

* 投影空间定义了3D场景中从相机的角度可以看到对象的区域: 对象进入则渲染, 对象退出则抛弃
* 由六个平面定义, 即近平面,、远平面、顶、左、底和右平面
  * 将“投影空间”渲染为几何图形, 它将看起来像是一个顶端被切掉的金字塔。 金字塔的尖端将是相机的位置, 尖端被切除的位置将是近z平面, 金字塔的基础将是远z平面。
  * 近平面和远平面由浮点值定义, 其他四个平面由长宽比和FOV（以弧度表示的视场）定义。

![透视投影][5]

#### 视场角: FOV

在摄影学中, 视角(angle of view)是在一般环境中, 相机可以接收影像的角度范围, 也可以常被称为视野。

![FOV示意图][6]

#### 屏幕空间(Screen space)

也就是屏幕这个2D空间, 偏向于概念性的概念, 实际中无需定义

#### 变换空间(Transforming space)

空间的变换涉及下面几步

* 局部空间中的对象顶点将发送到顶点着色器
  * 将一个顶点从一个空间转换到另一个空间, 将结果矩阵放到另一个称为WVP（世界视图投影）的矩阵中
  * 将WVP矩阵发送到效果文件中的常量缓冲区
  * VS将使用该缓冲区来转换对象顶点
* VS使用WVP传递的数据进行绘制
* 将顶点位置与WVP矩阵相乘, 从而转换空间
* 在渲染中删掉不在相机的视线范围内

#### 常量缓冲区(Constant buffers)

* 是一种在效果文件中的结构体, 包含可以从游戏代码中更新的变量

```c++
cbuffer cbPerObject
{
    float4x4 WVP;
};
```

* 通过刷新率区别
  * 由于调用需要大量时间, 因此这样可以减少调用函数的次数
  * 分为下面几种
    * 逐场景(per scene): 如整个场景中不变的照明
    * 逐帧(per frame): 如太阳在天空中移动
    * 逐对象(per object): 如每个物体都有运动

#### 测试单元

<https://www.zhihu.com/question/28729261>

#### 齐次坐标(投影坐标)(Todo: 补充齐次坐标的数学知识)

* 齐次坐标就是将一个原本是n维的向量用一个n+1维向量来表示
  * 比如(x,y,z,w)构成的三维坐标, 点 (x, y, z) 表示为 (xw, yw, zw, w), 且该点映射至平面上的点表示为 (xw, yw, zw)
  * 这样使得无穷远处的点更好的被定义
    * 如(1,2)被移至无穷远处, 在齐次坐标下表示为(1,2,1)->(1,2,0)

<https://www.cnblogs.com/lonelyxmas/p/10811299.html>----3D空间转2D屏幕

##### 透视除法(Perspective Division)

W 分量是投影仪到屏幕的距离, 用于3D物体投影至2D屏幕上

* 当缩放坐标的 W 为1时, 坐标不会增大或缩小, 保持原有的大小。所以, 当 W=1, 不会影响到 X, Y, Z 分量的值
* W<1时, 3D物体会变小
* W>1时, 3D物体会变大
* W=0时, 代表无穷远处, 渲染成一个点
* W<0时, 物体上下水平翻转

**透视法只是在矩阵变换后, 将齐次坐标中的w分量转换为1的专用名词**

也就是说, 最后转换"正确"后, 物体w分量一定为1

#### dx11中向效果文件传输矩阵

必须发送矩阵的**转置**

### 函数与类

#### XMMatrixPerspectiveFovLH()

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

#### XMMatrixIdentity()

返回一个空矩阵

## Tutorial8: Transformations

### 新知识

#### World View Projection三者转换

针对issue"Todo in tutorial7 #9"
<https://blog.csdn.net/allenjiao/article/details/79557760>

* code中的worldSpace矩阵, 实际上是用作将局部矩阵(Local)转换为world的矩阵
* 同理view projection也一样

#### 旋转(Rotation)

分别围绕x, y, z三轴进行旋转

```c++
     [ 1,      0,     0, 0]
Rx = [ 0, cos(r),sin(r), 0]
     [ 0,-sin(r),cos(r), 0]
     [ 0,      0,     0, 1]
 
XMMATRIX XMMatrixRotationX(
         FLOAT Angle    //Rotation angle in radians
)

     [cos(r), 0,-sin(r), 0]
Ry = [   0,   1,   0,    0]
     [sin(r), 0, cos(r), 0]
     [   0,   0,   0,    1]
 
XMMATRIX XMMatrixRotationY(
         FLOAT Angle    //Rotation angle in radians
)

     [ cos(r),sin(r), 0, 0]
Rz = [-sin(r),cos(r), 0, 0]
     [      0,     0, 1, 0]
     [      0,     0, 0, 1]
    
XMMATRIX XMMatrixRotationZ(
         FLOAT Angle    //Rotation angle in radians
)
//综合xyz
XMMATRIX XMMatrixRotationAxis(
         XMVECTOR Axis,    //Vector describing the axis of rotation
         FLOAT Angle    //Rotation angle in radians
)
```

#### 平移转换

```c++
    [ 1, 0, 0, 0]
T = [ 0, 1, 0, 0]
    [ 0, 0, 1, 0]
    [mx,my,mz, 1]

XMMATRIX XMMatrixTranslation(
         FLOAT OffsetX,    // Units translated on the x-axis
         FLOAT OffsetY,    // Units translated on the y-axis
         FLOAT OffsetZ    // Units translated on the z-axis
)

```

#### 索引缓存长度与顶点缓存长度

* 索引缓存需要通过片面数量\*3来得到总顶点数(包括重复顶点)
* 顶点缓存直接通过总顶点数(不包含重复顶点)
* 如一个正方体:
  * 索引:12\*3, 顶点:8

#### 世界矩阵的计算

spaceWorld = Translation \* Rotation \* Scale

### 函数与类

#### XMMatrixScaling()

返回一个缩放矩阵, 用于缩放物体

```c++
XMMATRIX XMMatrixScaling
(
    FLOAT ScaleX,    // x=axis scale
    FLOAT ScaleY,     // y-axis scale
    FLOAT ScaleZ    // z-axis scale
)
```

### 编程问题

#### 最后像两个正方形锥

```c++
vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
这个地方是八个顶点 而不是四个顶点
```

## Tutorial9: Render States

### 新知识

#### 渲染状态(Render states)

封装可用于配置Direct3d的设置, 以下三个是我们能够自定义的不同状态

* ID3D11RasterizerState: 用于自定义管道的光栅化阶段
* ID3D11BlendState: 用于混合(blending)
* ID3D11DepthStencilState: 用于设置深度模板测试

### 函数与类

#### D3D11_RASTERIZER_DESC

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

#### ID3D11Device::CreateRasterizerState()

* pRasterizerDesc: 描述指针
* ppRasterizerState: 返回的ID3D11RasterizerState客体

```c++
HRESULT CreateRasterizerState1(
  const D3D11_RASTERIZER_DESC1 *pRasterizerDesc,
  ID3D11RasterizerState1       **ppRasterizerState
);
```

#### ID3D11DeviceContext::RSSetState()

用于光栅状态与RS阶段绑定

[1]:images/render-pipeline-stages.png
[2]:images/basic-rebase-1.png
[3]:images/basic-rebase-2.png
[4]:images/basic-rebase-3.png
[5]:images/projection-space.png
[6]:images/FOV.jpg
