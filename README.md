# m173

m173 it's a hackable Minecraft Beta 1.7.3 server written in C++.

## Planned features

* Forward more events to Lua scripts
* Sleeping
* Weather changing
* Sitting on mobs
* Statistics
* Actually working map item
* Smelting
* Redstone
* Mobs
* Boats
* Minecarts
* Physics
* Fishing
* PvP with long range weapons
* AI for mobs
* Multiple worlds
* Pickuping up/Dropping items
* TNT/Explosions

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
4. [lua](https://github.com/lua/lua)

## Building thirdparty

Third party project uses `ExternalProject` CMake submodule to build all the dependencies. So the only thing you have to do is to run `cmake -S./ -B./build/` inside `3rd` directory and then build it using your build system. It should automatically create `install` folder inside `3rd` where built libraries will be stored.

## Building server

The server build type should match the thirdparty one, if you built `3rd` project with `Debug` build type then the server should be built with the `Debug` too. Build steps for the server are basically the same as for `3rd` project.
