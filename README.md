# Root IV analysis library and C# wrapper

Root IV analysis code can be found in `source` and `include` directories. Building the library will result in a shared library (.so on Linux, .dylib on MacOS)

## Prerequisities
* CERN Root (tested on 6.36)
* Windows, Linux, MacOS
* CMake

## Building
### Linux and MacOS
```
git clone https://github.com/divaldo95/LibRootIVAnalysis.git
mkdir build && cd build
cmake ../
make -jN (where N is the number of jobs to run simultaneously)
```

### Windows
Clone this repository, then open a `Developer PowerShell for VS 2022` and run the following commands:
```
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ../
MSBuild.exe .\RootIVAnalyser.sln -maxcpucount:N (where N is the number of jobs to run simultaneously)
```

## Changelog
### 2025.08.31
- Added support for Windows build (Visual Studio 22)
- Warning fixes
- Dirty fix of saving plots

### 2024.07.13
- Fixed pointer issues
- Added debug prints if NDEBUG is defined

### 2024.05.10
- Changed default values
- Temperature for compensation can be changed

### 2024.05.10
- Improved modularity
- Changed the interfaces
- Added temperature compensation
- Partially moved from raw pointers to C++ smart pointers
- Fit parameters can be changed
- Ability to save ROOT files
- Many changes, fixes under the hood
- Partial code cleanup, reduced duplications

# C# test application

## Prerequisities
* Dotnet Core 8.0
* Newtonsoft.JSON
* Library built

## How to run
1. Build C# app, this will create `CSharpTest/bin/Debug/net8.0` folder
2. Copy the library to the above mentioned folder (.so, .dylib, .dll)
3. Get a measurement JSON file and modify [Program.cs](CSharpTest/Program.cs) accordingly
4. Run and check the output (In [CSharpTest](CSharpTest) folder issue `dotnet run` command)

## Changelog
### 2024.07.23
- Added temperature gradient calculation and compensation

### 2024.07.13
- Added Forward Resistance and Dark Current measurements to binary file

### 2024.07.11
- Fixed positions in binary converter

### 2024.07.10
- Batch analysis
- Added new fields
- Adapt changes to the analysis

### 2024.07.05
- Added legacy binary result file converter stuff
- Added DMM resistance

### 2024.05.10
- Compatiblity changes
- Added common structs, types
- Demonstration of new interface
- Added basic temperature sensor mapping
- Added batch analysis function