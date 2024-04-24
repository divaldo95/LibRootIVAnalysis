# Root IV analysis library and C# wrapper

Root IV analysis code can be found in `source` and `include` directories. Building the library will result in a shared library (.so on Linux, .dylib on MacOS)

## Prerequisities
* CERN Root (tested on 6.30)
* Linux or MacOS
* CMake

## Building
```
git clone https://github.com/divaldo95/LibRootIVAnalysis.git
mkdir build && cd build
cmake ../
make -jN (where N is the number of jobs to run simultaneously)
```

# C# test application

## Prerequisities
* Dotnet Core 8.0
* Newtonsoft.JSON
* Library built

## How to run
1. Build C# app, this will create `CSharpTest/bin/Debug/net8.0` folder
2. Copy the library to the above mentioned folder (.so or .dylib)
3. Get a measurement JSON file and modify [Program.cs](CSharpTest/Program.cs) accordingly
4. Run and check the output