# MCPELinux - 在桌面 Linux 运行 MCPE for Android

## 目录
- [简介](#简介)
- [构建、运行与调试](#构建、运行与调试)
- [许可](#许可)

## 简介
  本项目提供一个在桌面 Linux 上运行 MCPE 的环境。与 [mcpelauncher-linux](https://github.com/MCMrARM/mcpelauncher-linux/)
  以及 [minecraft-linux](https://github.com/minecraft-linux/) 类似，并参考了其 ABI 差异处理库
  [libc-shim](https://github.com/minecraft-linux/libc-shim)。
  但本项目并*不是*上述项目的分支，相比之下本项目更加简单，实现思路也有所不同。
  只能运行 MCPE 0.14 至 1.1 的部分版本。1.2 以后的版本需要正版验证不能运行。
  欢迎有志之士折腾。
## 构建、运行与调试
  通过 meson 可以轻松构建所有模块，请注意需要使用 i686-linux-gnu- 工具链以编译 32 位程序，需要安装一些 32 位的库如 Mesa，GLFW，libpng 等。
```
  cd 本项目源码根目录
  meson 你的编译文件夹 --prefix=你的安装前缀 --cross-file=你的交叉编译描述文件
  cd 你的编译文件夹
  ninja install
```
  随便找个文件夹即可，不要用系统目录。

  编译完成之后会在`你的安装前缀/bin/`目录下得到一个`main`可执行文件，执行该文件以了解使用方法。其中`-mcpe`选项指定为你的 apk 安装包解压后的根目录。
  比如你在`/home/cosine/mcpe`解压了安装包，那么素材文件夹就是`/home/cosine/mcpe/assets`，而`libminecraftpe.so`应该在
  `/home/cosine/mcpe/lib/x86/libminecraftpe.so`。
  
  gdb 运行 main 之前用 `set solib-search-path <path>` 命令把`libminecraftpe.so`所在目录加进去即可在调试时看到 MCPE 的符号。
## 许可
  本项目所有模块以 [GPL v2.0](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html) 授权。
  但 aglinker 是修改版的 Bionic libc linker，来自 AOSP，有其自己的许可。
