# CORDIC Abs APFX

[![CMakeDocker](https://github.com/DrasLorus/Cordic_Abs_APFX/actions/workflows/docker.yml/badge.svg)](https://github.com/DrasLorus/Cordic_Abs_APFX/actions/workflows/docker.yml)

A free way to implement a CORDIC-based Complex Absolute Value using HLS, with bit-accurate precision.

## Goal

[CORDIC](https://en.wikipedia.org/wiki/CORDIC) (COordinate Rotation DIgital Computer) is an efficient way to implement hardware complex rotations (e.g. `z * exp(jw)`, with `z = x + jy` a complex and `w` a real angle). It also can be used to approximate the module of a complex. It is also useful for microcontrollers or microprocessors lacking floating-point units, as such multiplications can consume a noticeable amount of CPU cycles.

Indeed, `|z| = sqrt(x² + y²) = |exp(-j*angle(z)) * z|`, thus the module of `z` is the absolute value of the real part of its rotated version. So, ... CORDIC!

This implementation in C++14 (`-std=c++14`) is suitable for hardware simulation and (with the right headers and maybe a few tweaks) for synthesis.

This repository defines one working CORDIC-based Absolute Value units class, `CCordicAbs`.

## Test suite and dependencies

The [Catch2](https://github.com/catchorg/Catch) test framework has been used in conjunction with CTest to provides unit tests.

- Has been tested successfully compiled with:
  - GNU GCC 6.2 (*Xilinx bundled version*), 6.5, 9.4, 10.1, 10.2 and 11.2,
  - LLVM Clang 12.0 and 13.0,
- Uses Catch v2.13.7,
- Depends on Xilinx HLS arbitrary precision types, available as FOSS [here provided by Xilinx](https://github.com/Xilinx/HLS_arbitrary_Precision_Types) or [here patched by myself](https://github.com/DrasLorus/HLS_arbitrary_Precision_Types). Note: Xilinx also provides proprietary versions of those headers, suitable for synthesis and implementation, bundled with their products.

A Xilinx C++ HLS testbench is also available, as well as a TCL script to run simulation, synthesis, co-simulation and IP export and implementation if wanted. Xilinx HLS v2020.2 can be directly called from CMake by the target `run_hls` if the options `ENABLE_TESTING`, `ENABLE_XILINX` and `COSIM_XILINX` are enabled. If CMake feels like black magic, It is advice to use tools like `ccmake` (NCurses terminal interface to cmake) or `cmake-gui`.

## License and copyright

Copyright 2022 Camille "DrasLorus" Monière.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU
Lesser General Public License as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

A copy of the license is available [here in Markdown](lgpl-3.0.md) or [here in plain text](LICENSE).
