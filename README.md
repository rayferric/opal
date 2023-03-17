# OPAL

Overpowered Algorithm Library is a collection of algorithms and data structures packaged as a header-only library for easy use with C++ 20 and higher.

# Important Information

Build artifacts including showcase videos and the detailed description of each algorithm can be found in the [`build`](./build) directory.

The algorithms are described in detail in the [PDF document](./build/opal.pdf).

Demonstration videos for each algorithm are available in the [`build/manim`](./build/manim) directory.

Additionally, C++ implementation of every algorithm is located under [`./include/opal`](./include/opal).

# Building Demo Executables

The demonstration programs are not distributed in precompiled form as this would pose a series of problems, including the requirement of extensive cross-platform support and possible malware issues.

In order to build the demonstration executables, CMake 3.20 or newer is required along with a compiler capable of processing C++ 20 source code.
The code was tested for compilation errors or warning against the latest versions of Clang and GCC on Linux operating system.

To start the build process, issue the following commands:

```bash
chmod +x scripts/cmake
scripts/cmake
```

Alternatively:

```bash
python scripts/cmake
```

The resulting binaries will be packaged in the [`build/bin`](./build/bin) directory.
