# Debug问题记录

记录整个tutorial过程中出现的编程问题

## Tutorial1: win32 API

### 关于长字符串与LPCTSTR类型不符

1. 通过改变项目属性中的字符集-使用多字节字符集
2. 或者在每个长字符串前加上L, 示例如下

```c++
LPCTSTR WndClassName = L"firstwindow";
```

### 整个项目结构

调试的过程中应该注意CALLBACK回调函数的运行时刻,并不是按照线性步进运行的

类似于单片机的**事件驱动模型**("event-driven" programming model)

## Tutorial2: Initializing Direct3D 11

### LNK2019无法解析的外部符号

```c++
#pragma comment ( lib, "D3D11.lib")//链接对应的库即可
```

### 无法解析的外部符号___vsnprintf

出现这个问题的原因是vs2017默认编译时将许多标准库采用内联方式处理,因而没有可以链接的标准库文件,所以要专门添加标准库文件来链接标准库中的函数。

根据链接添加库文件legacy_stdio_definitions.lib
<https://jingyan.baidu.com/article/48206aeab8516f216ad6b38c.html>

## Tutorial3: Begin Drawing

### LNK1104 无法打开d3dx11.lib

上面的解决平台方案调成x86, 因为项目属性里的库文件没有包含dx的x64库, 新添加这个也可以解决

### VS_Buffer 是 nullptr导致中断

几个全局变量指针未初始化, 先尝试使用zeromemory(错的)

很显然是D3DX11CompileFromFile()这一步编译未成功, 导致还是空指针

添加.fx特效文件后, 很显然就能够获得一个非空指针

### VS+fx文件编译:未定义main函数

添加了.fx文件后报错

由于vs不自带.fx文件的模板, 因此使用的是HLSL模板, vs会用HLSL编译器进行编译
属性改为"不参与生成"就行

测试之后.fx运行正确, 显示蓝色三角形, 但是自己写的main有问题, 需要修改

输入布局D3D11_INPUT_ELEMENT_DESC中, 颜色布局为RGB并不是RGBA

<http://blog.sina.com.cn/s/blog_5e83fce60102vd0r.html>

### 上传GitHub之后merge conflict(合并冲突)

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

![产生冲突](images/basic-rebase-1.png)
![变基](images/basic-rebase-2.png)
![回到主分支再次合并](images/basic-rebase-3.png)

## Tutorial4: Color!

### VS_Buffer 是 nullptr

* 与上一个tutorial的VS_Buffer 是 nullptr同样的问题
  * 编译选项改成x86, 文件夹视图是默认x64编译
  * 项目属性已添加了x64的源文件
* 项目属性已添加了x64的源文件, 报无法打开d3dx11.lib
  * 老老实实x86编译

## Tutorial5: Indices

### 编译完成但是背景黑色不显示正方形

* D3d11DeviceContent->IASetIndexBuffer(squareIndexBuffer,
DXGI_FORMAT_R32_UINT, 0);
  * 编码格式是DXGI_FORMAT_R32_UINT不是DXGI_FORMAT_R32_FLOAT

## Tutorial6: Depth

无

## Tutorial7: World View and Local Spaces (static Camera)

无

## Tutorial8: Transformations

### 最后像两个正方形锥

```c++
vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
这个地方是八个顶点 而不是四个顶点
```

## Tutorial9: Render States

无

## Tutorial10: Textures

### 贴图不显示

由于tutorial9中涉及到了光栅化, 其中有D3D11_RASTERIZER_DESC中描述的fillmode中有线框渲染, 改为实体渲染

## Tutorial11: Blending

无

## Tutorial12: Pixel Clipping

无

## Tutorial13: Simple Font

### **KeyedMutexD3d10** 是 nullptr

在创建指针的过程中, 通常使用这样的函数创造

```c++
//错误(void**)后需要的是指针的地址,即&KeyedMutexD3d10, KeyedMutexD3d10本身就是个指针, 因此编译不会出错, 但是运行会报错
hr = SharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)KeyedMutexD3d10);
```

## Tutorial14: High Resolution Timer

### **基于MSVC搭建vscode的c++编译环境**

<https://blog.csdn.net/qq_27825451/article/details/102981833>

全网唯一一个讲到点上的文章----根据VS中的项目设置, 对操作系统的环境变量进行修改, 增加三个环境变量INCLUDE LIBPATH LIB

感谢大佬@yuwenhuisama对我的指导, 让我用vcpkg+cmake来进行库管理, 但是dx11的库这种特定版本上面没有, 所以搭建完了之后我也没用, 不过以后如果需要外部导入库可以使用这个

另外<https://code.visualstudio.com/docs/cpp/config-msvc>根据微软爸爸给的教程, 搭建好最基本的c++运行环境

### VScode中cl编译出现C2039: “DrawTextW”: 不是“ID2D1RenderTarget”的成员

### 无法从“const wchar_t [12]”转换为“LPCTSTR”

vscode中c++程序为UNICODE编译, 不是VS中的多字符编译, 使用L""进行转换会报错, 进入tchar.h之后, 将L变为_T()即可

### tab制表符

VS当中制表符不是以空格构成的, 因此如果在VS当中使用了if的块(即,输入if之后回车形成的块), 就会报错

解决办法, 重写一遍

### <库目录>和<附加依赖项>的区别

<https://www.zhihu.com/question/21735299>

### error LNK2019: 无法解析的外部符号_imp__MessageBoxA@16，该符号在函数 _WinMain@16 中被引用

<https://blog.csdn.net/nanjingligong/article/details/8333462>

### D3DX11CompileFromFile找不到指定的文件

通过监视hr返回值可以找到这个错误(表面错误是VS_Buffer为空), 可能是编译的时候要链接.fx文件, 但是目前没有找到任何相关文件至此放弃VScode写DX

### error C2001: 常量中有换行符

UTF-8编码会使得VS编译报错, 建议全改成GB2312

<https://www.cnblogs.com/cocos2d-x/archive/2012/02/26/2368873.html#commentform>

## Tutorial15: Simple Lighting

