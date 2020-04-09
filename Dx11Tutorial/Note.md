# 笔记

## Tutorial1: win32 API

### 新知识

#### 函数与类

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
    * CS_DBLCLKS: 当用户在属于该类的窗口中的光标位于用户双击鼠标时，指示Windows向窗口过程发送双击消息
    * CS_HREDRAW: 如果横向调整，则整个窗口将被重绘
    * CS_VREDRAW: 如果纵向调整，则整个窗口将被重绘
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

#### 真 新知识

* 双缓冲: 为了解决窗体进行复杂图像处理后,屏幕在重绘时由于过频的刷新引起闪烁的问题
  * 内存上创建一个与显示控件相同大小的画布,在画布上完成绘制后使用画布覆盖显示控件以达到显示一次则刷新一次
* 三缓冲: 双缓冲存在显示控件等待后台屏幕绘制的过程,三缓冲(以及更高的多重缓冲)解决这个问题,做到更低时延
  * 两个后置缓存(backBuffer)后台绘制,显示控件读取前置缓存,前置缓存读取后置缓存,由于有两个后置缓存,所以前置缓存无需等待
  * 第二种方法:三个缓存相当于三个交换链,以队列的方式(先进先出)依次绘制-被读取
* rgba色彩管理: 比rgb多一个a, 代表Alpha, 控制透明度
* __uuidof: 用来获取 某种结构、接口及其指针、引用、变量 所关联的GUID，类袭似于某些语言百中获取类型 typeof 这样的操作。
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

#### 函数与类

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
  * ppImmediateContext: 设备上下文将用于设备的呈现方法，以支持多线程并提高性能

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

出现这个问题的原因是vs2017默认编译时将许多标准库采用内联方式处理，因而没有可以链接的标准库文件，所以要专门添加标准库文件来链接标准库中的函数。

根据链接添加库文件legacy_stdio_definitions.lib
<https://jingyan.baidu.com/article/48206aeab8516f216ad6b38c.html>