# �ʼ�

## Tutorial1: win32 API

### ��֪ʶ

#### ��������

����win32 API�ӿڿɲ����ֲ�<http://www.yfvb.com/help/win32sdk/webhelpcontents.htm>

* function InitializeWindow()
  * hInstance: ���
  * ShowWnd: ���������ʾ���ڵ�����
  * Width,Height: ����
  * windowed: �Ƿ񴰿ڻ�(ȫ����)
* class WNDCLASS
  * cbSize: �೤��
  * style: ��������(��CS��ͷ),�ټ�������
    * CS_CLASSDC: һ��OK��ť
    * CS_DBLCLKS: ���û������ڸ���Ĵ����еĹ��λ���û�˫�����ʱ��ָʾWindows�򴰿ڹ��̷���˫����Ϣ
    * CS_HREDRAW: ���������������������ڽ����ػ�
    * CS_VREDRAW: ���������������������ڽ����ػ�
    * CS_NOCLOSE: �޹رռ�
    * CS_PARENTDC: ʹ���Ӵ����ڸ������ϻ���
  * lpfnWndProc: ָ�򴰿����к�����ָ��
  * cbClsExtra: WNDCLASSEX֮�����Ķ����ֽ�
  * cbWndExtra: ��������ʵ������Ķ����ֽ�
  * hInstance: ��ǰ���ڵľ��
  * ���漸�������־�֪����
* function CreateWindowEx()
  * ͻȻ����д��,�Ժ��Լ����,�����Ժ�����û���Լ�����

### ��������

* ʹ��VS�Դ���GitHub��չ���а汾����

����ɲμ�����: <https://zhuanlan.zhihu.com/p/31031838>

* ���Ѿ���������Ŀ�п���ʹ����չ����GitHub�ϴ����µĲֿ�Ҳ�����ڴ�����Ŀ�Ŀ�ʼ����Զ��ͬ��

* VS��Ӧ���л�����һ�п��еĿ�ݼ���ctrl+shift+enter,��VS Code�е�ctrl+enter����һ��

### ��������

* ���ڳ��ַ�����LPCTSTR���Ͳ���

1. ͨ���ı���Ŀ�����е��ַ���-ʹ�ö��ֽ��ַ���
2. ������ÿ�����ַ���ǰ����L, ʾ������

```c++
LPCTSTR WndClassName = L"firstwindow";
```

* ���ԵĹ�����Ӧ��ע��CALLBACK�ص�����������ʱ��,�����ǰ������Բ������е�

�����ڵ�Ƭ����**�¼�����ģ��**("event-driven" programming model)

## Tutorial2: Initializing Direct3D 11

### ��֪ʶ

#### �� ��֪ʶ

* ˫����: Ϊ�˽��������и���ͼ�����,��Ļ���ػ�ʱ���ڹ�Ƶ��ˢ��������˸������
  * �ڴ��ϴ���һ������ʾ�ؼ���ͬ��С�Ļ���,�ڻ�������ɻ��ƺ�ʹ�û���������ʾ�ؼ��Դﵽ��ʾһ����ˢ��һ��
* ������: ˫���������ʾ�ؼ��ȴ���̨��Ļ���ƵĹ���,������(�Լ����ߵĶ��ػ���)����������,��������ʱ��
  * �������û���(backBuffer)��̨����,��ʾ�ؼ���ȡǰ�û���,ǰ�û����ȡ���û���,�������������û���,����ǰ�û�������ȴ�
  * �ڶ��ַ���:���������൱������������,�Զ��еķ�ʽ(�Ƚ��ȳ�)���λ���-����ȡ
* rgbaɫ�ʹ���: ��rgb��һ��a, ����Alpha, ����͸����
* __uuidof: ������ȡ ĳ�ֽṹ���ӿڼ���ָ�롢���á����� ��������GUID����Ϯ����ĳЩ���԰��л�ȡ���� typeof �����Ĳ�����
* SwapChain: ����������,�����ػ���ĶԱ����<https://zhuanlan.zhihu.com/p/104244526>
* ����ͼ�γ��̶����Լ�����Ⱦ���Ż�, Ҫ�����������д
* ֱ���ڳ����б�д�����׳������ڿ��ٶ�λerror, Ҳ�ܽ��һЩ���벻����bug
  * ͨ���鿴HRESULT(������ؽ��), �����жϺ����������
    * S_OK: ���гɹ�
    * E_NOTIMPL: ����δʵ��
    * E_NOINTERFACE: �ӿڲ�֧��
    * E_ABORT: ��ֹ
    * E_FAIL: ����
    * E_INVALIDARG: һ������������Ч
    * DXGetErrorDescription(): �ɵõ���ϸ������Ϣ, ��ҪDXErr.h

#### ��������

* �й�BackBuffer: DXGI_MODE_DESC
  * RefreshRate: ˢ����
  * Format: ��ʾ��ʽ
  * ScanlineOrdering: ������դ��������Ⱦ�������ϵķ�ʽ,����ʹ��˫����,�����Ⱦ˳����Ҫ
  * Scaling: ����ͼ������Ӧ�������ķֱ���
    * DXGI_MODE_SCALING_UNSPECIFIED: ��ȷ��ģʽ
    * DXGI_MODE_SCALING_CENTERED: ��������Ƕ��ģʽ
    * DXGI_MODE_SCALING_STRETCHED: ����Ӧģʽ

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

* �й�SwapChain: DXGI_SWAP_CHAIN_DESC
  * BufferDesc: ���û���
  * SampleDesc: ����������
  * BufferUsage: CPU�Ժ��û���ķ���
  * BufferCount: ���û�����
  * OutputWindow: ���ھ��
  * SwapEffect: ������ʾ�ؼ��뻺����ν���,ʹ��Ч�����
  * Flags: ������������Ϊ�Ķ����־

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

* �й�D3D11CreateDeviceAndSwapChain(): ����d3d�豸,�豸������,������
  * pAdapter: ��Ƶ������ָ��
  * DriverType: ����d3d��α�����
    * D3D_DRIVER_TYPE_HARDWARE: GPU����
  * Software: ָ�������դ����dll�ļ����
  * pFeatureLevels: ָ��Dx�汾
  * FeatureLevels: ����ָ������
  * ppSwapChain: IDXGISwapChain�ӿ�ָ��
  * pFeatureLevel: ���ֿ��õ���߹��ܼ���
  * ppImmediateContext: �豸�����Ľ������豸�ĳ��ַ�������֧�ֶ��̲߳��������

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

* GetBuffer(): �������ǵĺ��û���, ���ڴ�����ȾĿ����ͼ��
  * Buffer: ֻ���ʵ�һ������, Ϊ0
  * riid: ���ĺ��û���Ľӿ����͵Ĳο�ID
  * ppSurface: ָ�򼴽���Ⱦ�ı���

```c++
HRESULT GetBuffer(
  [in]       UINT Buffer,
  [in]       REFIID riid,
  [in, out]  void **ppSurface
);
```

* CreateRenderTargetView(): ������ȾĿ����ͼ
  * pResource: ���û�����Ϊ��Դָ��
  * pDesc: ��ΪNULLʹ����ͼ�ܷ���minimap level 0������Դ(?)
  * ppRTView: ��ȾĿ����ͼ��ָ��

```c++
HRESULT CreateRenderTargetView(
  [in]   ID3D11Resource *pResource,
  [in]   const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
  [out]  ID3D11RenderTargetView **ppRTView
);
```

* OMSetRenderTargets(): ����ȾĿ����ͼ��ܵ�����ϲ�,ͬʱҲ�����ǵ����/ģ�建����
  * NumViews: �󶨵���ȾĿ������
  * ppRenderTargetViews: ָ����ȾĿ����ͼ��ָ��
  * pDepthStencilView: ָ�����/ģ�建������ָ��

```c++
void OMSetRenderTargets(
  [in]  UINT NumViews,
  [in]  ID3D11RenderTargetView *const **ppRenderTargetViews,
  [in]  ID3D11DepthStencilView *pDepthStencilView
);
```

### ��������

* LNK2019�޷��������ⲿ����

#pragma comment ( lib, "D3D11.lib"), ���Ӷ�Ӧ�Ŀ⼴��

* �޷��������ⲿ����___vsnprintf

������������ԭ����vs2017Ĭ�ϱ���ʱ������׼�����������ʽ�������û�п������ӵı�׼���ļ�������Ҫר����ӱ�׼���ļ������ӱ�׼���еĺ�����

����������ӿ��ļ�legacy_stdio_definitions.lib
<https://jingyan.baidu.com/article/48206aeab8516f216ad6b38c.html>