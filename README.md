
## Highlighting issue in BGFX, where the computation of the number of vertices becomes problematic when giving an layouthandle different from the layout at creation of the buffer.

-----
## Install (for windows):
```
git clone --recursive git@github.com:hjeanninVirtuos/bgfx-test.git
```

```
cd bgfx-test
```

```
cmake -B $PSScriptRoot/build $PSScriptRoot
```

```
cmake --build $PSScriptRoot/build
```

```
./build/Debug/bgfx-test.exe
```

-----
## Issue explanation:
* In main.cpp 
* Create 2 vertex layout with respectively a stride of 1 and 8 bytes.
* Create a layoutHandle for the 8 bytes stride layout.
* Create 2 vertex buffer using dummy layout of stride 1 byte.
* Main loop line 85, set both buffers with layoutHandle of stride 8.
* **PROBLEM:** The first buffer (line 85) render his triangle just fine but the second buffer (line 88) does not.
* Switching line 66 by line 67 makes the 2nd buffer creation use the layout of stride 8. -> second triangle renders.
* **ISSUE:** (Line 2655 bgfx_p.h) When rendering the second buffer, _dvb.m_startVertex is wrongly computed (6 x 8 + 1 = 49) instead (6 x 1 + 1 = 7), the layout of the buffer creation is used instead of the layoutHandle.
* -> the second triangle does not render.

**TLDR: switch line 66 and 67 of main.cpp, compare results.**