# m173

m173 is a Minecraft Beta 1.7.3 server written in C++.

TODO: More description

## Dependencies

> [!TIP]
> Every dependency described below comes with the project as a GitHub submodule,
> As long as you clone the server with `--recurse-submodules` parameter you
> don't have to worry about downloading them manually. However, you should
> build them separately from base server, using CMake.

1. [zlib](https://github.com/madler/zlib)
2. [sockpp](https://github.com/fpagliughi/sockpp)
3. [spdlog](https://github.com/gabime/spdlog)

## Building thirdparty

Third party project uses `ExternalProject` CMake submodule to build all the dependencies. So the only thing you have to do is to run `cmake -S./ -B./build/` inside `3rd` directory and then build it using your build system. It should automatically create `install` folder inside `3rd` where built libraries will be stored.

## Building server

The server build type should match the thirdparty one, if you built `3rd` project with `Debug` build type then the server should be built with the `Debug` too. Build steps for the server are basically the same as for `3rd` project.
