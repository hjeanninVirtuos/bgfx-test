
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
## Where:
* In main.cpp line 66, we create a 2nd buffer using a dummy layout.
* In the main loop line 85 we set a vertex buffer with an layoutHandle different from the layout used at the creation of the buffer.
* The first buffer (line 85) render just fine but the second buffer (line 88) does not.
* If you switch line 66 by line 67 you use the new layout at the creation of the 2nd buffer. -> it renders the second triangle forming a rectangle.
* It seems that in line 2655 of
```
modules\bgfx\bgfx\src\bgfx_p.h
```
when rendering the second buffer,

_dvb.m_startVertex is computed using the layout of the buffer creation and not the layouthandle.
* -> the second triangle stride is wrong -> no render.