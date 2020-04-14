# �ʼ�

## Tutorial1: win32 API

### ��֪ʶ

### ��������

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
    * CS_DBLCLKS: ���û������ڸ���Ĵ����еĹ��λ���û�˫�����ʱ,ָʾWindows�򴰿ڹ��̷���˫����Ϣ
    * CS_HREDRAW: ����������,���������ڽ����ػ�
    * CS_VREDRAW: ����������,���������ڽ����ػ�
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

* ˫����: Ϊ�˽��������и���ͼ�����,��Ļ���ػ�ʱ���ڹ�Ƶ��ˢ��������˸������
  * �ڴ��ϴ���һ������ʾ�ؼ���ͬ��С�Ļ���,�ڻ�������ɻ��ƺ�ʹ�û���������ʾ�ؼ��Դﵽ��ʾһ����ˢ��һ��
* ������: ˫���������ʾ�ؼ��ȴ���̨��Ļ���ƵĹ���,������(�Լ����ߵĶ��ػ���)����������,��������ʱ��
  * �������û���(backBuffer)��̨����,��ʾ�ؼ���ȡǰ�û���,ǰ�û����ȡ���û���,�������������û���,����ǰ�û�������ȴ�
  * �ڶ��ַ���:���������൱������������,�Զ��еķ�ʽ(�Ƚ��ȳ�)���λ���-����ȡ
* rgbaɫ�ʹ���: ��rgb��һ��a, ����Alpha, ����͸����
* __uuidof: ������ȡ ĳ�ֽṹ���ӿڼ���ָ�롢���á����� ��������GUID,��Ϯ����ĳЩ���԰��л�ȡ���� typeof �����Ĳ�����
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

### ��������

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
  * ppImmediateContext: �豸�����Ľ������豸�ĳ��ַ���,��֧�ֶ��̲߳��������

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

������������ԭ����vs2017Ĭ�ϱ���ʱ������׼�����������ʽ����,���û�п������ӵı�׼���ļ�,����Ҫר����ӱ�׼���ļ������ӱ�׼���еĺ�����

����������ӿ��ļ�legacy_stdio_definitions.lib
<https://jingyan.baidu.com/article/48206aeab8516f216ad6b38c.html>

## Tutorial3: Begin Drawing

### ��֪ʶ

* ��Ⱦ����: ����3D�����(�൱������)������3D����ת����һ��2DͼƬ��ʾ����Ļ��.
  * ��������ˮ�߽���, һ���׶ε����������һ���׶ε�����
* dx11�й��ߵĽ���: ����, ��Բ���̱�ʾ���û�����, ���ν��̿�ͨ��direct3d 11 device context�޸��趨
* ZeroMemory(), ��һ���ڴ�ͨ�������, �൱�ڳ�ʼ��ָ��(�ȸ�ָ��һ��nullҪţ��һ��?)
  * Todo: �Ƚ�һ��

```c++
D3D11_BUFFER_DESC vertexBufferDesc;
ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );
```

![ʮ����������][1]

#### IA:Input Assembler

��ȡ�������ݡ������������Ȼ��,��ʹ�����ݴ�������ͼԪ,��traingles��square��lines��points,��ЩͼԪ���������׶������ʹ�á�

* ��������֮ǰ, ��Ҫ���û����������û������ˡ����벼�ֺͻ��������
    * ��������(ͼԪ����?)(Primitive Topology): ����洢������������Ϣ(����λ��, ����, ��ɫ�ȵ�), �����ǰ���һ���Ĺ�����֯������γ�ͼ��(������򱻳�ΪPrimitive Topology).
      * <https://blog.csdn.net/u010333737/article/details/78556583>
* �ȴ�������: IAʹ�õ������������Ƕ��㻺����������������
* �ٴ������벼�ֶ���: ����Direct3d���ǵ��ı��ṹ��ʲô���,�Լ���δ������ǵĶ���ṹ�е�ÿ�����
  * ͨ��ID3D11Device::CreateInputLayout()ʵ����
* ����һ�����㻺��
  * ͨ��ID3D11Device::CreateBuffer()ʵ����
* �󶨲���˵���Ͷ��㻺��
  * ID3D11DeviceContext::IASetVertexBuffers()��IASetInputLayout()
* ���û�������,�Ա�IA֪�����ʹ�ö��㲢�������������λ�ֱ��֮��Ļ���Ԫ�ء�
  * ��, �����������㾿����������, ������������������
  * ID3D11DeviceContext::IASetPrimitiveTopology()
* ����׼������,ͨ��ID3D11DeviceContext::Draw()����ͼԪ

```c++
//The vertex Structure, ������Ķ���ṹ����λ�á���ɫ����Ԫ��
struct Vertex
{
    D3DXVECTOR3 pos;
    D3DXCOLOR   color;
};


//The input-layout description, ������벼�ֶ��󽫻�������Ԫ�طֱ��Ӧλ������ɫ
D3D11_INPUT_ELEMENT_DESC layout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
```

#### VS:Vertex Shader

�ǿɱ�̵���ɫ��,ͨ��VS����ִ������任,����,����,���������λ����ͼ֮��Ĳ�����

��ʹ����û�б仯, Ҳ����ʵ�ֶ�����ɫ��ʹ�ùܵ���������

�ܵ��еĶ�����ɫ��ͨ����C++����HLSLʵ��, ����һ�����㷵��һ������

#### HS: Hull Shader

* �����ɫ���׶�(HS)��ϸ�����׶�(TS)������ɫ���׶�(DS)�������½׶ι�ͬЭ����ʵ�ֳ�Ϊ��Ƕ(tesselation)�Ķ�����
  * �����ǽ�һ��ͼԪ�����������λ�ֱ�ߣ��ָ������С�Ĳ��֣��Ӷ��Լ�����ٶ�����ģ�͵�ϸ�ڡ�
  * ����ʾ֮ǰ, �Ƚ�ͼԪ��GPU�д���, Ȼ��ֱ��ӳ�䵽��Ļ
    * ����: ��������CPU �ڴ��д�����ʱ��

#### TS: Tessellator 

��HS�л�ȡ����, ��ͼԪ���зָ�, �ٽ����ݴ���DS

#### DS: Domain Shader

�ɱ����ɫ��, ��HS��ȡ����λ��, ������TS�����ݴӶ����ϸ��

���������ֱ�ӽ��ܶ���λ��, ��ô�������������ϸ��, ��Ϊ�������λ�ֱ�ߵ�������Ӹ��ඥ�㲻�������κ�ϸ��

#### GS: Geometry Shader

�ڶ����Ƭ����ɫ��֮����һ����ѡ����ɫ��������������ɫ��(Geometry Shader)��������ɫ����һ��������ʾΪһ����������ͼ�Σ�primitive���Ķ�����Ϊ���룬���������һ������������Ρ�������ɫ���ڽ���Щ���㷢�͵���һ����ɫ�׶�֮ǰ�����Խ���Щ����ת��Ϊ����Ϊ���ʵ����ݡ�������ɫ������˼�ĵط����������԰ѣ�һ������������ת��Ϊ��ȫ��ͬ�Ļ���ͼ�Σ�primitive�����Ӷ����ɱ�ԭ����ö�Ķ��㡣

#### SO: Stream Output

���ڴӹ����л�ȡ�������ݣ��ر�����û��GS�������

* ��SO����Ķ�������ʼ�����б���ʽ�������ڴ�, ��Զ���ᷢ����������ͼԪ
  * ��������ͼԪ: ����һ��ֻ������������Ϣ��������

#### RS: Rasterization Stage

RS�׶λ�ȡ���͸�����ʸ����Ϣ����״��ͼԪ������ͨ����ÿ��ͼԪ֮�����ÿ�������ֵ������ת��Ϊ���ء� ��������ü����������ǲü���Ļ��ͼ֮���ͼԪ��

��������Ҳ������ʲô����Ⱦ(*Rasterization*)

#### PS: Pixel Shader

* �ý׶ν��м��㲢�޸Ľ�����Ļ�Ͽ�����ÿ������, ����ÿ������Ƭ�ε�������ɫ
* �붥����ɫ��һ��, һһӳ��
  * һ����������, ���һ������
* ����Ƭ��: �ǽ������Ƶ���Ļ�ϵ�ÿ��Ǳ������

#### OM: Output Merger

�˽׶λ�ȡ����Ƭ�κ����/ģ�建��������ȷ��ʵ�ʽ���Щ����д����ȾĿ��

�˽׶κ�, �����û���ӳ�䵽��Ļ��

#### ��ɫ��������

d3d��һ��״̬��, ֻ�ܱ��浱ǰ��״̬���趨, ��˶���ɫ����������Ҫ������ʱ(ÿ����Ⱦ����֮ǰ)����, ���������ǳ�ʼ����ʱ������

#### ��ͼviewport

һ������Ĵ���, ������֪��դ�������ĸ���������Ҫ���Ƹ��û�������

#### ƫ����offset

�����ƫ����, ��ID3D11DeviceContext::Draw()�еĵڶ�������offset
���翪ʼ���ƵĶ�����v, �ӵ��������㿪ʼ����, ƫ����Ϊ2

#### Ч���ļ�(effect Files)

### ��������

#### D3D11_INPUT_ELEMENT_DESC

* SemanticName: ��Ԫ�ع������ַ���,���ַ��������ڽ�����ṹ�е�Ԫ��ӳ�䵽������ɫ���е�Ԫ��
* SemanticIndex: ��һ��������, ��"POSITION0"��"POSITION1"
* Format: ���������ʽ
* InputSlot: �����, �ɵ��߽���Ҳ�ɶ��߽���
* AlignedByteOffset: ������Ԫ�ص��ֽ�ƫ����(?û����)
* InstanceDataStepRate: ʵ����

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

����ͨ��Ч���ļ�����������ɫ��

* pSrcFile: ��ɫ�����ڵ��ļ���
* pDefines: ָ��������ָ��
* pInclude: �����ɫ��ͨ��#include���, ���NULL, ������NULL
* pFunctionName: ��ɫ����������
* pProfile: ��ɫ���汾
* Flags1: �����־
* Flags2: Ч����־
* pPump: ����߳����
* ppShader: ������ɫ�����й���ɫ����Ϣ�Ļ�����
* ppErrorMsgs: ������Ϣ
* pHResult: ����ֵ, ����ͨ�� HRESULT hr = D3DX11CompileFromFile(...); ��D3DX11CompileFromFile(...,&hr)���ַ�ʽʵ��

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

#### CreateVertexShader()��CreatePixelShader()

* pShaderBytecode: ��ɫ���������
* BytecodeLength: ���泤��
* pClassLinkage: �����ӽӿ�
* ppVertexShader ppPixelShader: ���ص���ɫ��

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

#### VSSetShader()��PSSetShader()

* ppClassInstances: ʵ����
* NumClassInstances: �������������

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

����֮ǰ������, ����д��

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

* pSysMem: ���뻺�������
* SysMemPitch: ���ֽ�Ϊ��λ�Ĵ������е�һ�е���һ�еľ���, ����2D��3D������
* SysMemSlicePitch: ��3D�����У���һ����ȼ�����һ����ȼ���ľ���

```c++
typedef struct D3D11_SUBRESOURCE_DATA
{
   const    void *pSysMem;
   UINT     SysMemPitch;
   UINT     SysMemSlicePitch;
}     D3D11_SUBRESOURCE_DATA;
```

#### CreateBuffer()

���ڴ�������

* pDesc: ָ�򻺴�����
* pInitialData: ָ��һ������Դ���ݽṹ��ָ�룬�ýṹ��������Ҫ�������������, ���֮���������������Ϊnull
* ppBuffer: ���صĻ���

```c++
HRESULT CreateBuffer( 
   [in]    const D3D11_BUFFER_DESC *pDesc,
   [in]    const D3D11_SUBRESOURCE_DATA *pInitialData,
   [in]    ID3D11Buffer **ppBuffer
);
```

#### IASetVertexBuffers()

���㻺����IA��

* StartSlot: �������(input slot)��
* NumBuffers: �󶨵Ļ�������
* ppVertexBuffers: ָ�򶥵㻺��
* pStrides: ÿ������Ĵ�С
* pOffsets: ��ʼλ�õĻ�������ʼ���ֽ�ƫ����(?)

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

�������벼��

* pInputElementDescs: �������㲼�ֵ�����
* NumElements: ���㲼��Ԫ������
* pShaderBytecodeWithInputSignature: ָ�򶥵���ɫ����ָ��
* BytecodeLength: ������ɫ����С
* ppInputLayout: �������벼��

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

���벼�ְ�����(Ӧ����������IA��)

* pInputLayout: ���벼��

```c++
void STDMETHODCALLTYPE IASetInputLayout( 
   [in]   ID3D11InputLayout *pInputLayout
);
```

#### IASetPrimitiveTopology()

����ͼԪ����, ��IA������Ҫ���ݵ�ͼԪ����

* D3D10_PRIMITIVE_TOPOLOGY_POINTLIST: �����㼯
* D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP: ����(һ����������)
* D3D10_PRIMITIVE_TOPOLOGY_LINELIST: ������(�������)
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: �����δ�(ÿ��������ͨ�����ö�������(�ɹ��ö������))
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST: �����μ�(�໥����)
* D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ: ������ɫ��ʹ��

#### RSSetViewports()

������ͼ, ����󶨵���դ��ɫ��, ��һ������Ϊ�󶨵���ͼ����, �ڶ���Ϊ��ͼ���ָ��

#### ID3D11DeviceContext::Draw()

��һ��������Ҫ���ƵĶ��������ڶ���������Ҫ��ʼ���ƵĶ������鿪ʼ��ƫ������

### ��������

#### LNK1104 �޷���d3dx11.lib

����Ľ��ƽ̨��������x86, ��Ϊ��Ŀ������Ŀ��ļ�û�а���dx��x64��, ��������Ҳ���Խ��

#### VS_Buffer �� nullptr�����ж�

����ȫ�ֱ���ָ��δ��ʼ��, �ȳ���ʹ��zeromemory(���)

����Ȼ��D3DX11CompileFromFile()��һ������δ�ɹ�, ���»��ǿ�ָ��

���.fx��Ч�ļ���, ����Ȼ���ܹ����һ���ǿ�ָ��

#### VS+fx�ļ�����:δ����main����

�����.fx�ļ��󱨴�

����vs���Դ�.fx�ļ���ģ��, ���ʹ�õ���HLSLģ��, vs����HLSL���������б���
���Ը�Ϊ"����������"����

����֮��.fx������ȷ, ��ʾ��ɫ������, �����Լ�д��main������, ��Ҫ�޸�

���벼��D3D11_INPUT_ELEMENT_DESC��, ��ɫ����ΪRGB������RGBA

<http://blog.sina.com.cn/s/blog_5e83fce60102vd0r.html>

[1]:images/render-pipeline-stages.png
