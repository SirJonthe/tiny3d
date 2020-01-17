# tiny3d
Copyright (c) 2020 Jonathan Karlsson

## About

Tiny3d is a platform independent software rendering API aiming to emulate the feature set and look of Quake and Playstation era video games.

## Features

### Platform agnostic

Tiny3d aims to be a platform agnostic software renderer by only providing the tools to render graphics rather than display the graphics. In order for tiny3d to be used in full it is left to the programmer to develop code that ties the platform independent render output from tiny3d to a platform dependent graphics API capable to display the tiny3d output.

## Optimization

### Compression

Colors are represented using 16 bits, with 5 bits for each channel of red, green, and blue, and 1 bit of blending information. Blending information is used differently depending on context.

Images use uncompressed 16 bits per pixel.

Textures use color cell compression (CCC) at a 4x4 texel block granularity to reduce the number of bits per texel to 3.

![alt text](https://i.imgur.com/yJDghZr.png "Color cell compression")

### Dithering

In order to emulate a greater color depth, the renderer offsets ('dithers') the values of nearby colors in a pixel grid to allow for smoother transitions between shades of colors rather than sharp stepping between colors.

### Access pattern

Textures use an optimized access pattern (Morton order) in order to accelerate rendering of geometry that misaligns the applied texture with its stored axis.

### Threading

Tiny3d does not use any threading directly, but is designed in such a way that multiple threads can work on composing a single image simultaneously by giving each thread its own workspace on the image. Tiny3d makes creating such a workspace as simple as setting up non-overlapping rectangles and pass them as arguments to the rendering functions, which can then be called in parallel by multiple threads.

### SIMD fragments

Pixels are processed in groups (SIMD fragments) using vector instructions. The size of the group depends on what vector instructions tiny3d is compiled with support for. Each group of pixels is processed at the same performance cost as processing one pixel.

![alt text](https://i.imgur.com/lMz7emQ.png "Renderer parallelism")

## Future

### General

The feature set of tiny3d is meant to be small and portable. Continued development aims to never break platform independence, move away from retro graphics, or let feature creep make maintainance difficult.

There are still features that should ideally be implemented and/or removed, as listed below.

### Depth buffers

Support for depth buffers will be removed. This increases performance, but may cause some scenes not to be rendered properly.

### Color depth

16 bit colors will be reduced to 8 bit indices + 256 entry 16 bit color palette. This allows for better performance and visual consistency, but may result in scenes that do not look the way the artists intended.

### Graphical user interface (GUI)

Tiny3d does not aim to implement widgets for a GUI. It however should ideally improve existing facilities for the programmer to write a custom GUI. Examples of improvements include icon rendering.

### Redesign

There are currently some initial plans to transition the tiny3d API from an stateless immediate mode API to a buffered state machine API. This will be beneficial for projects that use tiny3d as one of potentially several alternative graphics API:s as the design will more closely mirror efficient hardware accelerated API:s.

## Building

Tiny3d should be able to be built using most C++ compilers on essentially any platform since it uses nothing but standard libraries and platform independent code. Only the headers and source files need to be included to the compiler. No libraries need to be included.

Compiling for ARM may need some additional tweaks for performance and use of vector instructions. For instance, on g++ the following compiler options should be enabled
```
	-mcpu=cortex-a7
	-mfloat-abi=hard
	-mfpu=neon-vfpv4
```
For g++ and ARM see [this link](https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html)

## Credits

The images and videos above include content from the following creators:

Spiderbot and gun model (c) 2012 killyoverdrive (https://opengameart.org/users/killyoverdrive)
Plant models (c) 2018 pistachio (https://opengameart.org/users/pistachio)
Canister model (c) 2014 Ron Kapaun (https://opengameart.org/users/hreikin)

